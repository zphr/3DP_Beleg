#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once
using namespace std;

class BaseCurve
{
    protected:
         int _resolution;

    public:
         /* BaseCurve(int resolution=3);  */
         /* ~BaseCurve(); */

         virtual osg::Vec4Array* calcPoints(unsigned int resolution) =0;
         virtual osg::Geometry* buildMeshAlongPath(unsigned int resolution,
                                           float scale,
                                           const vector<osg::Matrix> &matrices,
                                           const osg::ref_ptr<osg::Vec3Array> &vertices,
                                           const vector<float> &profile_scale = vector<float>()) =0; 
};
