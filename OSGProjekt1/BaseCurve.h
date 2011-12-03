#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#define _USE_MATH_DEFINES
#include <math.h>

class BaseCurve
{
    protected:
         int _resolution;

    public:
         BaseCurve(int resolution=3);
         ~BaseCurve();

         virtual osg::Vec4Array* calcPoints(unsigned int resolution);
};
