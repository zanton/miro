#include <stdio.h>
#include <stdlib.h>
#include "Miro.h"
#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Console.h" 
#include "OpenGL.h"

Camera * g_camera = 0;

static bool firstRayTrace = true; 


Camera::Camera() :
	m_bgColor(0,0,0),
	m_renderer(RENDER_OPENGL),
	m_eye(0,0,0),
	m_viewDir(0,0,-1),
	m_up(0,1,0),
	m_lookAt(FLT_MAX, FLT_MAX, FLT_MAX),
	m_fov(45),
	m_distance(0.038627416998)
{
	calcLookAt();
}


Camera::~Camera()
{

}


void
Camera::click(Scene* pScene, Image* pImage)
{
	calcLookAt();
	static bool firstRayTrace = false;

	if (m_renderer == RENDER_OPENGL)
	{
		glDrawBuffer(GL_BACK);
		pScene->openGL(this);
		firstRayTrace = true;
	}
	else if (m_renderer == RENDER_RAYTRACE)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDrawBuffer(GL_FRONT);
		if (firstRayTrace)
		{
			pImage->clear(bgColor());
			pScene->raytraceImage(this, g_image);
			firstRayTrace = false;
		}

		g_image->draw();
	}
}


void
Camera::calcLookAt()
{
	// this is true when a "lookat" is not used in the config file
	if (m_lookAt.x != FLT_MAX)
	{
		setLookAt(m_lookAt);
		m_lookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
	}
}


void
Camera::drawGL()
{
	// set up the screen with our camera parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov(), g_image->width() / (float)g_image->height(), 0.01, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3 vCenter = eye() + viewDir();
	gluLookAt(eye().x,   eye().y,   eye().z,
			  vCenter.x, vCenter.y, vCenter.z,
			  up().x,    up().y,    up().z);
}


Ray
Camera::eyeRay(int x, int y, int imageWidth, int imageHeight)
{
	// first compute the camera coordinate system 
	// ------------------------------------------
	const Vector3 wDir = Vector3(-m_viewDir).normalize();
	const Vector3 uDir = cross(m_up, wDir).normalize();
	const Vector3 vDir = cross(wDir, uDir);

	// next compute the pixel location in the world coordinate system using the camera coordinate system
	// --------------------------------------------------------
	const float aspectRatio = (float)imageWidth / (float)imageHeight;
	const float imPlaneUPos = -((x + 0.5f) / (float)imageWidth - 0.5f);
	const float imPlaneVPos = -((y + 0.5f) / (float)imageHeight - 0.5f);
	const Vector3 pixelPos = m_eye + (aspectRatio * FILM_SIZE * imPlaneUPos) * uDir + (FILM_SIZE * imPlaneVPos) * vDir + m_distance * wDir;

	return Ray(pixelPos, (m_eye - pixelPos).normalize());
}
