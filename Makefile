#! /bin/bash

#compiling the 3D logo part
gcc rotating_logo.c -lGL -lGLU -lglut -lm

#compiling the main paint application
g++ -o paint paint_application.cpp -lGL -lglut -lm -lpthread



