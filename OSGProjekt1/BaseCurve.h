#define _USE_MATH_DEFINES
#include <math.h>

class BaseCurve
{
    public:
         BaseCurve();
         ~BaseCurve();

         virtual osg::Vec4Array* calcPoints(unsigned int resolution)=0;
};
