#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
	m_mesh(m), m_index(i)
{
}


Triangle::~Triangle()
{
}


Vector3
Triangle::get_vertex(int i) {
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
  switch (i) {
  case 0:
    return m_mesh->vertices()[ti3.x];
  case 1:
    return m_mesh->vertices()[ti3.y];
  case 2:
    return m_mesh->vertices()[ti3.z];
  default:
    return NULL;
  }
}

void
Triangle::renderGL()
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	glBegin(GL_TRIANGLES);
		glVertex3f(v0.x, v0.y, v0.z);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
	glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& ray,float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

  // Cramer's rule
  Vector3 a = Vector3(v0.x - v1.x,
                      v0.y - v1.y,
                      v0.z - v1.z);
  Vector3 b = Vector3(v0.x - v2.x,
                      v0.y - v2.y,
                      v0.z - v2.z);
  Vector3 c = Vector3(ray.d.x,
                      ray.d.y,
                      ray.d.z);
  Vector3 d = Vector3(v0.x - ray.o.x,
                      v0.y - ray.o.y,
                      v0.z - ray.o.z);
  float det = dot( cross(a,b) , c);
  float detx = dot( cross(d,b) , c);
  float dety = dot( cross(a,d) , c);
  float detz = dot( cross(a,b) , d);
  float beta = detx / det;
  float gamma = dety / det;
  float t = detz / det;
  
  if (beta > 0 && gamma > 0 && (beta + gamma < 1)) {
    
    result.t = t;
    result.P = ray.o + t * ray.d;

    // barycentric interpolation of normals
    TriangleMesh::TupleI3 nti3 = m_mesh->nIndices()[m_index];
    const Vector3 & n0 = m_mesh->normals()[nti3.x]; //normal of vertex a of triangle
    const Vector3 & n1 = m_mesh->normals()[nti3.y]; //normal of vertex b of triangle
    const Vector3 & n2 = m_mesh->normals()[nti3.z]; //normal of vertex c of triangle
    result.N = (1 - beta - gamma) * n0 + beta * n1 + gamma * n2;
    result.N.normalize();

    result.material = this->m_material;
    return true;
    
  } else {
    
    return false;
    
  }
}
