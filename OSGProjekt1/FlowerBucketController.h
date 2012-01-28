#pragma once
#include "FlowerBucketCtrlBase.h"
#include "FlowerBucket.h"
#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/BlendFunc>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class FlowerBucketController : public osgGA::GUIEventHandler, public FlowerBucketCtrlBase
{
    public:
    FlowerBucketController(osg::Group* root, float pickExpansion = 10.0);
    FlowerBucketController(osg::Group* root, 
                           vector< osg::ref_ptr<osg::Geometry> > groundGeoms,
                          float pickExpansion = 10.0);
    ~FlowerBucketController();
    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa );
    
    protected:
    inline void setupRectangle(osg::Vec3 hitVec);
    inline void setupPickPlane(osg::Vec3 hitVec);
    inline void removeHelper(osg::Geometry* geom);
    osg::ref_ptr<osg::Geometry> _pickGeom;
    osg::ref_ptr<osg::Vec3Array> _pickVerts;
    float _pickExpansion;
    vector< osg::ref_ptr<osg::Geometry> > _groundGeoms;
    osg::Geometry* _currentGround;
    
    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::Geometry> _rectGeom;
    osg::ref_ptr<osg::Vec3Array> _rectVerts;
    osg::Vec3 _hOffsetVec;
    bool _drawRect;
};

