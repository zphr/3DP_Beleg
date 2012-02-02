#include <osg/Geode>
#include <osg/Depth>
#include <osgDB/ReadFile>
#include <osg/AlphaFunc>
#include "NaturalCubicSpline.h"
#include <iostream>
#include <string>
#pragma once
using namespace std;

class LeafGeode: public osg::Geode
{
    public:
        
        LeafGeode(NaturalCubicSpline spline,
                  int resolution=3,
                  float scale=1.0f,
                  osg::Image* image = 0);

        LeafGeode(NaturalCubicSpline spline,
                  NaturalCubicSpline extrudeShape,
                  int resolution=3,
                  float scale=1.0f,
                  osg::Image* image = 0);

        ~LeafGeode();

        inline void setTexture(osg::Image* image);

    protected:
        LeafGeode();
        NaturalCubicSpline _spline;
        NaturalCubicSpline _extrudeShape;

};
