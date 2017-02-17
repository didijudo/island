//
//  main.cpp
//  EstudoOpenGL
//
//  Created by Diego Andrade on 25/01/17.
//  Copyright © 2017 Diego Andrade. All rights reserved.
//
#include <iostream>

#ifdef __APPLE__

#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>

#endif

#ifdef __WINDOWS__ | __LINUX__
#include <GL/gl.h>
#include <GLUT/gl.h>
#endif

void render(void);
void keyboard(unsigned char c, int x, int y);
void mouse(int button, int state, int x, int y);
void special(GLint c, GLint x, GLint y);
void inicializa();

GLfloat x1 = 100.0f;
GLfloat y1 = 150.0f;
GLsizei rsize = 10;

GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

//janela
GLfloat windowWidth;
GLfloat windowHeight;

GLfloat islandWidth;
GLfloat islandHeight;

void AlteraTamanhoJanela(GLsizei w, GLsizei h);
void Timer(int value);

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Island");
    glutReshapeFunc(AlteraTamanhoJanela);
    glutTimerFunc(33, Timer, 1);
    glutDisplayFunc(render);
    inicializa();
    
    
    glutMainLoop();
    
    return 0;
}

//Funcao que sera chamada a cada intervalo de tempo
void Timer(int value) {
    //Muda a direcao quando chega na borda esquerda ou direita
    if (x1 >= ((windowWidth - islandWidth)/2) + islandWidth-rsize || x1 < (windowWidth - islandWidth)/2) {
        xstep = -xstep;
    }
    
    //Muda de direcao quando chega na borda superior ou inferior
    if (y1 >= ((windowHeight - islandHeight)/2) + islandHeight - rsize || y1 < (windowHeight - islandHeight)/2) {
        ystep = -ystep;
    }
    
    //se o quadrado sair do volume de visualizacao
    if (x1 > windowWidth-rsize) {
        x1 = windowWidth-rsize-1;
    }
    
    if(y1 > windowHeight-rsize) {
        y1 = windowHeight-rsize-1;
    }
    
    //move o quadrado
    
    x1 += xstep;
    y1 += ystep;
    
    //redesenha o quadrado com as novas coordenadas
    glutPostRedisplay();
    glutTimerFunc(33,Timer, value);
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    // Evita a divisao por zero
    if(h == 0) {
        h = 1;
    }
    
    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);
    
    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Estabelece a janela de seleção (left, right, bottom, top)
    if (w <= h)  {
        windowHeight = 250.0f * h/w;
        windowWidth = 250.0f;
    } else  {
        windowWidth = 250.0f * w/h;
        windowHeight = 250.0f;
    }
    
    islandWidth = 0.6 * windowWidth;
    islandHeight = 0.6 * windowHeight;
    
    gluOrtho2D(0.0f, windowWidth, 0.0f, windowHeight);
}

void render(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3ub(107, 142, 35);
    glBegin(GL_QUADS);
    glVertex2f( (windowWidth - islandWidth)/2, ((windowHeight - islandHeight)/2) + islandHeight);
    glVertex2f( (windowWidth - islandWidth)/2, (windowHeight - islandHeight)/2);
    glVertex2f(((windowWidth - islandWidth)/2) + islandWidth, (windowHeight - islandHeight)/2);
    glVertex2f(((windowWidth - islandWidth)/2) + islandWidth, ((windowHeight - islandHeight)/2) + islandHeight);
    glEnd();
    
    glColor3ub(120, 0, 0);
    glBegin(GL_QUADS);
    
    glVertex2f(x1, y1+rsize);
    glVertex2f(x1, y1);
    glVertex2f(x1+rsize, y1);
    glVertex2f(x1+rsize, y1+rsize);
    glEnd();
    
    glutSwapBuffers();
}

void inicializa() {
    glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
}


