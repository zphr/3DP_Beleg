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

#include "TrackballAxisDragger.hpp"
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
    void toggleShadows();

protected:
    const unsigned int _cullMask;
    const unsigned int _rcvShadowMask;
    const unsigned int _castShadowMask;
    const unsigned int _intersectMask;

    osg::ref_ptr<Sun> _sun;

    osg::ref_ptr<osgShadow::ShadowedScene> _root;
    osgViewer::Viewer _viewer;

    static bool _renderShadows;

    void setupLight();
    void setupController();
    void setupModels();
    void setupRose();
        
};

class FinalController : public osgGA::GUIEventHandler
{ 
  
public:
    FinalController(FinalScene* root)
        :_root(root) {;}

    ~FinalController(){;}

    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa )
        {
            if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN &&
                ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_SHIFT)
            {
                switch( ea.getKey() )
                {
                case 'S':
                    _root->toggleShadows();
                    return false;
                }
            }
            return false;
        }

protected:
    FinalScene* _root;
};
