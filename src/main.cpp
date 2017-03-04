#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <math.h>
#include <sstream>

#include "imageloader.h"
#include "text3d.h"
#include "quadtree.h"


using namespace std;

const int NUM_GUYS = 100;
//The width of the terrain in units, after scaling
const float TERRAIN_WIDTH = 50.0f;
//The amount of time between each time that we handle collisions
const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.01f;

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}


void potentialCollisions(vector < Quadtree::GuyPair > &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Returns whether guy1 and guy2 are currently colliding
bool testCollision(Guy* guy1, Guy* guy2) {
	float dx = guy1->x() - guy2->x();
	float dz = guy1->z() - guy2->z();
	float r = guy1->radius() + guy2->radius();
	if (dx * dx + dz * dz < r * r) {
		float vx = guy1->velocityX() - guy2->velocityX();
		float vz = guy1->velocityZ() - guy2->velocityZ();
		return vx * dx + vz * dz < 0;
	}
	else {
		return false;
	}
}

void handleCollisions(vector<Guy*> &guys,
					  Quadtree* quadtree,
					  int &numCollisions) {
	vector<Quadtree::GuyPair> gps;
	potentialCollisions(gps, quadtree);
	for(unsigned int i = 0; i < gps.size(); i++) {
		Quadtree::GuyPair gp = gps[i];
		
		Guy* g1 = gp.guy1;
		Guy* g2 = gp.guy2;
		if (testCollision(g1, g2)) {
			g1->bounceOff(g2);
			g2->bounceOff(g1);
			numCollisions++;
		}
	}
}

//Moves the guys over the given interval of time, without handling collisions
void moveGuys(vector<Guy*> &guys, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < guys.size(); i++) {
		Guy* guy = guys[i];
		float oldX = guy->x();
		float oldZ = guy->z();
		guy->advance(dt);
		quadtree->guyMoved(guy, oldX, oldZ);
	}
}

//Advances the state of the guys over the indicated interval of time
void advance(vector<Guy*> &guys,
			 Quadtree* quadtree,
			 float t,
			 float &timeUntilHandleCollisions,
			 int &numCollisions) {
	while (t > 0) {
		if (timeUntilHandleCollisions <= t) {
			moveGuys(guys, quadtree, timeUntilHandleCollisions);
			handleCollisions(guys, quadtree, numCollisions);
			t -= timeUntilHandleCollisions;
			timeUntilHandleCollisions = TIME_BETWEEN_HANDLE_COLLISIONS;
		}
		else {
			moveGuys(guys, quadtree, t);
			timeUntilHandleCollisions -= t;
			t = 0;
		}
	}
}

//Returns a vector of numGuys new guys
vector<Guy*> makeGuys(int numGuys, MD2Model* model, Terrain* terrain) {
	vector<Guy*> guys;
	for(int i = 0; i < numGuys; i++) {
		guys.push_back(new Guy(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1)));
	}
	return guys;
}

//Draws the terrain
void drawTerrain(Terrain* terrain) {
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < terrain->length() - 1; z++) {
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < terrain->width(); x++) {
			Vec3f normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			normal = terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
}

//Draws a string at the top of the screen indicating that the specified number
//of collisions have occurred
void drawNumCollisions(int numCollisions) {
	ostringstream oss;
	oss << "Collisions: " << numCollisions;
	string str = oss.str();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0f, 1.7f, -5.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	t3dDraw2D(str, 0, 0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

MD2Model* _model;
vector<Guy*> _guys;
Terrain* _terrain;
float _angle = 0;
Quadtree* _quadtree;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
int _numCollisions; //The total number of collisions that have occurred

void cleanup() {
	delete _model;
	
	for(unsigned int i = 0; i < _guys.size(); i++) {
		delete _guys[i];
	}
	
	t3dCleanup();
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
	}
}

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image *image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 image->width, image->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	
	t3dInit(); //Initialize text drawing functionality
	
	//Load the model
	_model = MD2Model::load("lionteste.md2");
	if (_model != NULL) {
		_model->setAnimation("run");
	}
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//Draw the number of collisions that have occurred
	drawNumCollisions(_numCollisions);
	
	//The scaling factor for the terrain
	float scale = TERRAIN_WIDTH / (_terrain->width() - 1);
	
	glTranslatef(0, 0, -1.0f * scale * (_terrain->length() - 1));
	glRotatef(30, 1, 0, 0);
	glRotatef(_angle, 0, 1, 0);
	glTranslatef(-TERRAIN_WIDTH / 2, 0, -scale * (_terrain->length() - 1) / 2);
	
	GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat lightPos[] = {-0.2f, 0.3f, -1, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	//Draw the guys
	for(unsigned int i = 0; i < _guys.size(); i++) {
		_guys[i]->draw();
	}
	
	//Draw the terrain
	glScalef(scale, scale, scale);
	drawTerrain(_terrain);
	
	glutSwapBuffers();
}

void update(int value) {
	_angle += 0.3f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	advance(_guys,
			_quadtree,
			0.025f,
			_timeUntilHandleCollisions,
			_numCollisions);
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	srand((unsigned int)time(0)); //Seed the random number generator
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Putting It All Together - videotutorialsrock.com");
	initRendering();
	
	_terrain = loadTerrain("heightmap.bmp", 0.0f); //Load the terrain
	_guys = makeGuys(NUM_GUYS, _model, _terrain); //Create the guys
	//Compute the scaling factor for the terrain
	float scaledTerrainLength =
		TERRAIN_WIDTH / (_terrain->width() - 1) * (_terrain->length() - 1);
	//Construct and initialize the quadtree
	_quadtree = new Quadtree(0, 0, TERRAIN_WIDTH, scaledTerrainLength, 1);
	for(unsigned int i = 0; i < _guys.size(); i++) {
		_quadtree->add(_guys[i]);
	}
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









