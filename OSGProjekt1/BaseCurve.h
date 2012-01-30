#pragma once
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class NaturalCubicSpline;

class BaseCurve
{
    protected:
         int _resolution;

    public:
         /* BaseCurve(int resolution=3);  */
         /* ~BaseCurve(); */

         virtual osg::Vec4Array* calcPoints(unsigned int resolution) =0;
         virtual osg::Geometry* buildMeshAlongPath(unsigned int extrudeShapeRes,
                                           float scale,
                                           const vector<osg::Matrix> &matrices,
                                           const osg::ref_ptr<osg::Vec3Array> &vertices,
                                           BaseCurve* profileCurve = 0) =0;
         virtual float calcProfileAtPercent(float percent) =0;
};
