#include <vector>
#include <set>

#include "guy.h"

using namespace std;

//Our data structure for making collision detection faster
class Quadtree {
private:
	float minX;
	float minZ;
	float maxX;
	float maxZ;
	float centerX; //(minX + maxX) / 2
	float centerZ; //(minZ + maxZ) / 2

				   /* The children of this, if this has any.  children[0][*] are the
				   * children with x coordinates ranging from minX to centerX.
				   * children[1][*] are the children with x coordinates ranging from
				   * centerX to maxX.  Similarly for the other dimension of the children
				   * array.
				   */
	Quadtree *children[2][2];
	//Whether this has children
	bool hasChildren;
	//The guys in this, if this doesn't have any children
	set<Guy*> guys;
	//The depth of this in the tree
	int depth;
	//The number of guys in this, including those stored in its children
	int numGuys;

	const int MAX_QUADTREE_DEPTH = 6;
	const int MIN_GUYS_PER_QUADTREE = 2;
	const int MAX_GUYS_PER_QUADTREE = 5;

	//Adds a guy to or removes one from the children of this
	void fileGuy(Guy* guy, float x, float z, bool addGuy);

	//Creates children of this, and moves the guys in this to the children
	void haveChildren();

	//Adds all guys in this or one of its descendants to the specified set
	void collectGuys(set<Guy*> &gs);

	//Destroys the children of this, and moves all guys in its descendants
	//to the "guys" set
	void destroyChildren();

	//Removes the specified guy at the indicated position
	void remove(Guy* guy, float x, float z);

public:

	//Constructs a new Quadtree.  d is the depth, which starts at 1.
	Quadtree(float minX1, float minZ1, float maxX1, float maxZ1, int d);

	~Quadtree();

	//Adds a guy to this
	void add(Guy* guy);

	//Removes a guy from this
	void remove(Guy* guy);

	struct GuyPair {
		Guy* guy1;
		Guy* guy2;
	};

	//Changes the position of a guy in this from the specified position to
	//its current position
	void guyMoved(Guy* guy, float x, float z);

	//Adds potential collisions to the specified set
	void potentialCollisions(vector<GuyPair> &collisions);
};