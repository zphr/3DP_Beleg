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
#pragma once
#include "LSysPlant.h"
#include "Sun.h"
#include "SunController.h"
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include "FlowerGroup.h"
#include "FlowerBucket.h"
#include "RoseLeaf.h"
#include "RoseFlower.h"
#include "FencePart.h"
#include "FencePartController.h"
#include "FlowerBucketController.h"
#include "PlantController.h"

#include <iostream>
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class FinalScene
{
public:
    FinalScene();
    ~FinalScene();

    void run();

protected:
    const unsigned int _cullMask;
    const unsigned int _rcvShadowMask;
    const unsigned int _castShadowMask;
    const unsigned int _intersectMask;

    osg::ref_ptr<Sun> _sun;

    osg::ref_ptr<osgShadow::ShadowedScene> _root;
    osgViewer::Viewer _viewer;

    void setupLight();
    void setupController();
    void setupModels();
    void setupRose();
        
};
