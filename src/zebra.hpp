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
#endif

class Zebra {
    GLfloat x,y;
    int tamanho;
public:
    Zebra();
    Zebra(GLfloat x1, GLfloat y1);
    void movimenta();
    void aumenta();
    int getTamanho();
    void setTamanho(int tamanho1);
};
#endif /* zebra_hpp */
