#pragma once
#include "FlowerBucket.h"
#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <vector>

/* #define _USE_MATH_DEFINES */
/* #include <math.h> */
using namespace std;

class FencePartController : public osgGA::GUIEventHandler
{ 
  
  public:
    FencePartController(FlowerBucket* root);
    ~FencePartController();
    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa );

  protected:
    unsigned int _windowWidth, _windowHeight;
    osg::ref_ptr<osg::Geometry> _geom;
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<FlowerBucket> _root;

};
