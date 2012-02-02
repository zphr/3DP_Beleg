#pragma once
#include "LSysPlant.h"
#include "RoseLeaf.h"

class RosePlant: public LSysPlant
{
    public:
    RosePlant(unsigned int traversalMask);
    ~RosePlant();
    osg::Group* getPlant();
    osg::Group* releasePlant();

    protected:
    unsigned int _traversalMask;

    static const osg::ref_ptr<osg::Image> _stengelImg;
    osg::ref_ptr<osg::Group> _plant;

};
