#ifndef MIRO_ACCEL_H_INCLUDED
#define MIRO_ACCEL_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "Triangle.h"

class Space
{
 public:
  Space(Vector3 lb = Vector3(), Vector3 ub = Vector3(), Objects * o = NULL);
  Space(Space * parent);
  Objects * objects() {return m_objects;}
  bool overlap(Triangle * t);
  float calcSAH(int dim, Vector3 point);
  void split(Space * subspace1, Space * subspace2, int & split_d, Vector3 & split_p);
  bool intersect(const Ray& ray);
  
 protected:
  Vector3 m_lb; /* lower bound point */
  Vector3 m_ub; /* upper bound point */
  Objects * m_objects; /* all objects */
};

class Node
{
 public:
  Node(Space * s);
  Space * space() {return m_space;};
  int subdivision();
  bool traverse(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX);
  
 protected:
  Space * m_space;
  int m_leaf;  /* 1 if leaf */
  int m_split_d; /* split dimension */
  Vector3  m_split_p; /* split point */
  Node * m_children[2];
};

class Accel
{
public:
	void build(Objects * objs);
  bool intersect_naive(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX) const;
	bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
	Objects * m_objects;
  Node * m_kdtree;
};

#endif // MIRO_ACCEL_H_INCLUDED
