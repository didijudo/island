#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <vector>

#include "imageloader.h"
#include "text3d.h"
#include "animal.h"
#include "zebra.hpp"
#include "lion.hpp"
#include "quadtree.hpp"


using namespace std;

const int NUM_ZEBRA = 15;
const int NUM_LEAO = 5;

const float TERRAIN_WIDTH = 50.0f;

const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.1f;

//Carrega o terreno com base na cor da imagem, aumentando 
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = -( height * ((color / 255.0f) - 0.5f));
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

void inicializa() {
	//Cor do oceano
	glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
}

void potentialCollisions(vector<AnimalPair> &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Retorna se o animal1 e animal2 estão atualmente colidindo
bool testCollision(Animal* animal1, Animal* animal2) {
	float dx = animal1->x() - animal2->x();
	float dz = animal1->z() - animal2->z();
	float r = animal1->radius() + animal2->radius();
	if (dx * dx + dz * dz < r * r) {
		float vx = animal1->velocityX() - animal2->velocityX();
		float vz = animal1->velocityZ() - animal2->velocityZ();
		return vx * dx + vz * dz < 0;
	}
	else {
		return false;
	}
}

//Define o que fazer quando houver colisões
void handleCollisions(vector<Animal*> &animals,
					  Quadtree* quadtree,
					  int &numCollisions) {
	vector<AnimalPair> aps;
	potentialCollisions(aps, quadtree);
	for(unsigned int i = 0; i < aps.size(); i++) {
		AnimalPair ap = aps[i];
		
		Animal* a1 = ap.ani1;
		Animal* a2 = ap.ani2;
		if (a1 == NULL || a2 == NULL) {
			std::cout << "chegou NULL: ";
			continue;
		}

		if (testCollision(a1, a2)) {
			
			//TODO verificar se a colisao foi entre 2 leoes ou 2 zebras
			//Se for entre um leao e uma zebra define o que fazer
			
			if (a1->type() == a2->type()) {
				a1->bounceOff(a2);
				a2->bounceOff(a1);
				std::cout << "capacity changed: " << animals.capacity()<< '\n';
			} else {
				if (a1->type() == ZEBRA && a2->type() == LION) {
					if (a1->scale() > 1.5 * a2->scale()) {
						quadtree->remove(a2);
						animals.erase(animals.begin()+a2->position());
						std::cout << "capacity changed: " << animals.capacity()<< '\n';

						for (size_t i = a2->position(); i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
					} else {
						
						a2->setScale(a2->scale()*10);

						quadtree->remove(a1);
						animals.erase(animals.begin() + a1->position());
						
						

						for (size_t i = a1->position(); i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
						
						std::cout << "capacity changed: " << animals.capacity()<< '\n';
					}
				} 	

				if (a1->type() == LION && a2->type() == ZEBRA) {
					if (a2->scale() > 1.5 * a1->scale()) {
						quadtree->remove(a1);	
						animals.erase(animals.begin() + a1->position());

						for (size_t i = a1->position(); i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
					} else {
						
						a1->setScale(a1->scale()*10);

						quadtree->remove(a2);
						animals.erase(animals.begin() + a2->position());

						

						for (size_t i = a2->position(); i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
					}
				} 	
			}
			numCollisions++;
		}
	}
}

//Move todos os animais na cena
void moveAnimals(vector<Animal*> &animals, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < animals.size(); i++) {
		Animal* animal = animals[i];
		float oldX = animal->x();
		float oldZ = animal->z();
		animal->advance(dt);
		quadtree->animalMoved(animal, oldX, oldZ);
	}
}

//Atualiza a posição de todos animais
void advance(vector<Animal*> &animals,
			 Quadtree* quadtree,
			 float t,
			 float &timeUntilHandleCollisions,
			 int &numCollisions) {
	while (t > 0) {
		if (timeUntilHandleCollisions <= t) {
			moveAnimals(animals, quadtree, timeUntilHandleCollisions);
			handleCollisions(animals, quadtree, numCollisions);
			t -= timeUntilHandleCollisions;
			timeUntilHandleCollisions = TIME_BETWEEN_HANDLE_COLLISIONS;
		}
		else {
			moveAnimals(animals, quadtree, t);
			timeUntilHandleCollisions -= t;
			t = 0;
		}
	}
}

//Instanciar os animais na cena
vector<Animal*> makeAnimals(int numAnimals, MD2Model* model, Terrain* terrain) {
	vector<Animal*> animals;
	for(int i = 0; i < NUM_LEAO; i++) {
		animals.push_back(new Lion(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1),
								 i ));
	}
	for(int i = NUM_LEAO; i < NUM_LEAO + NUM_ZEBRA; i++) {
		animals.push_back(new Zebra(model,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1),
								 i));
	}
	return animals;
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
void drawNumCollisions(int numCollisions, vector<Animal*> animals) {
	ostringstream oss;
	oss << "Animais: " << animals.size();
	string str = oss.str();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0f, 1.7f, -5.0f);
	glScalef(0.1f, 0.1f, 0.1f);
	t3dDraw2D(str, 0, 0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}



MD2Model* _model;
vector<Animal*> _animals;
Terrain* _terrain;
float _angle = 0;
Quadtree* _quadtree;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
int _numCollisions; //The total number of collisions that have occurred

void cleanup() {
	delete _model;
	
	for(unsigned int i = 0; i < _animals.size(); i++) {
		delete _animals[i];
	}
	
	t3dCleanup();
}

void teclado(unsigned char key, int x, int y) {
	switch (key) {
		case 'a': 
			_angle += 0.8f;
			if (_angle > 360) {
				_angle -= 360;
			}
			break;

		case 'd': 
			_angle -= 0.8f;
			if (_angle < 360) {
				_angle += 360;
			}
			break;
		case 's':

			break;
		case 'w':
			break;
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
	
	t3dInit(); //Inicializa o desenho do texto
	
	//Carregar o modelo
	_model = MD2Model::load("blockybalboa.md2");
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
	drawNumCollisions(_numCollisions, _animals);
	
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
	
	//Draw the animals
	for(unsigned int i = 0; i < _animals.size(); i++) {
		_animals[i]->draw();
	}
	
	//Draw the terrain
	glScalef(scale, scale, scale);
	drawTerrain(_terrain);
	
	glutSwapBuffers();
}

void update(int value) {
/*	_angle += 0.3f;
	if (_angle > 360) {
		_angle -= 360;
	}*/
	
	advance(_animals,
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
	glutInitWindowSize(800, 600);
	
	glutCreateWindow("Island");
	initRendering();
	
	_terrain = loadTerrain("heightmap.bmp", 20.0f);

	//Cria as instancias dos animais
	_animals = makeAnimals(NUM_ZEBRA + NUM_LEAO, _model, _terrain); 


	//Calcula a escala do terreno
	float scaledTerrainLength =
		TERRAIN_WIDTH / (_terrain->width() - 1) * (_terrain->length() - 1);

	//Inicializa os quadrantes
	_quadtree = new Quadtree(0, 0, TERRAIN_WIDTH, scaledTerrainLength, 1);
	for(unsigned int i = 0; i < _animals.size(); i++) {
		_quadtree->add(_animals[i]);
	}
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(teclado);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	inicializa();
	glutMainLoop();
	return 0;
}
