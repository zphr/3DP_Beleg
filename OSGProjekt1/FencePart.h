#pragma once
#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/FrontFace>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>
using namespace std;

class FencePart: public osg::Geode
{
  public:
    FencePart(osg::ref_ptr<osg::Geometry> &geom,
              float extrudeWidth = 0.05);
    ~FencePart();
    void buildFencePart(osg::ref_ptr<osg::Geometry> &geom);

 protected:
    unsigned int _windowWidth, _windowHeight;
    float _extrudeWidth;
    osg::BoundingBox _bbox;
};
