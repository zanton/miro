#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "MiroWindow.h"

#include "assignment1.h"
#include "tribox2.c"


#if defined(__x86_64__)

  static unsigned long long rdtsc() {
    unsigned long long u;
    asm volatile ("rdtsc;shlq $32,%%rdx;orq %%rdx,%%rax":"=a"(u)::"%rdx");
    return u;
  }
  
#elif defined(__sparc__) && defined(__arch64__)
  
  static unsigned long long rdtsc(void) {
    unsigned long long u;
    asm volatile("rd %%tick, %0" : "=r" (u));
    return u;
  }

#else
  
  static unsigned long long rdtsc() {
    unsigned long long u;
    asm volatile ("rdtsc" : "=A" (u));
    return u;
  }
  
#endif

unsigned long long
get_cur_clock() {
  return rdtsc();
}

double
get_cur_time() {
  /* second */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + ((double)tv.tv_usec) * 1.0e-6;
}


void
makeSpiralScene()
{
  g_camera = new Camera;
  g_scene = new Scene;
  g_image = new Image;

  g_image->resize(512, 512);

  // set up the camera
  g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
  g_camera->setEye(Vector3(-5, 2, 3));
  g_camera->setLookAt(Vector3(0, 0, 0));
  g_camera->setUp(Vector3(0, 1, 0));
  g_camera->setFOV(35);

  // create and place a point light source
  PointLight * light = new PointLight;
  light->setPosition(Vector3(-3, 3, 3));
  light->setColor(Vector3(1, 1, 1));
  light->setWattage(1000);
  g_scene->addLight(light);

  // create a spiral of spheres
  const int maxI = 200;
  const float a = 0.10f;
  for (int i = 1; i < maxI; ++i)
  {
    float t = i/float(maxI);
    float theta = 6*PI*t;
    float r = a*theta;
    float x = r*cos(theta);
    float y = r*sin(theta);
    float z = 2*(2*PI*a - r/2);
    Sphere * sphere = new Sphere;
    sphere->setCenter(Vector3(x, y, z));
    sphere->setRadius(r/10);// * (float(i%10) / 10.0f));
    Material* mat = new Lambert(Vector3(float(i%3)/2.0f, float(i%5)/4.0f, float(i%7)/6.0f));
    sphere->setMaterial(mat);
    g_scene->addObject(sphere);
  }

  // let objects do pre-calculations if needed
  g_scene->preCalc();
}

float
cx(float A, float B, float t) {
  return A * cos(B * t);
}

float
cy(float A, float B, float t) {
  return A * sin(B * t);
}

void
makeSpirographScene()
{
  g_camera = new Camera;
  g_scene = new Scene;
  g_image = new Image;

  g_image->resize(512, 512);

  // set up the camera
  g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
  g_camera->setEye(Vector3(-5, 2, 3));
  g_camera->setLookAt(Vector3(0, 0, 0));
  g_camera->setUp(Vector3(0, 1, 0));
  g_camera->setFOV(35);

  // create and place a point light source
  PointLight * light = new PointLight;
  light->setPosition(Vector3(-3, 3, 3));
  light->setColor(Vector3(1, 1, 1));
  light->setWattage(1000);
  g_scene->addLight(light);

  // create a spiral of spheres
  const int maxI = 200;
  const float a = 0.10f;
  for (int i = 1; i < maxI; ++i)
  {
    float AX = 1.0;
    float BX = 1.0;
    float AY = 1.0;
    float BY = 1.0;
    float AZ = 1.0;
    float BZ = 1.0;
    float t = i/float(maxI);
    float theta = 2*PI*t;
    float r = a*theta;
    float x = cx(AX, BX, theta) + cy(AZ, BZ, theta);
    float y = cx(AX, BX, theta) + cy(AY, BY, theta);
    float z = cx(AZ, BZ, theta) + cy(AY, BY, theta);
    Sphere * sphere = new Sphere;
    sphere->setCenter(Vector3(x, y, z));
    sphere->setRadius(r/10);// * (float(i%10) / 10.0f));
    Material* mat = new Lambert(Vector3(float(i%3)/2.0f, float(i%5)/4.0f, float(i%7)/6.0f));
    sphere->setMaterial(mat);
    g_scene->addObject(sphere);
  }

  // let objects do pre-calculations if needed
  g_scene->preCalc();
}


int
main(int argc, char*argv[])
{
  // create a scene
  double time = get_cur_time();
  unsigned long long clock = get_cur_clock();
  int i = 0;
  if (argc > 1) {
    i = atoi(argv[1]);
  }
  switch (i) {
  case 1:
    makeSpiralScene();
    break;
  case 2:
    makeSpirographScene();
    break;
  case 3:
    makeTeapotScene();
    break;
  case 4:
    makeBunny1Scene();
    break;
  case 5:
    makeBunny20Scene();
    break;
  case 6:
    makeSponzaScene();
    break;
  default:
    makeTeapotScene();
  }
  clock = get_cur_clock() - clock;
  time = get_cur_time() - time;
  printf("Load time: %lfs (%llu clocks)\n", time, clock);

  MiroWindow miro(&argc, argv);
  miro.mainLoop();

  return 0; // never executed
}
