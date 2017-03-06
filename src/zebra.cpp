//
//  zebra.cpp
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
#include "zebra.hpp"

Zebra::Zebra(MD2Model* model1, Terrain* terrain1, float terrainScale1, int position1) : 
	Animal(model1, terrain1, terrainScale1, position1) {

		radius0 = 0.5f;
		speed = 1.9f;
}

void Zebra::draw() {
	if (model == NULL) {
		return;
	}

	scale0 = 0.10f;

	glPushMatrix();
	glTranslatef(x0, scale0 * 10.0f + y(), z0);
	glRotatef(90 - angle * 180 / PI, 0, 1, 0);
	glColor3f(0, 0, 255);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(scale0, scale0, scale0);
	model->draw(animTime);
	glPopMatrix();
}

int Zebra::type() {
	return ZEBRA;
}
