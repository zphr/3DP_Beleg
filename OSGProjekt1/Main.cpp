#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <fstream>
#include "LSysPlant.h"
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
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

void LeafTest(osg::ref_ptr<osg::Group> &root)
{

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;

    osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    line_points->push_back(osg::Vec4(0.25,-1,0,1));
    line_points->push_back(osg::Vec4(0, 0,0,1));
    line_points->push_back(osg::Vec4(0.25,1,0,1));

    NaturalCubicSpline line_spline(line_points, 1);
    gd->addDrawable( line_spline.drawSpline() );

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0,0,0,1));
    profile_points->push_back(osg::Vec4(1,0,0,1));

    NaturalCubicSpline profile_spline(profile_points,
                                      12,
                                      new NaturalCubicSpline(line_points, 1)
                                      );

    osg::ref_ptr<LeafGeode> leaf = new LeafGeode(profile_spline, 3, 0.9);
    //root->addChild(leaf);
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( leaf );

    osg::ref_ptr<osg::Vec4Array> branch_points = new osg::Vec4Array;
    branch_points->push_back(osg::Vec4(0,0,0,1));
    branch_points->push_back(osg::Vec4(0,0,2,1));
    branch_points->push_back(osg::Vec4(2,0,5,1));

    // NaturalCubicSpline branch_spline(branch_points, 3);
    // gd->addDrawable( branch_spline.drawTangentFrameAt( 0.5 ) );

    // osg::ref_ptr<osg::MatrixTransform> transVec = new osg::MatrixTransform;
    // osg::Matrix mat = branch_spline.calcFrameAt(0.5);
    // transVec->setMatrix( branch_spline.calcFrameAt(0.5) );
    // transVec->setMatrix( osg::Matrix::translate( 25.0f, 0.0f, 0.0f) );
    // transVec->addChild( leaf.get() );
    // root->addChild(transVec);

    BranchNode branch (0, branch_points, true, leaf_list, 6);
    gd->addDrawable( branch.calcBranch() );
    root->addChild(gd);
    root->addChild(branch.buildLeaves());

}

void extrudeSplineAlongSpline(osg::ref_ptr<osg::Group> &root)
{
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0,0,0,1));
    profile_points->push_back(osg::Vec4(1,0,1,1));
    profile_points->push_back(osg::Vec4(2,0,1,1));

    osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    line_points->push_back(osg::Vec4(0.25,-1,0,1));
    line_points->push_back(osg::Vec4(0, 0,0,1));
    line_points->push_back(osg::Vec4(0.25,1,0,1));

    NaturalCubicSpline line_spline(line_points, 1);
    gd->addDrawable( line_spline.drawSpline() );

    NaturalCubicSpline profile_spline(profile_points ,
                                      12,
                                      new NaturalCubicSpline(line_points, 1) 
                                      );

    osg::StateSet* st = gd->getOrCreateStateSet();

    gd->addDrawable( profile_spline.drawExtrudedCylinder(12, 1.1) );
    
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("blatt.png");
    texture->setImage(image.get());
    
    st->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    
    st->setMode(GL_BLEND, osg::StateAttribute::ON);
    osg::BlendFunc* blend = new osg::BlendFunc;
    blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_DST_ALPHA);

    root->addChild(gd);
}

int main( int argc, char** argv)
{

    osg::ref_ptr<osg::Group> root = new osg::Group;

    //extrudeSplineAlongSpline( root );

    LeafTest( root );

    //osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    //profile_points->push_back(osg::Vec4(0,0,0,1));
    //profile_points->push_back(osg::Vec4(1,0,1,1));
    //profile_points->push_back(osg::Vec4(2,0,1,1));

    //BranchNode branch (0, profile_points);

    //osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    //gd->addDrawable( branch.calcBranch() );

    //root->addChild( gd );
    //root->addChild( branch.buildLeaves() );
    
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
    viewer.setSceneData( root );
    return viewer.run();
}

