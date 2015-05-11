#include "Accel.h"
#include "Ray.h"
#include "Console.h"
#include <assert.h>

#define KDTREE_BUILD_THRESHOLD 100
#define KDTREE_N_SAMPLES_TO_PIVOT 10

Profile g_profile;

Space::Space(Vector3 lb, Vector3 ub, Objects * o) :
  m_lb(lb), m_ub(ub), m_objects(o)
{
}

Space::Space(Space * parent) 
{
  m_lb = parent->m_lb;
  m_ub = parent->m_ub;
  m_objects = NULL;
}

bool
Space::overlap(Triangle * t) {
  /* simple check */
  int v;
  for (v = 0; v < 3; v++) {
    const Vector3 & p = t->get_vertex(v);
    if (p.x >= m_lb.x && p.x <= m_ub.x
        && p.y >= m_lb.y && p.y <= m_ub.y
        && p.z >= m_lb.z && p.z <= m_ub.z) {
      return true;
    }
  }
  /* proper check */
  float boxhalfsize[3] =
    { (m_ub.x - m_lb.x) / 2.0,
      (m_ub.y - m_lb.y) / 2.0,
      (m_ub.z - m_lb.z) / 2.0 };
  float boxcenter[3] =
    { m_lb.x + boxhalfsize[0],
      m_lb.y + boxhalfsize[1],
      m_lb.z + boxhalfsize[2] };
  Vector3 v0 = t->get_vertex(0);
  Vector3 v1 = t->get_vertex(1);
  Vector3 v2 = t->get_vertex(2);
  float triverts[3][3] =
    { {v0.x , v0.y , v0.z},
      {v1.x , v1.y , v1.z},
      {v2.x , v2.y , v2.z} };
  int ret = triBoxOverlap(boxcenter, boxhalfsize, triverts);
  if (ret)
    return true;
  else
    return false;
}

float
Space::calcSAH(int dim, Vector3 point) {
  Vector3 new_lb = m_lb;
  Vector3 new_ub = m_ub;
  switch (dim) {
  case 0:
    //assert( new_lb.x <= point.x && point.x <= new_ub.x );
    new_lb.x = new_ub.x = point.x;
    break;
  case 1:
    //assert( new_lb.y <= point.y && point.y <= new_ub.y );
    new_lb.y = new_ub.y = point.y;
    break;
  case 2:
    //assert( new_lb.z <= point.z && point.z <= new_ub.z );
    new_lb.z = new_ub.z = point.z;
    break;
  }
  float V1 = (new_ub.x - m_lb.x) * (new_ub.y - m_lb.y) * (new_ub.z - m_lb.z);
  float V2 = (m_ub.x - new_lb.x) * (m_ub.y - new_lb.y) * (m_ub.z - new_lb.z);
  float V = V1 + V2;
  int N1 = 0;
  int N2 = 0;
  Objects::iterator it;
  for (it = m_objects->begin(); it != m_objects->end(); it++) {
    Triangle * t = (Triangle *) *it;
    int v;
    for (v = 0; v < 3; v++) {
      Vector3 p = t->get_vertex(v);
      if (p.x >= m_lb.x && p.x <= new_ub.x
          && p.y >= m_lb.y && p.y <= new_ub.y
          && p.z >= m_lb.z && p.z <= new_ub.z) {
        N1++;
        break;
      }
    }
    for (v = 0; v < 3; v++) {
      Vector3 p = t->get_vertex(v);
      if (p.x >= new_lb.x && p.x <= m_ub.x
          && p.y >= new_lb.y && p.y <= m_ub.y
          && p.z >= new_lb.z && p.z <= m_ub.z) {
        N2++;
        break;
      }
    }
  }
  return (N1 * V1 / V + N2 * V2 / V);
  /*
  if (N1 > N2)
    return N1 - N2;
  else
    return N2 - N1;
  */
}

void
Space::split(Space * subspace1, Space * subspace2, int & split_d, Vector3 & split_p) {
  int n = m_objects->size();
  int cand;
  float min_cost = FLT_MAX;
  for (cand = 0; cand < KDTREE_N_SAMPLES_TO_PIVOT; cand++) {
    Triangle * t = (Triangle *) (*m_objects)[cand * n / KDTREE_N_SAMPLES_TO_PIVOT];
    Vector3 p;
    int vertex;
    for (vertex = 0; vertex < 3; vertex++) {
      p = t->get_vertex(vertex);
      int dim;
      for (dim = 0; dim < 3; dim++) {
        if (dim == 0 && (p.x <= m_lb.x || p.x >= m_ub.x))
          continue;
        else if (dim == 1 && (p.y <= m_lb.y || p.y >= m_ub.y))
          continue;
        else if (dim == 2 && (p.z <= m_lb.z || p.z >= m_ub.z))
          continue;
        float cost = calcSAH(dim, p);
        if (cost < min_cost) {
          split_p = p;
          split_d = dim;
          min_cost = cost;
        }
      }
    }
  }
  //assert( min_cost < FLT_MAX );
  Vector3 new_lb = m_lb;
  Vector3 new_ub = m_ub;
  switch (split_d) {
  case 0:
    new_lb.x = new_ub.x = split_p.x;
    //printf(" (x=%f) ", split_p.x);
    break;
  case 1:
    new_lb.y = new_ub.y = split_p.y;
    //printf(" (y=%f) ", split_p.y);
    break;
  case 2:
    new_lb.z = new_ub.z = split_p.z;
    //printf(" (z=%f) ", split_p.z);
    break;
  }
  subspace1->m_ub = new_ub;
  subspace2->m_lb = new_lb;
  subspace1->m_objects = new Objects;
  subspace2->m_objects = new Objects;
  Objects::iterator it;
  for (it = m_objects->begin(); it != m_objects->end(); it++) {
    Object * t = *it;
    if (subspace1->overlap((Triangle *) t))
      subspace1->m_objects->push_back(t);
    if (subspace2->overlap((Triangle *) t))
      subspace2->m_objects->push_back(t);
  }
}

static bool
rayIntersectTriangle(const Ray & ray, const Vector3 & v0, const Vector3 & v1, const Vector3 & v2) {
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
    return true;
  } else {
    return false;
  }  
}

bool
Space::intersect(const Ray& ray) {
  Vector3 v0, v1, v2, v3;
  // x, y
  v0 = v1 = v2 = v3 = m_lb;
  v1.x = v3.x = m_ub.x;
  v2.y = v3.y = m_ub.y;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  // x, z
  v0 = v1 = v2 = v3 = m_lb;
  v1.x = v3.x = m_ub.x;
  v2.z = v3.z = m_ub.z;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  // y, z
  v0 = v1 = v2 = v3 = m_lb;
  v1.y = v3.y = m_ub.y;
  v2.z = v3.z = m_ub.z;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  
  // x, y
  v0 = v1 = v2 = v3 = m_ub;
  v1.x = v3.x = m_lb.x;
  v2.y = v3.y = m_lb.y;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  // x, z
  v0 = v1 = v2 = v3 = m_ub;
  v1.x = v3.x = m_lb.x;
  v2.z = v3.z = m_lb.z;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  // y, z
  v0 = v1 = v2 = v3 = m_ub;
  v1.y = v3.y = m_lb.y;
  v2.z = v3.z = m_lb.z;
  if (rayIntersectTriangle(ray, v0, v1, v2)) return true;
  if (rayIntersectTriangle(ray, v3, v1, v2)) return true;
  return false;
}


Node::Node(Space * s) :
  m_space(s)
{
  m_leaf = 1;
  m_split_d = 0;
  m_split_p.x = m_split_p.y = m_split_p.z = 0.0f;
  m_children[0] = NULL;
  m_children[1] = NULL;
}

int
Node::subdivision() {
  //printf("to divide %lu objects: ", m_space->objects()->size());
  if (m_space->objects()->size() < KDTREE_BUILD_THRESHOLD) {
    g_profile.n_leaves++;
    //printf("no divide\n");
    return 0;
  }
  /* To split */
  m_leaf = 0;
  Space * subspace1 = new Space(m_space);
  Space * subspace2 = new Space(m_space);
  m_space->split(subspace1, subspace2, m_split_d, m_split_p);
  //printf("divided to %lu and %lu\n", subspace1->objects()->size(), subspace2->objects()->size());
  g_profile.n_nodes += 2;
  this->m_children[0] = new Node(subspace1);
  this->m_children[1] = new Node(subspace2);
  this->m_children[0]->subdivision();
  this->m_children[1]->subdivision();
  return 1;
}

bool
Node::traverse(HitInfo& minHit, const Ray& ray, float tMin, float tMax)
{
  g_profile.n_traversals++;
  bool hit = false;
  HitInfo tempMinHit = minHit;
  if (m_leaf) {
    Objects * objects = m_space->objects();
    for (size_t i = 0; i < objects->size(); ++i) {
      if ( (*objects)[i]->intersect(tempMinHit, ray, tMin, tMax) ) {
        g_profile.n_raytri_intersected++;
        hit = true;
        if (tempMinHit.t < minHit.t)
          minHit = tempMinHit;
      }
    }          
  } else {
    /* left sub-space */
    bool hit0 = false;
    if (m_children[0]->space()->intersect(ray)) {
      tempMinHit = minHit;
      hit0 = m_children[0]->traverse(tempMinHit, ray, tMin, tMax);
      if (hit0 && tempMinHit.t < minHit.t)
        minHit = tempMinHit;
    }
    /* right sub-space */
    bool hit1 = false;
    if (m_children[1]->space()->intersect(ray)) {
      tempMinHit = minHit;
      hit1 = m_children[1]->traverse(tempMinHit, ray, tMin, tMax);
      if (hit1 && tempMinHit.t < minHit.t)
        minHit = tempMinHit;
    }
    hit = hit0 || hit1;
  }
  return hit;
}


void
Accel::build(Objects * objs)
{
	/* construct the bounding volume hierarchy */
  /* ... */
  
  /* construct the kd tree */
	m_objects = objs;
  float minx[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
  float maxx[3] = { FLT_MIN, FLT_MIN, FLT_MIN };
  Objects::iterator it;
  for (it = m_objects->begin(); it != m_objects->end(); it++) {
    Triangle * t = (Triangle *) *it;
    int i;
    for (i = 0; i < 3; i++) {
      float x[3] = { t->get_vertex(i).x,
                     t->get_vertex(i).y,
                     t->get_vertex(i).z };
      int j;
      for (j = 0; j < 3; j++) {
        if (x[j] < minx[j])
          minx[j] = x[j];
        else if (x[j] > maxx[j])
          maxx[j] = x[j];
      }
    }
  }
  printf("min point: %f, %f, %f\n", minx[0], minx[1], minx[2]);
  printf("max point: %f, %f, %f\n", maxx[0], maxx[1], maxx[2]);
  Space * space = new Space( Vector3(minx[0], minx[1], minx[2]),
                             Vector3(maxx[0], maxx[1], maxx[2]),
                             m_objects );
  g_profile.n_nodes++;
  m_kdtree = new Node(space);
  m_kdtree->subdivision();
}


bool
Accel::intersect_naive(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	// Here you would need to traverse the acceleration data structure to perform ray-intersection
	// acceleration. For now we just intersect every object.

	bool hit = false;
	HitInfo tempMinHit;
	minHit.t = MIRO_TMAX;

	for (size_t i = 0; i < m_objects->size(); ++i)
	{
		if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
		{
			hit = true;
			if (tempMinHit.t < minHit.t)
				minHit = tempMinHit;
		}
	}

	return hit;
}

bool
Accel::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	// Here you would need to traverse the acceleration data structure to perform ray-intersection
	// acceleration. For now we just intersect every object.

  bool hit = false;
  minHit.t = MIRO_TMAX;
  if (m_kdtree->space()->intersect(ray)) {
    hit = m_kdtree->traverse(minHit, ray, tMin, tMax);
  }
  return hit;
}
