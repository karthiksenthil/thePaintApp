#include <GL/glut.h> 
#include<stdio.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include<pthread.h>
#include<string.h>
#include<sstream>

using namespace std;
static int window;
static int menu_id;
static int submenu_id;
static int transforms_menu_id;
static int value = 0; 
int points_count;
int save_counter;
int max_points;
int end_points[4][2];
int global_primitive;
int GLOBAL_COLOR[3];
int circle_count=0;
int triangle_count=0;
string GLOBAL_SELECTED_SHAPE;
int GLOBAL_SELECTED_ID;
int GLOBAL_transformed_points[2];

struct box
{
   int id;
   string shape;
   int minx;
   int maxx;
   int miny;
   int maxy;
   box *link; 
};


struct line
{
   int vertices[2][2];
   int color[3];
   line* link;
};

struct circle
{
   int id;
   int centre[1][2];
   int radius;
   int color[3];
   circle* link;
};

struct triangle
{
   int id;
   int vertices[3][2];
   int color[3];
   triangle* link; 
};

box* bounding_box_list;
line* line_list;
circle* circle_list;
triangle* triangle_list;

//void selected_option(int value);


void menu(int num){
  if(num == 0){
    glutDestroyWindow(window);
    exit(0);
  }
  else if(num == 1){
    save_counter=0;
    value=1;
  }
  else{
    value=num;
  }
  
  glutPostRedisplay();
} 


/*void menu_select(int button,int state,int x,int y)
{
   cout<<"Menu "<<x<<" "<<y<<"\n";
   
   if(x>0 && x<=110)
   {
	if(y>=100 && y<=140)
		menu(7);
	if(y>=140 && y<=180)
		menu(6);
	if(y>=180 && y<=220)
		menu(5);
	if(y>=220 && y<=260)
		menu(4);
	if(y>=260 && y<=300)
		menu(3);
	if(y>=300 && y<=340)
		menu(2);
	if(y>=340 && y<=380)
		menu(1);
	if(y>=380 && y<=420)
		menu(0);
   }
}*/


void createMenu(void){
    //glutPostRedisplay();
    //glPopMatrix();

   //code with glMenu and glSubMenu
    
    submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Line", 2);
    glutAddMenuEntry("Circle", 3);
    glutAddMenuEntry("Triangle", 4);
    glutAddMenuEntry("Show data", 5);     
    transforms_menu_id=glutCreateMenu(menu);
    glutAddMenuEntry("Translation",7);
    glutAddMenuEntry("Line Clipping",8);
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Save", 1);
    glutAddMenuEntry("Open",6);
    glutAddMenuEntry("Quit", 0);
    
    glutAddSubMenu("Transformers",transforms_menu_id);
    glutAddSubMenu("Draw", submenu_id);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /*char menuEntries[8][15]={"Quit","Save","Line","Circle","Triangle","Show Data","Open","Transformers"};
    int first_y=100;
    for(int i=0;i<8;i++)
    {
		glColor3d(0,0,0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(0,first_y);
		glVertex2i(110,first_y);
		glVertex2i(110,first_y+40);
		glVertex2i(0,first_y+40);
		glEnd();
		glFlush();

		glColor3d(0,0,0);
   		glRasterPos3d(0,first_y+10,0);
   		for(int j=0;j<strlen(menuEntries[i]);j++)
   		{
      			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,menuEntries[i][j]);
   		}
		
		first_y+=40;
    }

    glutMouseFunc(menu_select);*/

}


int distance(int x1,int y1,int x2,int y2)
{
   int d=(int)(pow(y2-y1,2)+pow(x2-x1,2));
   d=(int)pow(d,0.5);
   return d;
}

void drawLine()
{
   glColor3d(GLOBAL_COLOR[0],GLOBAL_COLOR[1],GLOBAL_COLOR[2]);
   glBegin(GL_LINES);
       
       line* temp=new line();
       temp->vertices[0][0]=end_points[0][0];
       temp->vertices[0][1]=end_points[0][1];
       temp->vertices[1][0]=end_points[1][0];
       temp->vertices[1][1]=end_points[1][1];
       temp->color[0]=GLOBAL_COLOR[0];
       temp->color[1]=GLOBAL_COLOR[1];
       temp->color[2]=GLOBAL_COLOR[2];
       temp->link=NULL;

       if(line_list==NULL)
	    line_list=temp;
       else
       {
	       line* traverse=line_list;
	       while(traverse->link!=NULL)
		    traverse=traverse->link;
  
		traverse->link=temp;
       } 
	
       glVertex2i(end_points[0][0],end_points[0][1]);
       glVertex2i(end_points[1][0],end_points[1][1]);
   glEnd();
   glFlush();
}

void drawCircle(int r,int centre_x,int centre_y,int circle_id,string action)
{
   glColor3d(GLOBAL_COLOR[0],GLOBAL_COLOR[1],GLOBAL_COLOR[2]);
   glBegin(GL_POINTS);
   glVertex2f(centre_x,r+centre_y);
   glEnd();

       bool transformed_flag=false;
       bool first_time_open_flag=false;
	if(action=="Transformation"){

	       circle* checker=circle_list;
	       while(checker!=NULL)
	       {
		 if(checker->id==circle_id)
		 {
			checker->centre[0][0]=centre_x;
			checker->centre[0][1]=centre_y;
			checker->radius=r;
			checker->color[0]=GLOBAL_COLOR[0];
	       		checker->color[1]=GLOBAL_COLOR[1];
	       		checker->color[2]=GLOBAL_COLOR[2];
			transformed_flag=true;
		 }
		 checker=checker->link;
	       }

       
	       box* box_checker=bounding_box_list;
	       while(box_checker!=NULL)
	       {
		  if(box_checker->id==circle_id  && box_checker->shape=="Circle")
		  {
			box_checker->minx=centre_x-r;
			box_checker->maxx=centre_x+r;
			box_checker->miny=centre_y-r;
			box_checker->maxy=centre_y+r;
			transformed_flag=true;
		  }
		  box_checker=box_checker->link;
	       }
        }
   
	if(action=="Opening")
	{
	    cout<<"Inside first time open check\n";
	    circle* first_check=circle_list;
	    cout<<"Past assignment\n";
	      while(first_check!=NULL)
	      {
		if(first_check->id == circle_id)
		   break;

		first_check=first_check->link;
              }
	     cout<<"Past the while\n";		

	      if(first_check==NULL)
		 first_time_open_flag=true;

	}


      if(!transformed_flag && circle_id!=0 && (action=="Drawing"||first_time_open_flag==true))
      {

	       cout<<"*******"<<circle_id<<"********\n";
	       circle* temp=new circle();
	       if(action=="Drawing")
	       	   temp->id=++circle_count;
	       else if(action=="Opening")
		   temp->id=circle_id;
	       circle_count++;
	       temp->centre[0][0]=centre_x;
	       temp->centre[0][1]=centre_y;
	       temp->radius=r;
	       temp->color[0]=GLOBAL_COLOR[0];
	       temp->color[1]=GLOBAL_COLOR[1];
	       temp->color[2]=GLOBAL_COLOR[2];
	       temp->link=NULL;

	       if(circle_list==NULL)
		    circle_list=temp;
	       else
	       {
		       circle* traverse=circle_list;
		       while(traverse->link!=NULL)
			    traverse=traverse->link;
	  
			traverse->link=temp;
	       }

	
		box* temp_box=new box();
		temp_box->id=temp->id;
		temp_box->shape="Circle";
		temp_box->minx=centre_x-r;
		temp_box->maxx=centre_x+r;
		temp_box->miny=centre_y-r;
		temp_box->maxy=centre_y+r;
	 	temp_box->link=NULL;

		if(bounding_box_list==NULL)
		    bounding_box_list=temp_box;
		else
		{
		       box* traverse=bounding_box_list;
		       while(traverse->link!=NULL)
			    traverse=traverse->link;
	  
			traverse->link=temp_box;
		 }
      }

      

   int h_p=centre_x+1-r;
   int x_p=0;
   int y_p=r;
   glBegin(GL_POINTS);
   int flag=1;
   while(y_p>x_p)
   {    flag++;
	if(h_p<=0)
	{
	   glVertex2f(centre_x+x_p+1,centre_y+y_p);
	   x_p++;
	   h_p=h_p+(2*x_p)+3;
        }
	else
	{
	   glVertex2f(centre_x+x_p+1,centre_y+y_p-1);
	   x_p++;
	   y_p--;
	   h_p=h_p+(2*x_p)-(2*y_p)+5;
	}

	
	glVertex2f(centre_x+y_p,centre_y+x_p);
	glVertex2f(centre_x-y_p,centre_y+x_p);
	glVertex2f(centre_x-x_p,centre_y+y_p);
	glVertex2f(centre_x-x_p,centre_y-y_p);
	glVertex2f(centre_x-y_p,centre_y-x_p);
	glVertex2f(centre_x-y_p,centre_y+x_p);
	glVertex2f(centre_x+x_p,centre_y-y_p);
	glVertex2f(centre_x+y_p,centre_y-x_p);
	
   }
   glEnd();
   glFlush();

   

}

void drawTriangle(int triangle_id,string action)
{

   glColor3d(GLOBAL_COLOR[0],GLOBAL_COLOR[1],GLOBAL_COLOR[2]);
   glBegin(GL_LINES);

       bool transformed_flag=false;
       bool first_time_open_flag=false;
	if(action=="Transformation"){

	       triangle* checker=triangle_list;
	       while(checker!=NULL)
	       {
		 if(checker->id==triangle_id)
		 {
			checker->vertices[0][0]=end_points[0][0];
			checker->vertices[0][1]=end_points[0][1];
			checker->vertices[1][0]=end_points[1][0];
			checker->vertices[1][1]=end_points[1][1];
			checker->vertices[2][0]=end_points[2][0];
			checker->vertices[2][1]=end_points[2][1];
			transformed_flag=true;
		 }
		 checker=checker->link;
	       }
	
       
	       box* box_checker=bounding_box_list;
	       while(box_checker!=NULL)
	       {
		  if(box_checker->id==triangle_id && box_checker->shape=="Triangle")
		  {
			box_checker->minx=min(end_points[0][0],min(end_points[1][0],end_points[2][0]));
			box_checker->maxx=max(end_points[0][0],max(end_points[1][0],end_points[2][0]));
			box_checker->miny=min(end_points[0][1],min(end_points[1][1],end_points[2][1]));
			box_checker->maxy=max(end_points[0][1],max(end_points[1][1],end_points[2][1]));
			transformed_flag=true;
		  }
		  box_checker=box_checker->link;
	       }
        }		


	if(action=="Opening")
	{
	    cout<<"Inside first time open check triangle\n";
	    triangle* first_check=triangle_list;
	    cout<<"Past assignment\n";
	      while(first_check!=NULL)
	      {
		if(first_check->id == triangle_id)
		   break;

		first_check=first_check->link;
              }
	     cout<<"Past the while\n";		

	      if(first_check==NULL)
		 first_time_open_flag=true;

	}


       if(!transformed_flag && triangle_id!=0 && (action=="Drawing"||first_time_open_flag==true))
       {

	       triangle* temp=new triangle();
	       if(action=="Drawing")
	       	temp->id=++triangle_count;
	       else if(action=="Opening")
		temp->id=triangle_id;
	       triangle_count++;
	       temp->vertices[0][0]=end_points[0][0];
	       temp->vertices[0][1]=end_points[0][1];
	       temp->vertices[1][0]=end_points[1][0];
	       temp->vertices[1][1]=end_points[1][1];
	       temp->vertices[2][0]=end_points[2][0];
	       temp->vertices[2][1]=end_points[2][1];
	       temp->color[0]=GLOBAL_COLOR[0];
	       temp->color[1]=GLOBAL_COLOR[1];
	       temp->color[2]=GLOBAL_COLOR[2];
	       temp->link=NULL;

	       if(triangle_list==NULL)
		    triangle_list=temp;
	       else
	       {
		       triangle* traverse=triangle_list;
		       while(traverse->link!=NULL)
			    traverse=traverse->link;
	  
			traverse->link=temp;
	       }


		box* temp_box=new box();
		temp_box->id=temp->id;
		temp_box->shape="Triangle";

		temp_box->minx=min(end_points[0][0],min(end_points[1][0],end_points[2][0]));
		temp_box->maxx=max(end_points[0][0],max(end_points[1][0],end_points[2][0]));

		temp_box->miny=min(end_points[0][1],min(end_points[1][1],end_points[2][1]));
		temp_box->maxy=max(end_points[0][1],max(end_points[1][1],end_points[2][1]));

	 	temp_box->link=NULL;

		if(bounding_box_list==NULL)
		    bounding_box_list=temp_box;
		else
		{
		       box* traverse=bounding_box_list;
		       while(traverse->link!=NULL)
			    traverse=traverse->link;
	  
			traverse->link=temp_box;
		 }
      }

      

       glVertex2i(end_points[0][0],end_points[0][1]);
       glVertex2i(end_points[1][0],end_points[1][1]);
       
       glVertex2i(end_points[1][0],end_points[1][1]);
       glVertex2i(end_points[2][0],end_points[2][1]);

       glVertex2i(end_points[2][0],end_points[2][1]);
       glVertex2i(end_points[0][0],end_points[0][1]);

   glEnd();
   glFlush();
}

void getColor(int x,int y)
{
   cout<<"outside"<<x<<y<<"\n";
   if(y>=0 && y<=30)
   {
	if(x>=120 && x<=150)
	{
	    GLOBAL_COLOR[0]=0;
	    GLOBAL_COLOR[1]=0;
	    GLOBAL_COLOR[2]=0;
	}
	else if(x>=150 && x<=180)
	{
	    GLOBAL_COLOR[0]=0;
	    GLOBAL_COLOR[1]=0;
	    GLOBAL_COLOR[2]=1;
	}
	else if(x>=180 && x<=210)
	{
	    GLOBAL_COLOR[0]=0;
	    GLOBAL_COLOR[1]=1;
	    GLOBAL_COLOR[2]=0;
	}
	else if(x>=210 && x<=240)
	{
	    GLOBAL_COLOR[0]=0;
	    GLOBAL_COLOR[1]=1;
	    GLOBAL_COLOR[2]=1;
	}
	else if(x>=240 && x<=270)
	{
	    GLOBAL_COLOR[0]=1;
	    GLOBAL_COLOR[1]=0;
	    GLOBAL_COLOR[2]=0;
	    cout<<"Red\n";
	}
	else if(x>=270 && x<=300)
	{
	    GLOBAL_COLOR[0]=1;
	    GLOBAL_COLOR[1]=0;
	    GLOBAL_COLOR[2]=1;
	}
	else if(x>=300 && x<=330)
	{
	    GLOBAL_COLOR[0]=1;
	    GLOBAL_COLOR[1]=1;
	    GLOBAL_COLOR[2]=0;
	}
   }
}




void capturePoints(int button,int state,int x,int y)
{


  if(x>=120 && x<=330 && y<=30)
  {
	getColor(x,y);
  } 
  else
  {
   if(points_count==0 || points_count==1)
	points_count++;
   else if((points_count%2==1) && (points_count < (2*max_points)+1))
   {
	printf("count=%d\n",points_count);
	points_count++;
   }
   else if((points_count%2==1) && (points_count==(2*max_points)+1))
   {
	printf("count=%d\n",points_count);
	points_count=2;
   }
   else if(points_count==2*max_points)
   {
	printf("need to add the points\n");
	printf("count=%d\n",points_count);
	end_points[(points_count/2)-1][0]=x;
	end_points[(points_count/2)-1][1]=500-y;
	points_count++;
	printf("%d %d\n",end_points[0][0],end_points[0][1]);
	printf("%d %d\n",end_points[1][0],end_points[1][1]);

	switch(global_primitive)
	{
	   case 2 : drawLine();
		    break;

	   case 3 : 
           { 
	     int rad=distance(end_points[0][0],end_points[0][1],end_points[1][0],end_points[1][1]);
             printf("%d\n",rad);
	     drawCircle(rad,end_points[0][0],end_points[0][1],-1,"Drawing");
             break;
	   }

	   case 4 : drawTriangle(-1,"Drawing");
		    break;

	}
	
   }
   else
   {
	printf("need to add the points\n");
	printf("count=%d\n",points_count);
	end_points[(points_count/2)-1][0]=x;
	end_points[(points_count/2)-1][1]=500-y;
	points_count++;
   }

  }
  
}

void drawLoadingSymbol()
{
   save_counter++;
   glColor3d(1,0,0);
   glRasterPos3f(210.0,290.0,0);
   char loading[11]="SAVING...";
   for(int i=0;i<10;i++)
   {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,loading[i]);
   }
   glBegin(GL_LINES);
   	glVertex2i(250,260);
	glVertex2i(250,280);
   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(257,257);
	glVertex2i(271,271);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(260,250);
	glVertex2i(280,250);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(257,243);
	glVertex2i(271,229);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(250,240);
	glVertex2i(250,220);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(243,243);
	glVertex2i(229,229);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(240,250);
	glVertex2i(220,250);

   glEnd();
   glFlush();
   sleep(1);
   glBegin(GL_LINES);	
        glVertex2i(243,257);
	glVertex2i(229,271);

   glEnd();
   glFlush();

   glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glRasterPos3f(200.0,290.0,0);
   char saved[17]="PRIMITIVES SAVED";
   for(int i=0;i<16;i++)
   {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,saved[i]);
   }
   
}

void open_file()
{
  cout<<"Opening contents of file\n";
  string line;
  fstream file_reader;
  file_reader.open("primitives.txt");

  while(getline(file_reader,line))
  {     cout<<"inside\n";
	if(line=="Lines:")
	{
	   getline(file_reader,line);
	   while(line!="Circles:")
	   {
		if(line=="")
		{
		   getline(file_reader,line);
		   cout<<line;
		   continue;
		}
		string arr[7];
	    	int i = 0;
    		stringstream ssin(line);
    		while (ssin.good() && i < 7){
        		ssin >> arr[i];
        		++i;
    		}
		for(i = 0; i < 7; i++){
		  cout << arr[i] <<" ";
		}
		end_points[0][0]=atoi(arr[0].c_str());
	        end_points[0][1]=atoi(arr[1].c_str());
		end_points[1][0]=atoi(arr[2].c_str());
		end_points[1][1]=atoi(arr[3].c_str());
		GLOBAL_COLOR[0]=atoi(arr[4].c_str());
		GLOBAL_COLOR[1]=atoi(arr[5].c_str());
		GLOBAL_COLOR[2]=atoi(arr[6].c_str());
		drawLine();
		cout<<endl;
		getline(file_reader,line);
	   }
        }
	
	if(line=="Circles:")
	{
	  getline(file_reader,line);
	   while(line!="Triangles:")
	   {
		cout<<line<<"\n";
		if(line=="")
		{
		   getline(file_reader,line);
		   cout<<line;
		   continue;
		}

		string arr[7];
	    	int i = 0;
    		stringstream ssin(line);
    		while (ssin.good() && i < 7){
        		ssin >> arr[i];
        		++i;
    		}
		for(i = 0; i < 7; i++){
		  cout << atoi(arr[i].c_str())<<" ";
		}
                GLOBAL_COLOR[0]=atoi(arr[4].c_str());
		GLOBAL_COLOR[1]=atoi(arr[5].c_str());
		GLOBAL_COLOR[2]=atoi(arr[6].c_str());
		drawCircle(atoi(arr[3].c_str()),atoi(arr[1].c_str()),atoi(arr[2].c_str()),atoi(arr[0].c_str()),"Opening");   //Remember to write id of circle to file also
		cout<<endl;
		getline(file_reader,line);
	   }
		
	}


	if(line=="Triangles:")
	{
	   while(getline(file_reader,line))
	   {
		string arr[10];
	    	int i = 0;
    		stringstream ssin(line);
    		while (ssin.good() && i < 10){
        		ssin >> arr[i];
        		++i;
    		}
		for(i = 0; i < 10; i++){
		  cout << arr[i] <<" ";
		}
		end_points[0][0]=atoi(arr[1].c_str());
	        end_points[0][1]=atoi(arr[2].c_str());
		end_points[1][0]=atoi(arr[3].c_str());
		end_points[1][1]=atoi(arr[4].c_str());
		end_points[2][0]=atoi(arr[5].c_str());
		end_points[2][1]=atoi(arr[6].c_str());
		GLOBAL_COLOR[0]=atoi(arr[7].c_str());
		GLOBAL_COLOR[1]=atoi(arr[8].c_str());
		GLOBAL_COLOR[2]=atoi(arr[9].c_str());
		drawTriangle(atoi(arr[0].c_str()),"Opening");
		cout<<endl;
	   }
        }
        

  }
}

void color_picker(int button,int state,int x,int y)
{
  cout<<"color\n";
  getColor(x,y);
}

void matrix_multiplier(int first[3][3],int second[3][1])
{
  int product[3][1];
  for(int i=0;i<3;i++)
  {
	for(int j=0;j<1;j++)
	{
	     int sum=0;
	     for(int k=0;k<3;k++)
	     	sum+=first[i][k]*second[k][j];

	product[i][j]=sum;
	}
  }

  cout<<"Matrix mult product:\n";
  cout<<product[0][0]<<" "<<product[1][0]<<"\n";


  GLOBAL_transformed_points[0]=product[0][0];
  GLOBAL_transformed_points[1]=product[1][0];

}



void primitive_translator(int x,int y,int prevx,int prevy)
{
    cout<<"Translating...\n";
    if(GLOBAL_SELECTED_SHAPE=="Circle")
    {
	 circle* temp=circle_list;
	 
	 while(temp->id != GLOBAL_SELECTED_ID)
		temp=temp->link;

	int i,j,k;
	i=temp->color[0];
	j=temp->color[1];
	k=temp->color[2];
	//Code to translate written using glTranslate
	 /*glMatrixMode(GL_MODELVIEW);
  	 glLoadIdentity();
  	 glTranslated(x-temp->centre[0][0],y-temp->centre[0][1],0);
	 glPushMatrix();
	 GLOBAL_COLOR[0]=temp->color[0];
	 GLOBAL_COLOR[1]=temp->color[1];
	 GLOBAL_COLOR[2]=temp->color[2];
  	 drawCircle(temp->radius,temp->centre[0][0],temp->centre[0][1],temp->id,"Transforming");
	 glPopMatrix();

	 
	 //glTranslated(-(x-temp->centre[0][0]),-(y-temp->centre[0][1]),0);
	 //glPushMatrix();
	 
	   //drawCircle(x,y,temp->radius);
	 glFlush();*/


         
	 GLOBAL_COLOR[0]=1;
	 GLOBAL_COLOR[1]=1;
	 GLOBAL_COLOR[2]=1;
	 drawCircle(temp->radius,temp->centre[0][0],temp->centre[0][1],temp->id,"Opening");
	 cout<<"\nDrawn old circle\n";
         int transform[3][3]={{1,0,x-temp->centre[0][0]},{0,1,y-temp->centre[0][1]},{0,0,1}};
	 int old[3][1]={{temp->centre[0][0]},{temp->centre[0][1]},{1}};
	 matrix_multiplier(transform,old);
	 cout<<temp->color[0]<<temp->color[1]<<temp->color[2]<<"\n";
	 GLOBAL_COLOR[0]=i;
	 GLOBAL_COLOR[1]=j;
	 GLOBAL_COLOR[2]=k;
	 cout<<GLOBAL_COLOR[0]<<GLOBAL_COLOR[1]<<GLOBAL_COLOR[2]<<"\n";
	 drawCircle(temp->radius,GLOBAL_transformed_points[0],GLOBAL_transformed_points[1],temp->id,"Transformation");
	
	
    }
    else if(GLOBAL_SELECTED_SHAPE=="Triangle")
    {
        int translate_x,translate_y;
	triangle* temp=triangle_list;
	while(temp->id != GLOBAL_SELECTED_ID)
		temp=temp->link;

	translate_x=x-prevx;
	translate_y=y-prevy;

	cout<<prevx<<" "<<prevy<<"\n";

	//Code to translate triangle using glTranslate
	/*glMatrixMode(GL_MODELVIEW);
  	 glLoadIdentity();
  	 glTranslated(translate_x,translate_y,0);
	 glPushMatrix();
	 end_points[0][0]=temp->vertices[0][0];
	 end_points[0][1]=temp->vertices[0][1];
	 end_points[1][0]=temp->vertices[1][0];
	 end_points[1][1]=temp->vertices[1][1];
	 end_points[2][0]=temp->vertices[2][0];
	 end_points[2][1]=temp->vertices[2][1];
	 GLOBAL_COLOR[0]=temp->color[0];
	 GLOBAL_COLOR[1]=temp->color[1];
	 GLOBAL_COLOR[2]=temp->color[2];
  	 drawTriangle(temp->id,"Transforming");
	 glPopMatrix();
	 
	   //drawCircle(x,y,temp->radius);
	 glFlush();*/

	 int i,j,k;
	 i=temp->color[0];
	 j=temp->color[1];
	 k=temp->color[2];

 	 end_points[0][0]=temp->vertices[0][0];
	 end_points[0][1]=temp->vertices[0][1];
	 end_points[1][0]=temp->vertices[1][0];
	 end_points[1][1]=temp->vertices[1][1];
	 end_points[2][0]=temp->vertices[2][0];
	 end_points[2][1]=temp->vertices[2][1];
	 GLOBAL_COLOR[0]=1;
	 GLOBAL_COLOR[1]=1;
	 GLOBAL_COLOR[2]=1;
  	 drawTriangle(temp->id,"Opening");


	int transform[3][3];
	int old[3][1];


	 transform[0][0]=1;
	 transform[0][1]=0;
	 transform[0][2]=translate_x;
	 transform[1][0]=0;
	 transform[1][1]=1;
	 transform[1][2]=translate_y;
	 transform[2][0]=0;
	 transform[2][1]=0;
	 transform[2][2]=1;
	 old[0][0]=temp->vertices[0][0];
	 old[1][0]=temp->vertices[0][1];
	 old[2][0]=1;
	 matrix_multiplier(transform,old);
	 end_points[0][0]=GLOBAL_transformed_points[0];
	 end_points[0][1]=GLOBAL_transformed_points[1];


	 transform[0][0]=1;
	 transform[0][1]=0;
	 transform[0][2]=translate_x;
	 transform[1][0]=0;
	 transform[1][1]=1;
	 transform[1][2]=translate_y;
	 transform[2][0]=0;
	 transform[2][1]=0;
	 transform[2][2]=1;
	 old[0][0]=temp->vertices[1][0];
	 old[1][0]=temp->vertices[1][1];
	 old[2][0]=1;
	 matrix_multiplier(transform,old);
	 end_points[1][0]=GLOBAL_transformed_points[0];
	 end_points[1][1]=GLOBAL_transformed_points[1];


	 transform[0][0]=1;
	 transform[0][1]=0;
	 transform[0][2]=translate_x;
	 transform[1][0]=0;
	 transform[1][1]=1;
	 transform[1][2]=translate_y;
	 transform[2][0]=0;
	 transform[2][1]=0;
	 transform[2][2]=1;
	 old[0][0]=temp->vertices[2][0];
	 old[1][0]=temp->vertices[2][1];
	 old[2][0]=1;
	 matrix_multiplier(transform,old);
	 end_points[2][0]=GLOBAL_transformed_points[0];
	 end_points[2][1]=GLOBAL_transformed_points[1];

	 GLOBAL_COLOR[0]=i;
	 GLOBAL_COLOR[1]=j;
	 GLOBAL_COLOR[2]=k;
  	 drawTriangle(temp->id,"Transformation");



    }
  
}


int prevx=0;
int prevy=0;


void primitive_selector(int button,int state,int x,int y)
{
   y=500-y;
   
   if(points_count==0 || points_count==1)
	points_count++;

   else if(points_count==2)
   {
	  cout<<"Going to select...\n";
	   box* traversal=bounding_box_list;
	   
	   while(traversal!=NULL)
	   {
	        cout<<x<<"\n";
		if(x>=traversal->minx && x<=traversal->maxx)
		{
			cout<<y<<"\n";
			cout<<traversal->miny<<" "<<traversal->maxy<<"\n";
			if(y>=traversal->miny && y<=traversal->maxy)
			{
				cout<<"*******Selected the shape "<<traversal->shape<<" *******\n";
				GLOBAL_SELECTED_SHAPE=traversal->shape;
				GLOBAL_SELECTED_ID=traversal->id;
				prevx=x;
				prevy=y;
			}
		}

		traversal=traversal->link;
	   }
	   
   }
   
   else if(points_count==4)
   {
	cout<<"Previous x,y: ";
	cout<<prevx<<" "<<prevy<<"\n";
	primitive_translator(x,y,prevx,prevy);
	points_count=-1;	
   }

   points_count++;

}

void line_clipping(int corner1_x,int corner1_y,int corner2_x,int corner2_y)
{
   cout<<corner1_x<<" "<<corner1_y<<" "<<corner2_x<<" "<<corner2_y<<"\n";

   glColor3d(0,0,0);
   glBegin(GL_LINES);
	glVertex2i(corner1_x,corner1_y);
	glVertex2i(corner2_x,corner1_y);   	
	glVertex2i(corner2_x,corner1_y);
	glVertex2i(corner2_x,corner2_y);
        glVertex2i(corner2_x,corner2_y);
	glVertex2i(corner1_x,corner2_y);
	glVertex2i(corner1_x,corner2_y);
	glVertex2i(corner1_x,corner1_y);

   glEnd();
   glFlush();

   triangle* triangle_clip=triangle_list;
   while(triangle_clip!=NULL)
   {
	
	for(int i=0;i<3;i++)
	{
	     line* new_line=new line();
	     
	     new_line->vertices[0][0]=triangle_clip->vertices[i][0];
      	     new_line->vertices[0][1]=triangle_clip->vertices[i][1];

	     new_line->vertices[1][0]=triangle_clip->vertices[(i+1)%3][0];
      	     new_line->vertices[1][1]=triangle_clip->vertices[(i+1)%3][1];

	     new_line->color[0]=triangle_clip->color[0];
	     new_line->color[1]=triangle_clip->color[1];
	     new_line->color[2]=triangle_clip->color[2];

	     new_line->link=NULL;


	     if(line_list==NULL)
	    	line_list=new_line;
       	     else
             {
	        line* traverse=line_list;
	        while(traverse->link!=NULL)
		    traverse=traverse->link;
  
		traverse->link=new_line;
             } 

	}

      triangle_clip=triangle_clip->link;
   }



   float slope;
   line* line_traverse=line_list;
   
   while(line_traverse!=NULL)
   {
	slope=(float)(line_traverse->vertices[1][1]-line_traverse->vertices[0][1])/(float)(line_traverse->vertices[1][0]-line_traverse->vertices[0][0]);

	cout<<"Slope :"<<slope<<"\n";

        //flags for each side
	bool left=false;
	bool right=false;
	bool top=false;
	bool bottom=false;


	//along left edge of clipping box
	int intersecting_y_left=line_traverse->vertices[0][1]+(slope*(corner1_x-line_traverse->vertices[0][0]));
	if(intersecting_y_left<=corner1_y && intersecting_y_left>=corner2_y)
	{
	    left=true;
	    glColor3d(1,1,1);
	    glLineWidth(2);
	    glBegin(GL_LINES);
	         glVertex2i(corner1_x,intersecting_y_left);
		 if(line_traverse->vertices[0][0]<line_traverse->vertices[1][0])
		 {
		      glVertex2i(line_traverse->vertices[0][0],line_traverse->vertices[0][1]);
		 }
		 else
 		 {
		      glVertex2i(line_traverse->vertices[1][0],line_traverse->vertices[1][1]); 
		 } 
	     glEnd();
	     glFlush();

	}


        //along right edge of clipping box
	int intersecting_y_right=line_traverse->vertices[0][1]+(slope*(corner2_x-line_traverse->vertices[0][0]));
	cout<<intersecting_y_right<<"\n";
	if(intersecting_y_right<=corner1_y && intersecting_y_right>=corner2_y)
	{
	    right=true;
	    cout<<"Inside clip part\n"; 
	    glColor3d(1,1,1);
	    glBegin(GL_LINES);
	         glVertex2i(corner2_x,intersecting_y_right);
		 if(line_traverse->vertices[0][0]<line_traverse->vertices[1][0])
		 {
		      glVertex2i(line_traverse->vertices[1][0],line_traverse->vertices[1][1]);
		 }
		 else
 		 {
		      glVertex2i(line_traverse->vertices[0][0],line_traverse->vertices[0][1]); 
		 } 
	     glEnd();
	     glFlush();

	}


	//along bottom edge of clipping box
	int intersecting_x_bottom=line_traverse->vertices[0][0]+((corner2_y-line_traverse->vertices[0][1])/slope);
        cout<<intersecting_x_bottom<<"\n";
	if(intersecting_x_bottom>=corner1_x && intersecting_x_bottom<=corner2_x)
        {
	     bottom=true;
             glColor3d(1,1,1);
	     glBegin(GL_LINES);
		  glVertex2i(intersecting_x_bottom,corner2_y);
		  if(line_traverse->vertices[0][1]<line_traverse->vertices[1][0])
		  {
			glVertex2i(line_traverse->vertices[0][0],line_traverse->vertices[0][1]);
		  }
		  else
		  {
			glVertex2i(line_traverse->vertices[1][0],line_traverse->vertices[1][1]);
		  }
	     glEnd();
	     glFlush();
        }


	//along top edge of clipping box
	int intersecting_x_top=line_traverse->vertices[0][0]+((corner1_y-line_traverse->vertices[0][1])/slope);
        cout<<intersecting_x_top<<"\n";
	if(intersecting_x_top>=corner1_x && intersecting_x_top<=corner2_x)
        {
	     top=true;
             glColor3d(1,1,1);
	     glBegin(GL_LINES);
		  glVertex2i(intersecting_x_top,corner1_y);
		  if(line_traverse->vertices[0][1]<line_traverse->vertices[1][1])
		  {
			glVertex2i(line_traverse->vertices[1][0],line_traverse->vertices[1][1]);
		  }
		  else
		  {
			glVertex2i(line_traverse->vertices[0][0],line_traverse->vertices[0][1]);
		  }
	     glEnd();
	     glFlush();
        }


	if(!left && !right && !top && !bottom)
	{
	    glColor3d(1,1,1);
	    glBegin(GL_LINES);
	    	glVertex2i(line_traverse->vertices[0][0],line_traverse->vertices[0][1]);
		glVertex2i(line_traverse->vertices[1][0],line_traverse->vertices[1][1]);
  	    glEnd();
	    glFlush();
	}

	

	line_traverse=line_traverse->link;
   }



}



int clipping_window_corners[2][2];

void clipping_window_selector(int button,int state,int x,int y)
{
   y=500-y;

   if(points_count==0 || points_count==1)
	points_count++;

   else if(points_count==2)
   {
	cout<<"Selected first corner of clip :"<<x<<" "<<y<<"\n";
	clipping_window_corners[0][0]=x;
	clipping_window_corners[0][1]=y;
   }
   else if(points_count==4)
   {
	cout<<"Selected second corner of clip :"<<x<<" "<<y<<"\n";
	clipping_window_corners[1][0]=x;
	clipping_window_corners[1][1]=y;

	line_clipping(clipping_window_corners[0][0],clipping_window_corners[0][1],clipping_window_corners[1][0],clipping_window_corners[1][1]);

        points_count=-1;
   }
   points_count++;

}



void transformations_window()
{
	  open_file();
	  points_count=0;
	  glutMouseFunc(primitive_selector);
}


void clipping_function()
{
          open_file();
	  points_count=0;
	  glutMouseFunc(clipping_window_selector);
}


/*void drawLine(int button,int state,int x,int y)
{
   if(points_count==0 || points_count==1)
	points_count++;
   if(points_count==2)
   {
	printf("count=1\n");
	end_points[0][0]=x;
	end_points[0][1]=500-y;
	points_count++;
   }
   else if(points_count==3)
   {
	printf("count=2\n");
	points_count++;
   }
   else if(points_count==4)
   {
	printf("count=3\n");
	end_points[1][0]=x;
	end_points[1][1]=500-y;
	points_count++;
        printf("%d %d\n",end_points[0][0],end_points[0][1]);
	printf("%d %d\n",end_points[1][0],end_points[1][1]);
	glPushMatrix();
	glColor3d(1.0, 0.0, 0.0);
	glLineWidth(3.0);
	glBegin(GL_LINES);
	    glVertex2i(end_points[0][0],end_points[0][1]);
	    glVertex2i(end_points[1][0],end_points[1][1]);
	glEnd();
	glFlush();
	glPopMatrix();

   }
   else if(points_count==5)
   {
	printf("count=4\n");
	points_count=2;
   }
	

}*/
 
void display(){
  glClear(GL_COLOR_BUFFER_BIT);
   
   glColor3d(0,0,0);
   glRasterPos3f(15,480,0);
   char loading[10]="COLOURS: ";
   for(int i=0;i<10;i++)
   {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,loading[i]);
   }

  int first_x=120;
  for(int i=0;i<2;i++)
  {
      for(int j=0;j<2;j++)
      {
	 for(int k=0;k<2;k++)
	 {
                glColor3d(0,0,0);
		glBegin(GL_LINE_LOOP);
		glVertex2i(first_x,500);
		glVertex2i(first_x+30,500);
		glVertex2i(first_x+30,470);
		glVertex2i(first_x,470);
		glEnd();

		glColor3d(i,j,k);
		glBegin(GL_POLYGON);
		glVertex2i(first_x,500);
		glVertex2i(first_x+30,500);
		glVertex2i(first_x+30,470);
		glVertex2i(first_x,470);
		glEnd();
		first_x+=30;
	        
         }
      }
  }
  glutMouseFunc(color_picker);
 

   if(value == 1){
    //glutPostRedisplay();

   if(save_counter<1)
   {
    drawLoadingSymbol();
    cout<<"Writing to the file\n";
    ofstream myfile;
    myfile.open("primitives.txt");
    myfile<<"\nLines:\n";
    line* temp=line_list;
    while(temp!=NULL)
    {
	cout<<"writing lines\n";
	myfile<<temp->vertices[0][0]<<" "<<temp->vertices[0][1]<<" ";
	myfile<<temp->vertices[1][0]<<" "<<temp->vertices[1][1]<<" ";
	myfile<<temp->color[0]<<" "<<temp->color[1]<<" "<<temp->color[2]<<"\n";
	temp=temp->link;
    }
    myfile<<"\nCircles:\n";
    circle* temp1=circle_list;
    while(temp1!=NULL)
    {
	cout<<"writing circles\n";
	myfile<<temp1->id<<" ";
	myfile<<temp1->centre[0][0]<<" "<<temp1->centre[0][1]<<" ";
	myfile<<temp1->radius<<" ";
	myfile<<temp1->color[0]<<" "<<temp1->color[1]<<" "<<temp1->color[2]<<"\n";
	temp1=temp1->link;
    }
    myfile<<"\nTriangles:\n";
    triangle* temp2=triangle_list;
    while(temp2!=NULL)
    {
	cout<<"writing triangles\n";
	myfile<<temp2->id<<" ";
	myfile<<temp2->vertices[0][0]<<" "<<temp2->vertices[0][1]<<" ";
	myfile<<temp2->vertices[1][0]<<" "<<temp2->vertices[1][1]<<" ";
	myfile<<temp2->vertices[2][0]<<" "<<temp2->vertices[2][1]<<" ";
	myfile<<temp2->color[0]<<" "<<temp2->color[1]<<" "<<temp2->color[2]<<"\n";
	temp2=temp2->link;
    }

    myfile.close();
   }

  }else if(value == 2){
 
    points_count=0;
    max_points=2;
    global_primitive=2;
    glutMouseFunc(capturePoints);
    
  }else if(value == 3){
    
    points_count=0;
    max_points=2;
    global_primitive=3;
    glutMouseFunc(capturePoints);

  }else if(value == 4){
    
    points_count=0;
    max_points=3;
    global_primitive=4;
    glutMouseFunc(capturePoints);    

  }else if(value == 5){
   cout<<"Checking the linked list line:\n";
   line* temp=line_list;
   while(temp!=NULL)
   {
	cout<<temp->vertices[0][0]<<" "<<temp->vertices[0][1]<<endl;
	temp=temp->link;
   }
   cout<<"Checking the linked list circle:\n";
   circle* temp1=circle_list;
   while(temp1!=NULL)
   {
	cout<<temp1->centre[0][0]<<" "<<temp1->centre[0][1]<<endl;
	cout<<temp1->radius<<endl;
	temp1=temp1->link;
   }
   cout<<"Checking the linked list triangle:\n";
   triangle* temp2=triangle_list;
   while(temp2!=NULL)
   {
	cout<<temp2->vertices[0][0]<<" "<<temp2->vertices[0][1]<<endl;
	temp2=temp2->link;
   }

   cout<<"Checking the linked list of bounding boxes:\n";
   box* temp3=bounding_box_list;
   while(temp3!=NULL)
   {
	cout<<temp3->id<<" "<<temp3->shape<<" "<<temp3->minx<<" "<<temp3->miny<<endl;
	temp3=temp3->link;
   }



  }

  else if(value == 6){
    open_file();
  }

  else if(value == 7){
    save_counter=0;
    transformations_window();
  }

  else if(value==8){
    save_counter=0;
    clipping_function();
  }


  glFlush();
} 
int main(int argc, char **argv){     glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    window = glutCreateWindow("The Rotating Teapot Paint Application");
    glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
  // setup viewing projection
    glLoadIdentity();
  // start with identity matrix
    glOrtho(0.0, 500.0, 0.0, 500.0,-1.0,1.0);
    glutDisplayFunc(display);     
    createMenu();     
    glutMainLoop();
    return EXIT_SUCCESS;
}
