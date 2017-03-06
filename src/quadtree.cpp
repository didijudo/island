#include "quadtree.hpp"

using namespace std;

void Quadtree::fileAnimal(Animal* animal, float x, float z, bool addAnimal) {
	//Figure out in which child(ren) the animal belongs
	for(int xi = 0; xi < 2; xi++) {
		if (xi == 0) {
			if (x - animal->radius() > centerX) {
				continue;
			}
		}
		else if (x + animal->radius() < centerX) {
			continue;
		}
		
		for(int zi = 0; zi < 2; zi++) {
			if (zi == 0) {
				if (z - animal->radius() > centerZ) {
					continue;
				}
			}
			else if (z + animal->radius() < centerZ) {
				continue;
			}
			
			//Add or remove the animal
			if (addAnimal) {
				children[xi][zi]->add(animal);
			}
			else {
				children[xi][zi]->remove(animal, x, z);
			}
		}
	}
}
		
void Quadtree::haveChildren() {
	for(int x = 0; x < 2; x++) {
		float minX2;
		float maxX2;
		if (x == 0) {
			minX2 = minX;
			maxX2 = centerX;
		}
		else {
			minX2 = centerX;
			maxX2 = maxX;
		}
		
		for(int z = 0; z < 2; z++) {
			float minZ2;
			float maxZ2;
			if (z == 0) {
				minZ2 = minZ;
				maxZ2 = centerZ;
			}
			else {
				minZ2 = centerZ;
				maxZ2 = maxZ;
			}
			
			children[x][z] =
				new Quadtree(minX2, maxX2, minZ2, maxZ2, depth + 1);
		}
	}
	
	//Remove all animals from "animals" and add them to the new children
	for(set<Animal*>::iterator it = animals.begin(); it != animals.end();
			it++) {
		Animal* animal = *it;
		fileAnimal(animal, animal->x(), animal->z(), true);
	}
	animals.clear();
	
	hasChildren = true;
}
		
//Adds all animals in this or one of its descendants to the specified set
void Quadtree::collectAnimals(set<Animal*> &gs) {
	if (hasChildren) {
		for(int x = 0; x < 2; x++) {
			for(int z = 0; z < 2; z++) {
				children[x][z]->collectAnimals(gs);
			}
		}
	}
	else {
		for(set<Animal*>::iterator it = animals.begin(); it != animals.end();
				it++) {
			Animal* animal = *it;
			gs.insert(animal);
		}
	}
}
	
//Destroys the children of this, and moves all animals in its descendants
//to the "animals" set
void Quadtree::destroyChildren() {
	//Move all animals in descendants of this to the "animals" set
	collectAnimals(animals);
	
	for(int x = 0; x < 2; x++) {
		for(int z = 0; z < 2; z++) {
			delete children[x][z];
		}
	}
	
	hasChildren = false;
}
	
//Removes the specified animal at the indicated position
void Quadtree::remove(Animal* animal, float x, float z) {
	numAnimals--;
	
	if (hasChildren && numAnimals < MIN_ANIMALS_PER_QUADTREE) {
		destroyChildren();
	}
	
	if (hasChildren) {
		fileAnimal(animal, x, z, false);
	}
	else {
		animals.erase(animal);
	}
}

Quadtree::Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d) {
	minX = minX1;
	minZ = minZ1;
	maxX = maxX1;
	maxZ = maxZ1;
	centerX = (minX + maxX) / 2;
	centerZ = (minZ + maxZ) / 2;
	
	depth = d;
	numAnimals = 0;
	hasChildren = false;
}

Quadtree::~Quadtree() {
	if (hasChildren) {
		destroyChildren();
	}
}

//Adds a animal to this
void Quadtree::add(Animal* animal) {
	numAnimals++;
	if (!hasChildren && depth < MAX_QUADTREE_DEPTH &&
		numAnimals > MAX_ANIMALS_PER_QUADTREE) {
		haveChildren();
	}
	
	if (hasChildren) {
		fileAnimal(animal, animal->x(), animal->z(), true);
	}
	else {
		animals.insert(animal);
	}
}

//Removes a animal from this
void Quadtree::remove(Animal* animal) {
	remove(animal, animal->x(), animal->z());
}

//Changes the position of a animal in this from the specified position to
//its current position
void Quadtree::animalMoved(Animal* animal, float x, float z) {
	remove(animal, x, z);
	add(animal);
}

//Adds potential collisions to the specified set
void Quadtree::potentialCollisions(vector<AnimalPair> &collisions) {
	if (hasChildren) {
		for(int x = 0; x < 2; x++) {
			for(int z = 0; z < 2; z++) {
				children[x][z]->potentialCollisions(collisions);
			}
		}
	}
	else {
		//Add all pairs (ani1, ani2) from animals
		for(set<Animal*>::iterator it = animals.begin(); it != animals.end();
				it++) {
			Animal* ani1 = *it;
			for(set<Animal*>::iterator it2 = animals.begin();
					it2 != animals.end(); it2++) {
				Animal* ani2 = *it2;
				//This test makes sure that we only add each pair once
				if (ani1 < ani2) {
					AnimalPair gp;
					gp.ani1 = ani1;
					gp.ani2 = ani2;
					collisions.push_back(gp);
				}
			}
		}
	}
}
