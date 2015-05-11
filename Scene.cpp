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

  g_profile.n_nodes = 0;
  g_profile.n_leaves = 0;
  g_profile.build_t = get_cur_time();
  g_profile.build_clock = get_cur_clock();
	m_accel.build(&m_objects);
  g_profile.build_clock = get_cur_clock() - g_profile.build_clock;
  g_profile.build_t = get_cur_time() - g_profile.build_t;
  printf("Build time: %lfs (%llu clocks)\n", g_profile.build_t, g_profile.build_clock);
  printf("n_nodes = %ld\n"
         "n_leaves = %ld\n",
         g_profile.n_nodes,
         g_profile.n_leaves);
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
	Ray ray;
	HitInfo hitInfo;
	Vector3 shadeResult;

  g_profile.n_rays = 0;
  g_profile.n_traversals = 0;
  g_profile.n_raytri_intersect = 0;
  g_profile.n_raytri_intersected = 0;
  
  g_profile.render_t = get_cur_time();
  g_profile.render_clock = get_cur_clock();
	// loop over all pixels in the image
	for (int j = 0; j < img->height(); ++j)
	{
		for (int i = 0; i < img->width(); ++i)
		{
      g_profile.n_rays++;
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
  g_profile.render_clock = get_cur_clock() - g_profile.render_clock;
  g_profile.render_t = get_cur_time() - g_profile.render_t;

	printf("Rendering Progress: 100.000%\n");
	debug("done Raytracing!\n");

  printf("Render time: %lfs (%llu clocks)\n", g_profile.render_t, g_profile.render_clock);

  printf("n_rays = %ld\n"
         "n_traversals = %ld\n"
         "n_raytri_intersect = %ld\n"
         "n_raytri_intersected = %ld\n",
         g_profile.n_rays,
         g_profile.n_traversals,
         g_profile.n_raytri_intersect,
         g_profile.n_raytri_intersected);
  printf("%ld & %ld & %lfs (%llu clocks) & %ld & %ld & %ld & %lfs (%llu clocks)\n",
         g_profile.n_nodes,
         g_profile.n_leaves,
         g_profile.build_t, g_profile.build_clock,
         g_profile.n_rays,
         g_profile.n_traversals,
         g_profile.n_raytri_intersect,
         g_profile.render_t, g_profile.render_clock);
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
  //bool hit = m_accel.intersect_naive(minHit, ray, tMin, tMax);
  bool hit = m_accel.intersect(minHit, ray, tMin, tMax);
  //printf("returned from m_accel.intersect()\n");
	return hit;
}
