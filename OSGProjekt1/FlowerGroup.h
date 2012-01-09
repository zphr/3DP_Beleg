#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once
using namespace std;

class FlowerGroup: public osg::Group
{

  public:
    FlowerGroup(
        osg::ref_ptr<osg::Geode> body,
        osg::ref_ptr<osg::Geode> stamen,      /* stamen -> Staubblätter */
        int stamenCount,
        float stamenStartRadius,
        float stamenEndRadius,
        vector<osg::ref_ptr<LeafGeode>> petalGeodes,
        float petalStartRadius,
        float petalEndRadius,
        int petalCount);

    ~FlowerGroup();

    void buildFlower();

  private:
    osg::ref_ptr<osg::Geode> _body;

    osg::ref_ptr<osg::Geode> _stamen;
    int _stamenCount;
    float _stamenStartRadius, _stamenEndRadius;

    vector<osg::ref_ptr<LeafGeode>> _petalGeodes;
    int _petalCount;
    float _petalStartRadius, _petalEndRadius;

};
