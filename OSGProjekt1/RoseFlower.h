#include "FlowerGroup.h"
#pragma once

class RoseFlower: public FlowerGroup
{
  public:
    RoseFlower();
    ~RoseFlower();
  
    void calcAnimation(unsigned int index, osg::MatrixTransform* trans);
    osg::Matrix animationStep(unsigned int index, float percent);

  protected:
    static const osg::ref_ptr<osg::Image> _leafImg;
    static const osg::ref_ptr<osg::Image> _bodyImg;
    static const osg::ref_ptr<osg::Node>  _bodyModel;
    static const osg::ref_ptr<osg::Node>  _insideModel;
    static const osg::ref_ptr<osg::Image> _insideImg;
  
};
