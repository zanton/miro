#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

Scene * g_scene = 0;

void
Scene::openGL(Camera *cam)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam->drawGL();

	// draw objects
	for (size_t i = 0; i < m_objects.size(); ++i) 
	{
		m_objects[i]->renderGL();
	}

	glutSwapBuffers();
}

void
Scene::preCalc()
{
	Objects::iterator it;
	for (it = m_objects.begin(); it != m_objects.end(); it++)
	{
		Object* pObject = *it;
		pObject->preCalc();
	}
	Lights::iterator lit;
	for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
	{
		PointLight* pLight = *lit;
		pLight->preCalc();
	}

  double time = get_cur_time();
	m_accel.build(&m_objects);
  time = get_cur_time() - time;
  printf("Build time: %lfs\n", time);
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
  double time = get_cur_time();
  
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
			ray = cam->eyeRay(i, j, img->width(), img->height());
			if (trace(hitInfo, ray))
			{
        //printf("0\n");
				shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
        //printf("1\n");
				img->setPixel(i, j, shadeResult);
        //printf("2\n");
			}
		}
		img->drawScanline(j);
		glFinish();
		printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
		fflush(stdout);
	}

	printf("Rendering Progress: 100.000%\n");
	debug("done Raytracing!\n");

  time = get_cur_time() - time;
  printf("Render time: %lfs\n", time);
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
  //bool hit = m_accel.intersect_naive(minHit, ray, tMin, tMax);
  bool hit = m_accel.intersect(minHit, ray, tMin, tMax);
  //printf("returned from m_accel.intersect()\n");
	return hit;
}
