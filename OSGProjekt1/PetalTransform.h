#pragma once
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "Sun.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once
using namespace std;

class PetalTransform: public osg::MatrixTransform
{
  public:
    PetalTransform(unsigned int index, float startRadius, float distributionAngle);
    ~PetalTransform();
    
    void animationStep(float percent);

  protected:
    unsigned int _index;
    float _startRadius;
    float _distributionAngle;
};

class PetalTransformCallback : public osg::NodeCallback 
{
  public:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::ref_ptr<PetalTransform> pt_trans = 
            dynamic_cast<PetalTransform*> (node);

        if(pt_trans)
        {
            double step = 0.0001;
            static double t = 0.0;

            if(t >= 1.0)
                t = 0;

            // pt_trans->animationStep((float)(t+=s tep));
            pt_trans->animationStep(Sun::_zPos);
        }

        traverse(node, nv); 
    }
};
