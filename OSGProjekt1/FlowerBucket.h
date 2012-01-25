#pragma once
#include <iostream>
#include <stdlib.h>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include "FencePart.h"
using namespace std;

class FlowerBucket: public osg::Group
{
  public:
    FlowerBucket(osgViewer::Viewer* viewer);
    ~FlowerBucket();
    void buildBucket();

    void setFencePart(osg::Geode* gd);

  protected:
    void buildFence();
    inline void placeFence(bool alongY=false);
    osg::Geometry* buildBoxWalls(osg::ref_ptr<osg::Vec3Array> verts,
                                 float height,
                                 float inset=0.0,
                                 float h_offset=0.0);

    void buildEarth();

    osg::ref_ptr<osg::Node> _fencePartModel;
    osg::ref_ptr<osg::Node> _fencePart;
    float _fenceWidth;
    float _fenceHeight;
    float _fenceOffset;
    float _fenceArrayOffset;
    float _offset;

    unsigned int _fenceCountX;
    unsigned int _fenceCountY;

    osg::ref_ptr<osg::Vec3Array> _verts;
};
