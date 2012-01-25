#pragma once
#include <iostream>
#include <stdlib.h>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/TextureCubeMap>
#include <osg/TexGen>
#include "FencePart.h"
using namespace std;

class FlowerBucket: public osg::Group
{
  public:
    FlowerBucket();
    ~FlowerBucket();
    void buildBucket();
    void swapModels();
    void setFencePart(osg::Geode* gd);

  protected:
    void buildFence();
    inline void calcParameters(float fp_width, float scale_ratio = 1.0);
    inline void placeFence(bool alongY=false);
    osg::Geometry* buildBoxWalls(osg::ref_ptr<osg::Vec3Array> verts,
                                 float height,
                                 float inset=0.0,
                                 float h_offset=0.0);

    void buildEarth();

    osg::ref_ptr<osg::Node> _fencePartModel;
    osg::ref_ptr<osg::Node> _fencePart;

    float _width;
    float _depth;

    float _fenceWidth;
    float _fenceHeight;
    float _fenceDepth;
    float _fenceOffset;
    float _fenceArrayOffsetX;
    float _fenceArrayOffsetY;
    
    float _fenceModelWidth;
    float _fenceModelHeight;
    float _fenceModelDepth;
    float _fenceModelOffset;
    float _fenceModelArrayOffsetX;
    float _fenceModelArrayOffsetY;
    bool _useModel;

    unsigned int _fenceCountX;
    unsigned int _fenceCountY;
    
    osg::ref_ptr<osg::Vec3Array> _verts;
    osg::ref_ptr<osg::Geode> _fpGeode;

    int _fpIndex;
};
