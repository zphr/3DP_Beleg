#pragma once
#include <iostream>
#include <stdlib.h>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/PolygonMode>
#include "FencePart.h"
using namespace std;

class FlowerBucket: public osg::Group
{
  public:
    FlowerBucket(float width = 5.0, float depth = 1.5);
    ~FlowerBucket();
    void buildBucket();
    void swapModels();
    void setFencePart(osg::Geode* gd);
    
    float getWidth() { return _width; }
    float getDepth() { return _depth; }
    float getHeight() { return _fenceHeight + _fenceHOffset; }

  protected:
    void buildFence();
    inline void calcParameters(osg::Node *fencePart,
                               float fp_width, float scale_ratio,
                               float r[4], float s[4], float t[4]);
    inline void placeFence(bool alongY=false);
    osg::Geometry* buildBoxWalls(osg::ref_ptr<osg::Vec3Array> verts,
                                 float height,
                                 float inset=0.0,
                                 float h_offset=0.0,
                                 float tile_u = 1.0,
                                 float tile_v = 1.0);

    void buildEarth();

    osg::ref_ptr<osg::Node> _fencePartModel;
    osg::ref_ptr<osg::Node> _fencePart;

    float _width;
    float _depth;

    float _fenceWidth;
    float _fenceHeight;
    float _fenceDepth;
    float _fenceOffset;
    float _fenceHOffset;
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
    osg::ref_ptr<osg::Texture2D> _texture;

    int _fpIndex;
};
