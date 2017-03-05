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

Lion::Lion(MD2Model* model1, Terrain* terrain1, float terrainScale1) : 
	Animal(model1, terrain1, terrainScale1) {

		radius0 = 0.5f;
		speed = 2.0f;
}

void Lion::draw() {
	if (model == NULL) {
		return;
	}

	float scale = 0.12f;

	glPushMatrix();
	glTranslatef(x0, scale * 10.0f + y(), z0);
	glRotatef(90 - angle * 180 / PI, 0, 1, 0);
	glColor3f(200, 10, 200);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, scale);
	model->draw(animTime);
	glPopMatrix();
}

int Lion::type() {
	return LION;
}
