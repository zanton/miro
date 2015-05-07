#ifndef MIRO_OBJECT_H_INCLUDED
#define MIRO_OBJECT_H_INCLUDED

#include <vector>
#include "Miro.h"
#include "Material.h"

class Object
{
public:
	Object() {}
	virtual ~Object() {}

	void setMaterial(const Material* m) {m_material = m;}

	virtual void renderGL() {}
	virtual void preCalc() {}

	virtual bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX) = 0;

protected:
    const Material* m_material;
};

typedef std::vector<Object*> Objects;

#endif // MIRO_OBJECT_H_INCLUDED
