#include "animal.h"
//jpgReturns a random float from 0 to < 1
float Animal::randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

void Animal::step() {
	//Update the turning direction information
	timeUntilSwitchDir -= ANIMAL_STEP_TIME;
	while (timeUntilSwitchDir <= 0) {
		timeUntilSwitchDir += 20 * randomFloat() + 15;
		isTurningLeft = !isTurningLeft;
	}

	//Update the position and angle
	float maxX = terrainScale * (terrain->width() - 1) - radius0;
	float maxZ = terrainScale * (terrain->length() - 1) - radius0;

	x0 += velocityX() * ANIMAL_STEP_TIME;
	z0 += velocityZ() * ANIMAL_STEP_TIME;
	bool hitEdge = false;
	if (x0 < radius0) {
		x0 = radius0;
		hitEdge = true;
	}
	else if (x0 > maxX) {
		x0 = maxX;
		hitEdge = true;
	}

	if (z0 < radius0) {
		z0 = radius0;
		hitEdge = true;
	}
	else if (z0 > maxZ) {
		z0 = maxZ;
		hitEdge = true;
	}

	if (hitEdge) {
		//Turn more quickly if we've hit the edge
		if (isTurningLeft) {
			angle -= 0.5f * speed * ANIMAL_STEP_TIME;
		}
		else {
			angle += 0.5f * speed * ANIMAL_STEP_TIME;
		}
	}
	else if (isTurningLeft) {
		angle -= 0.05f * speed * ANIMAL_STEP_TIME;
	}
	else {
		angle += 0.05f * speed * ANIMAL_STEP_TIME;
	}

	while (angle > 2 * PI) {
		angle -= 2 * PI;
	}
	while (angle < 0) {
		angle += 2 * PI;
	}
}


Animal::Animal(MD2Model* model1,
	Terrain* terrain1,
	float terrainScale1) {
	model = model1;
	terrain = terrain1;
	terrainScale = terrainScale1;

	animTime = 0;
	timeUntilNextStep = 0;

	//Initialize certain fields to random values
	
	radius0 = 0.4f * randomFloat() + 0.20f;
	x0 = randomFloat() *
		(terrainScale * (terrain->width() - 1) - radius0) + radius0;
	z0 = randomFloat() *
		(terrainScale * (terrain->length() - 1) - radius0) + radius0;
	speed = 1.5f * randomFloat() + 2.0f;
	isTurningLeft = randomFloat() < 0.5f;
	angle = 2 * PI * randomFloat();
	timeUntilSwitchDir = randomFloat() * (20 * randomFloat() + 15);
}

//Advances the state of the guy by the specified amount of time, by
//calling step() the appropriate number of times and adjusting animTime
void Animal::advance(float dt) {
	//Adjust animTime
	animTime += 0.45f * dt * speed / radius0;
	if (animTime > -100000000 && animTime < 1000000000) {
		animTime -= (int)animTime;
		if (animTime < 0) {
			animTime += 1;
		}
	}
	else {
		animTime = 0;
	}

	//Call step() the appropriate number of times
	while (dt > 0) {
		if (timeUntilNextStep < dt) {
			dt -= timeUntilNextStep;
			step();
			timeUntilNextStep = ANIMAL_STEP_TIME;
		}
		else {
			timeUntilNextStep -= dt;
			dt = 0;
		}
	}
}

int Animal::type() {
	return 0;
}

void Animal::draw() {
	if (model == NULL) {
		return;
	}

	float scale = radius0 / 2.5f;

	glPushMatrix();
	glTranslatef(x0, scale * 10.0f + y(), z0);
	glRotatef(90 - angle * 180 / PI, 0, 1, 0);
	glColor3f(1, 1, 1);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, scale);
	model->draw(animTime);
	glPopMatrix();
}

float Animal::x() {
	return x0;
}

float Animal::z() {
	return z0;
}

//Returns the current height of the guy on the terrain
float Animal::y() {
	return terrainScale *
		heightAt(terrain, x0 / terrainScale, z0 / terrainScale);
}

float Animal::velocityX() {
	return speed * cos(angle);
}

float Animal::velocityZ() {
	return speed * sin(angle);
}

//Returns the approximate radius of the guy
float Animal::radius() {
	return radius0;
}

//Returns the angle at which the guy is currently walking, in radians.
//An angle of 0 indicates the positive x direction, while an angle of
//PI / 2 indicates the positive z direction.
float Animal::walkAngle() {
	return angle;
}

//Returns the approximate height of the terrain at the specified (x, z) position
float Animal::heightAt(Terrain* terrain, float x, float z) {
	//Make (x, z) lie within the bounds of the terrain
	if (x < 0) {
		x = 0;
	}
	else if (x > terrain->width() - 1) {
		x = terrain->width() - 1;
	}
	if (z < 0) {
		z = 0;
	}
	else if (z > terrain->length() - 1) {
		z = terrain->length() - 1;
	}

	//Compute the grid cell in which (x, z) lies and how close we are to the
	//left and outward edges
	int leftX = (int)x;
	if (leftX == terrain->width() - 1) {
		leftX--;
	}
	float fracX = x - leftX;

	int outZ = (int)z;
	if (outZ == terrain->width() - 1) {
		outZ--;
	}
	float fracZ = z - outZ;

	//Compute the four heights for the grid cell
	float h11 = terrain->getHeight(leftX, outZ);
	float h12 = terrain->getHeight(leftX, outZ + 1);
	float h21 = terrain->getHeight(leftX + 1, outZ);
	float h22 = terrain->getHeight(leftX + 1, outZ + 1);

	//Take a weighted average of the four heights
	return (1 - fracX) * ((1 - fracZ) * h11 + fracZ * h12) +
		fracX * ((1 - fracZ) * h21 + fracZ * h22);
}

//Adjusts the angle at which this guy is walking in response to a
//collision with the specified guy
void Animal::bounceOff(Animal* otherGuy) {
	float vx = velocityX();
	float vz = velocityZ();

	float dx = otherGuy->x0 - x0;
	float dz = otherGuy->z0 - z0;
	float m = sqrt(dx * dx + dz * dz);
	dx /= m;
	dz /= m;

	float dotProduct = vx * dx + vz * dz;
	vx -= 2 * dotProduct * dx;
	vz -= 2 * dotProduct * dz;

	if (vx != 0 || vz != 0) {
		angle = atan2(vz, vx);
	}
}
