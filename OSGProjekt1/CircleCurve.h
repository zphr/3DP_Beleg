#include "BaseCurve.h"

class CircleCurve: public BaseCurve
{
    public:
         CircleCurve();
         ~CircleCurve();

         osg::Vec4Array calcPoints(unsigned int resolution);
}
