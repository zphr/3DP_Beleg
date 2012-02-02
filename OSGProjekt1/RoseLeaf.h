#pragma once
#include "LeafGeode.h"

class RoseLeaf: public LeafGeode
{
  public:
    RoseLeaf();
    ~RoseLeaf();

  protected:
    static const osg::ref_ptr<osg::Image> _leafImg;
};
