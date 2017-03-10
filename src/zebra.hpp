//
//  zebra.hpp
//  Island
//
//  Created by Diego Andrade on 17/02/17.
//  Copyright © 2017 Diego Andrade. All rights reserved.
//

#ifndef zebra_hpp
#define zebra_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#endif
#include "animal.h"

class Zebra : public Animal {
public:
	Zebra(Modelo* model1,
		Terrain* terrain1,
		float terrainScale1,
		int position1);

	virtual void draw();
	virtual int type();
};
#endif /* zebra_hpp */
