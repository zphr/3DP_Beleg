#pragma once
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/AutoTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>

#include "LSysPlant.h"
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include "FlowerGroup.h"
#include "FlowerBucket.h"
#include "RoseFlower.h"
#include "RoseLeaf.h"
#include "FencePart.h"
#include "FencePartController.h"
#include "FlowerBucketController.h"

#include <iostream>
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class FinalScene()
{
  public:
    FinalScene();
    ~FinalScene();

  protected:
    const unsigned int _cullMask;
    const unsigned int _rcvShadowMask;
    const unsigned int _castShadowMask;

    osg::ref_ptr<osg::LightSource> _shadowSrc;
    osg::ref_ptr<osgShadow::ShadowMap> _shadowMap;

    osg::ref_ptr<osgShadow::ShadowMap> _shadowMap;
    osg::ref_ptr<osgShadow::ShadowedScene> _root;
    osgViewer::Viewer viewer;

    void setupLight();
    void setupShadows();
    void setupController();
    void setupBackground();
        
};
