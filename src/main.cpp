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

const int NUM_ZEBRA = 10;
const int NUM_LEAO = 10;
bool teste = true;
const float TERRAIN_WIDTH = 50.0f;

const float TIME_BETWEEN_HANDLE_COLLISIONS = 0.1f;

//Carrega o terreno onde a altura do terreno muda de -height / 2 até height / 2
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

void inicializa() {
	glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
}

void potentialCollisions(vector<AnimalPair> &cs, Quadtree* quadtree) {
	quadtree->potentialCollisions(cs);
}

//Retorna se o animal1 esta colidindo com o animal2
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

//Funcao que lida com as colisões
void handleCollisions(vector<Animal*> &animals,
					  Quadtree* quadtree,
					  int &numCollisions) {
	vector<AnimalPair> aps;

	//Calcula as potenciais colisões de acordo com o quadrante
	potentialCollisions(aps, quadtree);
	for(unsigned int i = 0; i < aps.size(); i++) {
		AnimalPair ap = aps[i];
		
		Animal* a1 = ap.ani1;
		Animal* a2 = ap.ani2;
		if (a1 == NULL || a2 == NULL) {
			continue;
		}

		//Teste se dois animais do mesmo quadrante se encontraram.
		if (testCollision(a1, a2)) {
			
			//TODO verificar se a colisao foi entre 2 leoes ou 2 zebras
			//Se for entre um leao e uma zebra define o que fazer
			
			if (a1->type() == a2->type()) {
				a1->bounceOff(a2);
				a2->bounceOff(a1);
			} else {
				if (a1->type() == ZEBRA && a2->type() == LION) {
					if (a1->scale()-0.88 > 1.5 * a2->scale()) {
						//Remove o a2 (Animal 2) do quadrante
						quadtree->remove(a2);	
						//Elimina o a2 do vetor principal de animais
						animals.erase(animals.begin()+a2->position());
						//Aumenta a escala do animal que ganhou a batalha
						a1->setScale(a1->scale() + 0.2f);

						//Descola todo os valores do vector em -1 posicoes
						//Necessário para evitar o segmentation fault 11
						for (size_t i = a2->position() - 1 ; i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}

					}	else {
						//quadrante
						quadtree->remove(a1);
						//vetor principal
						animals.erase(animals.begin() + a1->position());
						//Aumenta a escala
						a2->setScale(a2->scale() + 0.2f);
						//desloca os animais no vetor principal
						for (size_t i = a1->position() - 1; i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
						
					}
				} 	

				if (a1->type() == LION && a2->type() == ZEBRA) {
					if (a2->scale() - 0.88 > 1.5 * a1->scale()) {
						quadtree->remove(a1);	
						animals.erase(animals.begin() + a1->position());

						for (size_t i = a1->position() - 1; i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
					}	else {
						quadtree->remove(a2);

						animals.erase(animals.begin() + a2->position());
						for (size_t i = a2->position() - 1; i < animals.size(); i++)
						{
							animals[i]->setPosition(animals[i]->position() - 1);
						}
					}
				} 	
			}
			//Calcula o numero de colisoes
			numCollisions++;
		}
	}
}

//Funcao que move os animais a cada intervalo de tempo
void moveAnimals(vector<Animal*> &animals, Quadtree* quadtree, float dt) {
	for(unsigned int i = 0; i < animals.size(); i++) {
		Animal* animal = animals[i];
		float oldX = animal->x();
		float oldZ = animal->z();
		animal->advance(dt);
		quadtree->animalMoved(animal, oldX, oldZ);
	}
}

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

//Funcao que cria todos os animais
vector<Animal*> makeAnimals(int numAnimals, Modelo* modelLion, Modelo* modelZebra,Terrain* terrain) {
	vector<Animal*> animals;
	for(int i = 0; i < NUM_LEAO; i++) {
		animals.push_back(new Lion(modelLion,
							   terrain,
							   TERRAIN_WIDTH / (terrain->width() - 1),
								 i ));
	}
	for(int i = NUM_LEAO; i < NUM_LEAO + NUM_ZEBRA; i++) {
		animals.push_back(new Zebra(modelZebra,
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

//Desenha na tela o numero de animais
void drawNumCollisions(int numCollisions, vector<Animal*> animals) {
	int zebras = 0;
	int lions = 0;
	for (int i = 0; i < animals.size(); i++) {
		Animal* a = animals[i];
		if (a->type() == ZEBRA) {
			zebras++;
		} else {
			lions++;
		}
	}

	ostringstream oss;
	oss << "Zebras: " << zebras << "  ||  Lions: " << lions;
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


//MD2Model* _model;
Modelo* _modelLion;
Modelo* _modelZebra;
vector<Animal*> _animals;
Terrain* _terrain;
float _angle = 0;
Quadtree* _quadtree;
//The amount of time until we next check for and handle all collisions
float _timeUntilHandleCollisions = 0;
int _numCollisions; //The total number of collisions that have occurred

void cleanup() {
	delete _modelLion;
	delete _modelZebra;
	
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
	
	_modelLion = Modelo::carregarObj((char*)"Simba.obj",(char*)"Simba.png");
	_modelZebra = Modelo::carregarObj((char*)"Zebra.obj",(char*)"Zebra.jpg");
	//Load the model
	/*_model = MD2Model::load("blockybalboa.md2");
	if (_model != NULL) {
		_model->setAnimation("run");
	}*/
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
	
	float scale = TERRAIN_WIDTH / (_terrain->width() - 1);
	glTranslatef(0, 0, -1.0f * scale * (_terrain->length() - 1));
	//glTranslatef(0, 0, -0.02f);
	glRotatef(30, 1, 0, 0);
	glRotatef(_angle, 0, 1, 0);
	glTranslatef(-TERRAIN_WIDTH / 2, 0, -scale * (_terrain->length() - 1) / 2);
	
	GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat lightPos[] = {-0.2f, 0.3f, -1, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
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
	_animals = makeAnimals(NUM_ZEBRA + NUM_LEAO, _modelLion, _modelZebra, _terrain); 


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
