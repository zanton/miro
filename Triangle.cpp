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
  g_profile.n_raytri_intersect++;
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
  Vector3 temp = cross(b,c);
  float det = dot(a, temp); // (a x b) . c = a . (b x c)
  float detx = dot(d, temp); // (d x b) . c = d . (b x c)
  float beta = detx / det;
  if (beta < 0 || beta > 1.0)
    return false;
  temp = cross(a,d);
  float dety = dot(temp, c);
  float gamma = dety / det;
  if (gamma < 0 || beta + gamma > 1.0)
    return false;
  float detz = - dot(temp, b); // (a x b) . d = - (a x d) . b
  float t = detz / det;
  
  if (t >= tMin && t <= tMax) {
    
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
