#include "vec3f.h"


class Terrain {
private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	Vec3f** normals;
	bool computedNormals; //Whether normals is up-to-date

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