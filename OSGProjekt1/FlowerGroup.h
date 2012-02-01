#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedMatrixElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "PetalTransform.h"
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include <iostream>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once
using namespace std;

class FlowerGroup: public osg::Group
{

  public:
    
    FlowerGroup(
        string bodyFile,
        string insideFile,
        vector<osg::ref_ptr<LeafGeode>> petalGeodes,
        float petalStartRadius,
        float petalEndRadius,
        float distributionAngle,
        float time = 6.0,
        unsigned int samples = 8);
    
    FlowerGroup(
        osg::ref_ptr<osg::Node> body,
        osg::ref_ptr<osg::Node> inside,
        vector<osg::ref_ptr<LeafGeode>> petalGeodes,
        float petalStartRadius,
        float petalEndRadius,
        float distributionAngle,
        float time = 6.0,
        unsigned int samples = 8);

    ~FlowerGroup();

  protected:
    FlowerGroup();
    
    virtual void buildFlower(bool animate);
    virtual void calcAnimation(unsigned int index,
                               osg::MatrixTransform* trans);

    osg::ref_ptr<osg::Node> _body;
    osg::ref_ptr<osg::Node> _inside;

    vector<osg::ref_ptr<LeafGeode>> _petalGeodes;
    int _petalCount;
    float _petalStartRadius, _petalEndRadius;
    float _distributionAngle;
    osg::ref_ptr<osgAnimation::BasicAnimationManager> _manager;
    unsigned int _samples;
    float _time;

    osg::ref_ptr<osg::Material> _leavesMaterial; /* Blütenblatt-Material */
    osg::ref_ptr<osg::Material> _baseMaterial; /* Knospen-Material */

};

