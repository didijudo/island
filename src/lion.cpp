//
//  lion.cpp
//  Island
//
//  Created by Diego Andrade on 17/02/17.
//  Copyright © 2017 Diego Andrade. All rights reserved.
//
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#endif
#include "lion.hpp"

Lion::Lion(Modelo* model1, Terrain* terrain1, float terrainScale1, int position1) : 
	Animal(model1, terrain1, terrainScale1, position1) {

		radius0 = 0.5f;
		speed = 3.0f;
}

void Lion::draw() {
	if (model == NULL) {
		return;
	}

	glPushMatrix();
	glTranslatef(x0, scale0 * 1.0f + y(), z0);
	glRotatef(90 - angle * 180 / PI, 0, 1, 0);
	//glColor3f(255, 0, 0);
	//glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	//glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.01f, 0.01f, 0.01f);
	//model->draw(animTime);
	model->desenhar();
	glPopMatrix();
}

int Lion::type() {
	return LION;
}
