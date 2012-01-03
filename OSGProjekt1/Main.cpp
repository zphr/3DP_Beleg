#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <fstream>
#include "LSysPlant.h"
#include "NaturalCubicSpline.h"
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/PolygonMode>

int main( int argc, char** argv)
{

    osg::ref_ptr<osg::Geode> root = new osg::Geode;

    
    // osg::ref_ptr<osg::Vec4Array> points = new osg::Vec4Array;
    // points->push_back(osg::Vec4(1,0,0,1));
    // points->push_back(osg::Vec4(1,0,1,1));
    // points->push_back(osg::Vec4(2,0,1,1));

    // NaturalCubicSpline spline(points, 1);
    // root->addDrawable( spline.drawSpline() );


    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0,0,0,1));
    profile_points->push_back(osg::Vec4(1,0,1,1));
    profile_points->push_back(osg::Vec4(2,0,1,1));

    // profile_points->push_back(osg::Vec4(0,0,2,1));
    // profile_points->push_back(osg::Vec4(0.25,0.75,0,1));
    // profile_points->push_back(osg::Vec4(0.9,0.25,3,1));
    // profile_points->push_back(osg::Vec4(1,.5,0,1));
    
    osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    line_points->push_back(osg::Vec4(0.25,-1,0,1));
    line_points->push_back(osg::Vec4(0, 0,0,1));
    line_points->push_back(osg::Vec4(0.25,1,0,1));

    NaturalCubicSpline line_spline(line_points, 1);
    root->addDrawable( line_spline.drawSpline() );

    NaturalCubicSpline profile_spline(profile_points ,
                                      12,
                                      new NaturalCubicSpline(line_points, 1) 
                                      );
    // root->addDrawable( profile_spline.drawTangentFrame( profile_spline.calcFrameAt(0.01) ));
    root->addDrawable( profile_spline.drawExtrudedCylinder(8, 0.1) );
    root->addDrawable( profile_spline.drawSpline() );

    // osg::ref_ptr<osg::Vec4Array> points = new osg::Vec4Array;
    // points->push_back(osg::Vec4(1,0,0,1));
    // points->push_back(osg::Vec4(1,0,1,1));
    // points->push_back(osg::Vec4(2,0,1,1));

    // NaturalCubicSpline spline(points, 6, CircleCurve(7), &profile_spline);
    // root->addDrawable( spline.drawExtrudedCylinder(12, 0.25f) );

    //root->addDrawable( spline.drawSpline() );
    //root->addDrawable( spline.getPointSprites(root) );
    ////root->addDrawable( spline.drawExtrudedCylinder(3, 0.25f) );
    //// root->addDrawable( spline.drawTangentCoordinateSystems() );

    //osg::ref_ptr<osg::Vec4Array> points2 = new osg::Vec4Array;
    //points2->push_back(osg::Vec4(2,0,0,1));
    //points2->push_back(osg::Vec4(1,1,0,1));
    //points2->push_back(osg::Vec4(2,1,0,1));
    //points2->push_back(osg::Vec4(1,0,0,1));

    //NaturalCubicSpline spline2(points2);
    //root->addDrawable( spline2.drawSpline() );
    //root->addDrawable( spline2.getPointSprites(root) );



    // map<char, string> rules;

    // rules['A'] = "F[{(x/1.456)A(x/1.456)][}(x/1.456)A(x/1.456)][&(x/1.456)A(x/1.456)][^(x/1.456)A(x/1.456)]";
    
    // rules['A'] = "F[&(x/1.456)A(x/1.456)]";

    // rules['A'] = "SS[&FLA]{{&{{&{[&FLA]{{&{[&FLA]";
    // rules['F'] = "S{{{{F";
    // rules['S'] = "F";

    // rules['A'] = "F[[{F]&A]F[^}}(34.4)FA]&AF&F";
    // rules['F'] = "fF";

    // rules['F'] = "FF[&+(45)FF][&+(90)FF][&+(180)FF]";
    
    // rules['S'] = "F[&+(-45)A][&+(45)A]{(8)FS";
    // rules['A'] = "^F[&FA&F][^FA^F]F";

    // rules['S'] = "[A(x)]+(66)[A(1.0)]+(66)[A(1.0)]+(66)[A(1.0)]F";
    // rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)[S]";

    // // Test-Baum ausm Buch
    // rules['S'] = "F[&S][^S]FS";
    // rules['F'] = "FF";
    
    // float delta = 22.5;
    // osg::Vec4 dist (0.0, 0.0, 5.0, 1.0);
    // osg::Matrix rot_mat;

    // LSysPlant plant(5, delta, rules, rules['S'],  dist, rot_mat);

    // plant.drawPlant(root);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}

