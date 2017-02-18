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
#endif

#include "zebra.hpp"

Zebra::Zebra() {

}

Zebra::Zebra(GLfloat x1, GLfloat y1) {
    x = x1;
    y = y1;
    tamanho = 10;
    glBegin(GL_QUADS);
    glVertex2f(x, y+tamanho);
    glVertex2f(x, y);
    glVertex2f(x + tamanho, y);
    glVertex2f(x + tamanho, y + tamanho);
    glEnd();
}

void Zebra::movimenta() {
    //Logica para movimentacao da zebra
}

void Zebra::aumenta() {
    printf("Testando o aumento %d \n", tamanho);
    setTamanho(tamanho + 5);
    printf("Testando depois %d \n", tamanho);
}

int Zebra::getTamanho() {
    return tamanho;
}

void Zebra::setTamanho(int tamanho1) {
    tamanho = tamanho1;
}
