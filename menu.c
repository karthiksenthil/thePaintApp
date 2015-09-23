#include <GL/glut.h> 
#include<stdio.h>
#include<math.h>
static int window;
static int menu_id;
static int submenu_id;
static int value = 0; 
int points_count;
int max_points;
int end_points[4][2];
int global_primitive;

void menu(int num){
  if(num == 0){
    glutDestroyWindow(window);
    exit(0);
  }else{
    value = num;
  }
  glutPostRedisplay();
} 
void createMenu(void){
    glutPostRedisplay();
    submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Line", 2);
    glutAddMenuEntry("Circle", 3);
    glutAddMenuEntry("Triangle", 4);
    glutAddMenuEntry("Teapot", 5);     menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Clear", 1);
    glutAddSubMenu("Draw", submenu_id);
    glutAddMenuEntry("Quit", 0);     glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int distance(int x1,int y1,int x2,int y2)
{
   int d=(int)(pow(y2-y1,2)+pow(x2-x1,2));
   d=(int)pow(d,0.5);
   return d;
}

void drawLine()
{
   glColor3d(1.0, 0.0, 0.0);
   glBegin(GL_LINES);
       glVertex2i(end_points[0][0],end_points[0][1]);
       glVertex2i(end_points[1][0],end_points[1][1]);
   glEnd();
   glFlush();
}

void drawCircle(int r,int centre_x,int centre_y)
{
   glBegin(GL_POINTS);
   glVertex2f(centre_x,r+centre_y);
   glEnd();
   
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

void drawTriangle()
{
   glColor3d(1.0, 0.0, 0.0);
   glBegin(GL_LINES);
       glVertex2i(end_points[0][0],end_points[0][1]);
       glVertex2i(end_points[1][0],end_points[1][1]);
       
       glVertex2i(end_points[1][0],end_points[1][1]);
       glVertex2i(end_points[2][0],end_points[2][1]);

       glVertex2i(end_points[2][0],end_points[2][1]);
       glVertex2i(end_points[0][0],end_points[0][1]);

   glEnd();
   glFlush();
}




void capturePoints(int button,int state,int x,int y)
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
	     drawCircle(rad,end_points[0][0],end_points[0][1]);
             break;
	   }

	   case 4 : drawTriangle();
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
 
void display(void){
  glClear(GL_COLOR_BUFFER_BIT);   if(value == 1){
    return; //glutPostRedisplay();
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
    glPushMatrix();
    glColor3d(1.0, 0.0, 1.0);
    glutSolidTeapot(0.5);
    glPopMatrix();
  }
  glFlush();
} 
int main(int argc, char **argv){     glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    window = glutCreateWindow("Menus and Submenus - Programming Techniques");
    glClearColor(0.0,0.0,0.0,0.0);
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
