#include "FlowerGroup.h"
#pragma once

class RoseFlower: public FlowerGroup
{
 public:
  RoseFlower();
  ~RoseFlower();
  
  void calcAnimation(unsigned int index, osg::MatrixTransform* trans);
  
};