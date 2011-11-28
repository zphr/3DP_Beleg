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

struct PlantStack
{
  osg::Matrix rot_mat;
  osg::Vec4 dist;
};

int main( int argc, char** argv)
{

    osg::ref_ptr<osg::Geode> root = new osg::Geode;

    osg::ref_ptr<osg::Vec4Array> points = new osg::Vec4Array;
    points->push_back(osg::Vec4(0,-5,0,1));
    points->push_back(osg::Vec4(0,0,0,1));
    points->push_back(osg::Vec4(1,1,0,1));

    /*NaturalCubicSpline spline(points);*/
    /*root->addDrawable( spline._geometry );*/

    map<char, string> rules;
    /*rules['A'] = "F[{(x/1.456)A(x/1.456)][}(x/1.456)A(x/1.456)][&(x/1.456)A(x/1.456)][^(x/1.456)A(x/1.456)]";*/

    /*rules['A'] = "[&FLA]{{{{{[&FLA]{{{{{{[&FLA]";*/
    /*rules['F'] = "S{{{{F";*/
    /*rules['S'] = "F";*/

    rules['X'] = "F[[{X]&X]F[^}}(34.4)FX]&XF&F";
    rules['F'] = "FF";

    //rules['X'] = "FF[&F]F";

    float delta = 22.5;
    osg::Vec4 dist (0.0, 0.0, 0.5, 1.0);
    osg::Matrix rot_mat;

    LSysPlant plant(2, delta, rules, rules['X'],  dist, rot_mat);

    plant.drawPlant(root);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}

