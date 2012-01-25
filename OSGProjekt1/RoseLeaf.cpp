#include "RoseLeaf.h"

RoseLeaf::RoseLeaf()
{
    osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    line_points->push_back(osg::Vec4(0.25,-0.85,0,1));
    line_points->push_back(osg::Vec4(0,    0,  0,1));
    line_points->push_back(osg::Vec4(0.25,0.85,0,1));

    NaturalCubicSpline line_spline(line_points, 1);

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0,0,0,1));
    profile_points->push_back(osg::Vec4(0.3,0,0.15,1));
    profile_points->push_back(osg::Vec4(1,0,0,1));

    NaturalCubicSpline profile_spline(profile_points,
                                      12,
                                      new NaturalCubicSpline(line_points, 1)
                                      );
    addDrawable( profile_spline.drawExtrudedCylinder(3, 0.5) );

    setTexture("blatt.png");
}

RoseLeaf::~RoseLeaf()
{

}
