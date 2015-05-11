#ifndef MIRO_TRIANGLE_H_INCLUDED
#define MIRO_TRIANGLE_H_INCLUDED

#include "Object.h"

/*
	The Triangle class stores a pointer to a mesh and an index into its
	triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object
{
public:
	Triangle(TriangleMesh * m = 0, unsigned int i = 0);
	virtual ~Triangle();

	void setIndex(unsigned int i) {m_index = i;}
	void setMesh(TriangleMesh* m) {m_mesh = m;}

	virtual void renderGL();
	virtual bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.00001f, float tMax = MIRO_TMAX);

  Vector3 get_vertex(int i);

protected:
	TriangleMesh* m_mesh;
	unsigned int m_index;
};

#endif // MIRO_TRIANGLE_H_INCLUDED

