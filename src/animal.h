#ifndef animal_h
#define animal_h

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#endif

#include "md2model.h"
#include "terrain.h"
#include <math.h>
#include "carregadorObj.cpp"

const int LION = 1;
const int ZEBRA = 2;

class Animal {
protected:
	//MD2Model* model;
	Modelo* model;
	Terrain* terrain;
	float terrainScale; //The scaling factor for the terrain
	float x0;
	float z0;
	float animTime; //The current position in the animation of the model
	float radius0; //The approximate radius of the guy
	float speed;
	int position0;
	//The angle at which the guy is currently walking, in radians.  An angle
	//of 0 indicates the positive x direction, while an angle of PI / 2
	//indicates the positive z direction.  The angle always lies between 0
	//and 2 * PI.
	float angle;
	//The amount of time until step() should next be called
	float timeUntilNextStep;
	bool isTurningLeft; //Whether the guy is currently turning left
	float timeUntilSwitchDir; //The amount of time until switching direction

	const float ANIMAL_STEP_TIME = 0.01f;
	const float PI = 3.1415926535f;
	//Returns a random float from 0 to < 1
	float randomFloat();
	float scale0;

	void step();

public:
	Animal(Modelo* model1,
		Terrain* terrain1,
		float terrainScale1,
		int position1);

	void advance(float dt);
	virtual void draw();
	virtual int type(); 
	void setScale(float scale1);
	void setPosition(int p);

	float x();
	float z();
	float y();
	float scale();
	int position();

	float velocityX();
	float velocityZ();

	float radius();
	float walkAngle();
	float heightAt(Terrain* terrain, float x, float z);

	void bounceOff(Animal* otherGuy);
};
#endif
