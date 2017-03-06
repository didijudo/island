#ifndef quadtree_hpp
#define quadtree_hpp

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#endif
#include <set>
#include <vector>
#include "animal.h"
using namespace std;

struct AnimalPair {
	Animal* ani1;
	Animal* ani2;
};

const int MAX_QUADTREE_DEPTH = 100;
const int MIN_ANIMALS_PER_QUADTREE = 1;
const int MAX_ANIMALS_PER_QUADTREE = 8;

class Quadtree {
	private:
		float minX;
		float minZ;
		float maxX;
		float maxZ;
		float centerX; //(minX + maxX) / 2
		float centerZ; //(minZ + maxZ) / 2
		
		Quadtree *children[2][2];
		//Whether this has children
		bool hasChildren;
		//The animals in this, if this doesn't have any children
		set<Animal*> animals;
		//The depth of this in the tree
		int depth;
		//The number of animals in this, including those stored in its children
		int numAnimals;

		void fileAnimal(Animal* animal, float x, float z, bool addAnimal);
		void haveChildren();
		void collectAnimals(set<Animal*> &gs);
		void destroyChildren();
		void remove(Animal* animal, float x, float z);
	public:
		Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d);
		~Quadtree();
		void add(Animal* animal);
		void remove(Animal* animal);
		void animalMoved(Animal* animal, float x, float z);
		void potentialCollisions(vector<AnimalPair> &collisions);
};
#endif
