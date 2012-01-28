#pragma once
#include "FlowerBucket.h"
#include <osg/Group>

class FlowerBucketCtrlBase
{
  public:
    FlowerBucketCtrlBase(){};
  
  protected:
    static osg::ref_ptr<FlowerBucket> _currentBucket;
};

    
