
#include <iostream>
#include "vec3f.h"

class Terrain {
private:
	bool computedNormals;
	public:
		Terrain(int w2, int l2);

		~Terrain();

		int width();
		int length();

		void setHeight(int x, int z, float y);
		float getHeight(int x, int z);
		void computeNormals();

		Vec3f getNormal(int x, int z);
};