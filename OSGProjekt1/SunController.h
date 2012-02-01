#pragma once
#include "Sun.h"
#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgManipulator/TrackballDragger>
#include <vector>
// #define _USE_MATH_DEFINES
// #include <math.h>
using namespace std;

class SunController : public osgGA::GUIEventHandler
{ 
  
public:
    SunController( Sun* sun);
    ~SunController();
    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa );

protected:
    osg::ref_ptr<Sun> _sun;
};
