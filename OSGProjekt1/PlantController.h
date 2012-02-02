#pragma once
#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgManipulator/TrackballDragger>
#include <vector>
#include "RosePlant.hpp"
#include "FlowerBucketCtrlBase.h"
// #define _USE_MATH_DEFINES
// #include <math.h>
using namespace std;

class PlantController : public osgGA::GUIEventHandler, public FlowerBucketCtrlBase
{ 
 public:
  PlantController(osg::Group* root,
                  unsigned int traversalMask);
  ~PlantController();
  virtual bool handle( const osgGA::GUIEventAdapter& ea,
                       osgGA::GUIActionAdapter& aa );

 protected:
  unsigned int _traversalMask;
  osg::ref_ptr<osg::Group> _root;
};
