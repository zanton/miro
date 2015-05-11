#ifndef __MIRO_H__
#define __MIRO_H__

const float MIRO_TMAX = 1e12f;
const float epsilon   = 0.001f; 
const float PI = 3.1415926535897932384626433832795028841972f;
const float DegToRad = PI/180.0f;
const float RadToDeg = 180.0f/PI; 

#include <stdlib.h>
#include "OpenGL.h"
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <tbb/tbb.h>

class Ray;
class HitInfo;

class Object;
class Sphere;
class Triangle;
class TriangleMesh;
class Instance;

class PointLight;

class Camera;
class Image;
class Scene;
class Material;

extern void ParseFile(FILE* fp);
extern void initOpenGL();
extern Camera* g_camera;
extern Scene* g_scene;
extern Image* g_image;

extern double get_cur_time();
extern unsigned long long get_cur_clock();
extern int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);

typedef struct Profile {
  long n_nodes;
  long n_leaves;
  double build_t;
  unsigned long long build_clock;
  long n_rays;
  long n_traversals;
  long n_raytri_intersect;
  long n_raytri_intersected;
  double render_t;
  unsigned long long render_clock;
} Profile;

extern Profile g_profile;

#define real double

#endif // __MIRO_H__
