#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <iostream>
#include "BranchNode.h"
#pragma once
using namespace std;

class BranchVisitor : public osg::NodeVisitor
{
  
public:
  
  BranchVisitor(unsigned int leavesLevel,
                unsigned int leavesCount = 5,
                float distributionAngle = 144.0,
                NaturalCubicSpline* profileSpline = 0,
                NaturalCubicSpline* leavesSpline = 0);
  virtual void apply( osg::Group& group );
    
 protected:
  unsigned int _level;
  unsigned int _leavesLevel;
  unsigned int _leavesCount;
  float _distributionAngle;
  NaturalCubicSpline* _profileSpline;
  NaturalCubicSpline* _leavesSpline;
};
