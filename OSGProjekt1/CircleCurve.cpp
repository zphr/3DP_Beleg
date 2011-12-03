#include "CircleCurve.h"

CircleCurve::CircleCurve(int resolution):BaseCurve(resolution)
{
}

CircleCurve::~CircleCurve()
{
}

osg::Vec4Array* CircleCurve::calcPoints(unsigned int resolution)
{
    float x, y;
    float PI_step = (2*M_PI)/(float)resolution;
    float PI_2 =  2.0f*M_PI;

    osg::Vec4 circle_vert;
    osg::ref_ptr<osg::Vec4Array> cylinder_verts = new osg::Vec4Array;

    for(float j=0; j <= (PI_2-PI_step); j+=PI_step)
    {
        x = cos(j);
        y = sin(j);

        circle_vert = osg::Vec4(x, y, 0, 1);
        circle_vert[3] = 1;

        cylinder_verts->push_back(circle_vert);
    }

    return cylinder_verts.release();
}
