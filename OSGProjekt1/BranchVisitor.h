#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <iostream>
#include "BranchNode.h"
#pragma once
using namespace std;

class BranchVisitor : public osg::NodeVisitor
{
  
public:
  
 BranchVisitor() : _level(0)
    { setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN); }
  
  virtual void apply( osg::Group& group );
    
 protected:
  unsigned int _level;
};
