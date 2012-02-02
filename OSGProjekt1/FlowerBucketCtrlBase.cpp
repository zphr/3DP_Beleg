#include "FlowerBucketCtrlBase.h"

osg::ref_ptr<FlowerBucket> FlowerBucketCtrlBase::_currentBucket = 0;
const osg::Vec4 FlowerBucketCtrlBase::_helperColor =
    osg::Vec4( osg::Vec4(0.362, 0.800, 0.233, 1.0) );
