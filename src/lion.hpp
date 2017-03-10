//
//  lion.hpp
//  Island
//
//  Created by Diego Andrade on 17/02/17.
//  Copyright © 2017 Diego Andrade. All rights reserved.
//

#ifndef lion_hpp
#define lion_hpp

#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#endif
#include "animal.h"
#include "carregadorObj.cpp"

class Lion : public Animal {
public:
	Lion(Modelo* model1,
		Terrain* terrain1,
		float terrainScale1,
		int position1);

	virtual void draw();
	virtual int type();
};
#endif /* lion_hpp */
