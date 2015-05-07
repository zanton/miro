#ifndef MIRO_MATERIAL_H_INCLUDED
#define MIRO_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
	Material();
	virtual ~Material();

	virtual void preCalc() {}

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const;
};

#endif // MIRO_MATERIAL_H_INCLUDED
