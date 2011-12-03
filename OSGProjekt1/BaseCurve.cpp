#include "BaseCurve.h"

BaseCurve::BaseCurve(int resolution)
    : _resolution(resolution)
{

}

BaseCurve::~BaseCurve()
{

}

osg::Vec4Array* BaseCurve::calcPoints(unsigned int resolution)
{
    return 0;
}
