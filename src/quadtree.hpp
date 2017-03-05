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

struct GuyPair {
	Animal* guy1;
	Animal* guy2;
};

const int MAX_QUADTREE_DEPTH = 6;
const int MIN_GUYS_PER_QUADTREE = 2;
const int MAX_GUYS_PER_QUADTREE = 5;

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
		//The guys in this, if this doesn't have any children
		set<Animal*> guys;
		//The depth of this in the tree
		int depth;
		//The number of guys in this, including those stored in its children
		int numGuys;

		void fileGuy(Animal* guy, float x, float z, bool addGuy);
		void haveChildren();
		void collectGuys(set<Animal*> &gs);
		void destroyChildren();
		void remove(Animal* guy, float x, float z);
	public:
		Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d);
		~Quadtree();
		void add(Animal* guy);
		void remove(Animal* guy);
		void guyMoved(Animal* guy, float x, float z);
		void potentialCollisions(vector<GuyPair> &collisions);
};
#endif
