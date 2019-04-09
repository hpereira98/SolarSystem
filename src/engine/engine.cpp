﻿#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "glut.h"
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <string>
#include <vector>
#include <group.h>
#include <operation.h>
#include <translate.h>
#include <rotate.h>
#include <scale.h>
#include <hash.h>
#include <figure.h>
#include <xmlHandler.h>

#define _USE_MATH_DEFINES
#include <math.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


using namespace std;


/**
Variável global com a lista de figuras a desenhar
*/
vector<Group> groups;

/**
Função que desenha um figura recebida como parâmetro
*/
void drawModel(Figure f) {
    int color=0;
	vector<Triangle> triangles = f.get_triangles();
	glBegin(GL_TRIANGLES);
	for (vector<Triangle>::iterator it = triangles.begin(); it != triangles.end(); ++it) {
		Triangle t = *it;
        
        if(color==0)
            glColor3f(0.49,0.51,0.53);
        else
            glColor3f(0.2,0.2,0.2);
        
		glVertex3d(t.getOne().getX(), t.getOne().getY(), t.getOne().getZ());
		glVertex3d(t.getTwo().getX(), t.getTwo().getY(), t.getTwo().getZ());
		glVertex3d(t.getThree().getX(), t.getThree().getY(), t.getThree().getZ());
        
        
        color = abs(color-1);
	}
	glEnd();
}





// GLUT ------------------------------------------------------------------------------------
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0) h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void drawCoordinates() {

	glBegin(GL_LINES);
		// x
		glColor3f(1.0, 0.0, 0.0); // red x	
		glVertex3f(-40.0, 0.0f, 0.0f);
		glVertex3f(40.0, 0.0f, 0.0f);


		// y 
		glColor3f(0.0, 1.0, 0.0); // green y
		glBegin(GL_LINES);
		glVertex3f(0.0, -40.0f, 0.0f);
		glVertex3f(0.0, 40.0f, 0.0f);


		// z 
		glColor3f(0.0, 0.0, 1.0); // blue z
		glVertex3f(0.0, 0.0f, -40.0f);
		glVertex3f(0.0, 0.0f, 40.0f);

	glEnd();
}

/*float alpha = M_PI/3.4;
float beta = M_PI/6;
float radius = 200;
*/
float camX = 0; float camY = 0; float camZ = 200;
float Lx = 0; float Ly = 0; float Lz = 0;
int startX, startY, tracking = 0;
int alpha = 180, beta = 0, r = 50;


void renderGroup(vector<Figure> figs, vector<Operation*> ops, vector<Group> subGroups) {
	glPushMatrix();

    for (Operation* o : ops) {
        o->transformacao();
	}
	for (vector<Figure>::iterator it = figs.begin(); it != figs.end(); ++it) {
		Figure f = *it;
		drawModel(f);
	}
	for (vector<Group>::iterator it = subGroups.begin(); it != subGroups.end(); ++it) {
		Group g = *it;
		vector<Operation*> g_ops = g.getOperations();
		vector<Figure> g_figs = g.getFigures();
		vector<Group> g_sub_groups = g.getSubGroups();
		renderGroup(g_figs, g_ops, g_sub_groups);
	}

	glPopMatrix();
}

void renderScene(void) {

	glClearColor(20.0f, 20.0f, 20.0f, 1);
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		Lx, Ly, Lz,
		0.0f, 1.0f, 0.0f);

	glColor3b(0, 5, 20);
	
	for (vector<Group>::iterator it = groups.begin(); it != groups.end(); ++it) {
		Group g = *it;
		vector<Operation*> ops = g.getOperations();
		vector<Figure> figs = g.getFigures();
		vector<Group> subGroups = g.getSubGroups();
		renderGroup(figs, ops, subGroups);
	}

	drawCoordinates();

	// End of frame
	glutSwapBuffers();
}

/*
void processCamera(unsigned char key, int x, int y) {

	switch (key) {
	case 'A' | 'a':
		alpha += 0.1;
		break;
	case 'D' | 'd':
		alpha -= 0.1;
		break;
	case 'W' | 'w':
		if (beta < 1.5) beta += 0.1;
		break;
	case 'S' | 's':
		if (beta > -1.5) beta -= 0.1;
		break;
	case 'Q' | 'q':
		radius += 20;
		break;
	case 'E' | 'e':
		radius -= 20;
		break;
	}
	
	glutPostRedisplay();
}*/

// escrever fun��o de processamento do teclado

void processKeys(unsigned char key, int xx, int yy) {
	if (key == 'p') {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	float k;
	float dx, dy, dz;
	if (key == 'w' || key == 's') {
		if (key == 'w')
			k = 0.2;
		if (key == 's')
			k = -0.2;
		dx = Lx - camX;
		dy = Ly - camY;
		dz = Lz - camZ;

		camX = camX + k * dx;
		camY = camY + k * dy;
		camZ = camZ + k * dz;

		Lx = Lx + k * dx;
		Ly = Ly + k * dy;
		Lz = Lz + k * dz;
	}
	if (key == 'd' || key == 'a') {
		if (key == 'd') {
			k = 0.2;
			
		}
		if (key == 'a') {
			k = -0.2;
			
		}


		dx = Lx - camX;
		dy = Ly - camY;
		dz = Lz - camZ;
		float upX = 0;
		float upY = 1;
		float upZ = 0;
		float rx = dy*upZ-dz*upY;
		float rz = dx*upY-dy*upX;

		camX = camX + k * rx;
		camZ = camZ + k * rz;

		Lx = Lx + k * rx;
		Lz = Lz + k * rz;
		
	}


	
}



void processMouseButtons(int button, int state, int xx, int yy) {

	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}

}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}

	//Lx = camX + sin(alphaAux * 3.14 / 180.0);
	//Ly = camY;
	//Lz = camZ + cos(alphaAux* 3.14 / 180.0);

	Lx = camX + rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	Lz = camZ + rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	Ly = camY + rAux * sin(betaAux * 3.14 / 180.0);


}


int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Por favor insira todos os parametros necessarios. \n");
		return -1;
	}

	if (readXML(argv[1], &groups) == XML_SUCCESS){
		// init GLUT and the window
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(800, 800);
		glutCreateWindow("solar-system");

		// Required callback registry 
		glutDisplayFunc(renderScene);
		glutIdleFunc(renderScene);
		glutReshapeFunc(changeSize);

		// Callback registration for keyboard processing
		glutKeyboardFunc(processKeys);
		glutMouseFunc(processMouseButtons);
		glutMotionFunc(processMouseMotion);

		//  OpenGL settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// enter GLUT's main cycle
		glutMainLoop();

		return 1;
	}

	else return 0;
}


#pragma GCC diagnostic pop
