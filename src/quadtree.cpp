
#include "quadtree.h"
using namespace std;

//Adds a guy to or removes one from the children of this
void Quadtree::fileGuy(Guy* guy, float x, float z, bool addGuy) {
	//Figure out in which child(ren) the guy belongs
	for (int xi = 0; xi < 2; xi++) {
		if (xi == 0) {
			if (x - guy->radius() > centerX) {
				continue;
			}
		}
		else if (x + guy->radius() < centerX) {
			continue;
		}

		for (int zi = 0; zi < 2; zi++) {
			if (zi == 0) {
				if (z - guy->radius() > centerZ) {
					continue;
				}
			}
			else if (z + guy->radius() < centerZ) {
				continue;
			}

			//Add or remove the guy
			if (addGuy) {
				children[xi][zi]->add(guy);
			}
			else {
				children[xi][zi]->remove(guy, x, z);
			}
		}
	}
}

//Creates children of this, and moves the guys in this to the children
void Quadtree::haveChildren() {
	for (int x = 0; x < 2; x++) {
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

		for (int z = 0; z < 2; z++) {
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

	//Remove all guys from "guys" and add them to the new children
	for (set<Guy*>::iterator it = guys.begin(); it != guys.end();
		it++) {
		Guy* guy = *it;
		fileGuy(guy, guy->x(), guy->z(), true);
	}
	guys.clear();

	hasChildren = true;
}

//Adds all guys in this or one of its descendants to the specified set
void Quadtree::collectGuys(set<Guy*> &gs) {
	if (hasChildren) {
		for (int x = 0; x < 2; x++) {
			for (int z = 0; z < 2; z++) {
				children[x][z]->collectGuys(gs);
			}
		}
	}
	else {
		for (set<Guy*>::iterator it = guys.begin(); it != guys.end();
			it++) {
			Guy* guy = *it;
			gs.insert(guy);
		}
	}
}

//Destroys the children of this, and moves all guys in its descendants
//to the "guys" set
void Quadtree::destroyChildren() {
	//Move all guys in descendants of this to the "guys" set
	collectGuys(guys);

	for (int x = 0; x < 2; x++) {
		for (int z = 0; z < 2; z++) {
			delete children[x][z];
		}
	}

	hasChildren = false;
}

//Removes the specified guy at the indicated position
void Quadtree::remove(Guy* guy, float x, float z) {
	numGuys--;

	if (hasChildren && numGuys < MIN_GUYS_PER_QUADTREE) {
		destroyChildren();
	}

	if (hasChildren) {
		fileGuy(guy, x, z, false);
	}
	else {
		guys.erase(guy);
	}
}


//Constructs a new Quadtree.  d is the depth, which starts at 1.
Quadtree::Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d) {
	minX = minX1;
	minZ = minZ1;
	maxX = maxX1;
	maxZ = maxZ1;
	centerX = (minX + maxX) / 2;
	centerZ = (minZ + maxZ) / 2;

	depth = d;
	numGuys = 0;
	hasChildren = false;
}

Quadtree::~Quadtree() {
	if (hasChildren) {
		destroyChildren();
	}
}

//Adds a guy to this
void Quadtree::add(Guy* guy) {
	numGuys++;
	if (!hasChildren && depth < MAX_QUADTREE_DEPTH &&
		numGuys > MAX_GUYS_PER_QUADTREE) {
		haveChildren();
	}

	if (hasChildren) {
		fileGuy(guy, guy->x(), guy->z(), true);
	}
	else {
		guys.insert(guy);
	}
}

//Removes a guy from this
void Quadtree::remove(Guy* guy) {
	remove(guy, guy->x(), guy->z());
}



//Changes the position of a guy in this from the specified position to
//its current position
void Quadtree::guyMoved(Guy* guy, float x, float z) {
	remove(guy, x, z);
	add(guy);
}

//Adds potential collisions to the specified set
void Quadtree::potentialCollisions(vector<GuyPair> &collisions) {
	if (hasChildren) {
		for (int x = 0; x < 2; x++) {
			for (int z = 0; z < 2; z++) {
				children[x][z]->potentialCollisions(collisions);
			}
		}
	}
	else {
		//Add all pairs (guy1, guy2) from guys
		for (set<Guy*>::iterator it = guys.begin(); it != guys.end();
			it++) {
			Guy* guy1 = *it;
			for (set<Guy*>::iterator it2 = guys.begin();
				it2 != guys.end(); it2++) {
				Guy* guy2 = *it2;
				//This test makes sure that we only add each pair once
				if (guy1 < guy2) {
					GuyPair gp;
					gp.guy1 = guy1;
					gp.guy2 = guy2;
					collisions.push_back(gp);
				}
			}
		}
	}
}