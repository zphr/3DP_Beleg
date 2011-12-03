#include "BaseCurve.h"

class CircleCurve: public BaseCurve
{
    public:
         CircleCurve(int resolution=3);
         ~CircleCurve();

         osg::Vec4Array* calcPoints(unsigned int resolution);
};
