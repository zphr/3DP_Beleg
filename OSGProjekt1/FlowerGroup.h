#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedMatrixElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osg/MatrixTransform>
#include <osg/Geode>
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
        string body_file,
        string stamen_file,
        int stamenCount,
        float stamenStartRadius,
        float stamenEndRadius,
        vector<osg::ref_ptr<LeafGeode>> petalGeodes,
        float petalStartRadius,
        float petalEndRadius,
        int petalCount);
    
    FlowerGroup(
        osg::ref_ptr<osg::Node> body,
        osg::ref_ptr<osg::Node> stamen,      /* stamen -> Staubblätter */
        int stamenCount,
        float stamenStartRadius,
        float stamenEndRadius,
        vector<osg::ref_ptr<LeafGeode>> petalGeodes,
        float petalStartRadius,
        float petalEndRadius,
        int petalCount);

    ~FlowerGroup();

    void buildFlower();
    void calcAnimation(unsigned int index,
                              float time,
                              unsigned int samples,
                              osgAnimation::MatrixKeyframeContainer* quatContainer);

  private:
    osg::ref_ptr<osg::Node> _body;

    osg::ref_ptr<osg::Node> _stamen;
    int _stamenCount;
    float _stamenStartRadius, _stamenEndRadius;

    vector<osg::ref_ptr<LeafGeode>> _petalGeodes;
    int _petalCount;
    float _petalStartRadius, _petalEndRadius;

};
