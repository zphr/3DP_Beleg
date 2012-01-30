#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <iostream>
#include "BranchNode.h"
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once
using namespace std;

class BranchVisitor : public osg::NodeVisitor
{
  
  public:
  
    BranchVisitor(float baseScale,
                  float relativeLevelScale,
                  unsigned int splineRes,
                  unsigned int cylinderRes,
                  osg::Texture2D* branchTex,
                  NaturalCubicSpline* branchProfileSpline = 0,
                  unsigned int leavesLevel = 0,
                  unsigned int leavesCount = 5,
                  float distributionAngle = 144.0,
                  float leavesBaseScale = 1.0,
                  float leavesRelativeScale = 1.0,
                  NaturalCubicSpline* leavesRelativeScaleSpline = 0,
                  NaturalCubicSpline* leavesSpline = 0);
    virtual void apply( osg::Group& group );
    
  protected:
    unsigned int _level;        /* Level-Nr Laufvariable */

    float _baseScale;
    float _relativeLevelScale;
    unsigned int _splineRes;
    unsigned int _cylinderRes;
    osg::Texture2D* _branchTex;
    NaturalCubicSpline* _branchProfileSpline;
    
    unsigned int _leavesLevel;
    unsigned int _leavesCount;
    float _distributionAngle;
    float _leavesBaseScale;
    float _leavesRelativeScale;
    NaturalCubicSpline* _leavesRelativeScaleSpline;
    NaturalCubicSpline* _leavesSpline;
};
