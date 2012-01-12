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
#include "FlowerGroup.h"
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

osg::Geometry* createQuad()
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
    vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
    vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
    vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );
    
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
    
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );
    colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
    colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
    colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
    
    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->setNormalArray( normals.get() );
    quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
    quad->setColorArray( colors.get() );
    quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );
    return quad.release();
}

class DynamicQuadCallback : public osg::Drawable::UpdateCallback
{
public:
    virtual void update( osg::NodeVisitor*, osg::Drawable* drawable );
};

void DynamicQuadCallback::update( osg::NodeVisitor*,
                                  osg::Drawable* drawable )
{
    osg::Geometry* quad = static_cast<osg::Geometry*>( drawable );
    if ( !quad ) return;
    
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(
        quad->getVertexArray() );
    if ( !vertices ) return;
    
    osg::Quat quat(osg::PI*0.01, osg::X_AXIS);
    vertices->back() = quat * vertices->back();
    
    quad->dirtyDisplayList();
    quad->dirtyBound();
}


void FlowerTest(osg::ref_ptr<osg::Group> &root)
{
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;

    // osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    // line_points->push_back(osg::Vec4(-0.5,-1,0,1));
    // line_points->push_back(osg::Vec4(0, 0,0,1));
    // line_points->push_back(osg::Vec4(-0.5,1,0,1));

    osg::ref_ptr<osg::Vec4Array> line_points = new osg::Vec4Array;
    line_points->push_back(osg::Vec4(-0.75,  -1,0,1));
    line_points->push_back(osg::Vec4(-0.15,-0.5,0,1));
    line_points->push_back(osg::Vec4(   0,   0,0,1));
    line_points->push_back(osg::Vec4(-0.15, 0.5,0,1));
    line_points->push_back(osg::Vec4(-0.75,   1,0,1));

    NaturalCubicSpline line_spline(line_points, 1);
    gd->addDrawable( line_spline.drawSpline() );

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0,0,0,1));
    profile_points->push_back(osg::Vec4(0.3,0,0.55,1));
    profile_points->push_back(osg::Vec4(0,0,1,1));

    NaturalCubicSpline profile_spline(profile_points,
                                      12,
                                      new NaturalCubicSpline(line_points, 1));
    
    osg::ref_ptr<LeafGeode> leaf = new LeafGeode(profile_spline, 3, 0.5, "bluete1.png");
    //root->addChild(leaf);
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( leaf );
    
    osg::ref_ptr<osg::Node> innen = osgDB::readNodeFile("3d/Bluete_innen.obj");
    root->addChild( innen );
    
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("bluete1.png");
    texture->setImage(image.get());

    osg::StateSet* state = innen->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    
    state->setMode(GL_BLEND, osg::StateAttribute::ON);
    osg::BlendFunc* blend = new osg::BlendFunc;
    blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        
    state->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
	
    alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.6f);
    state->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );
    
    osg::ref_ptr<FlowerGroup> flower = new FlowerGroup("3d/Bluete.obj",
                                                       "",
                                                       0, 0, 0,
                                                       leaf_list,
                                                       0.2, 1.0, 5);
    root->addChild( flower );

}

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

    osg::ref_ptr<LeafGeode> leaf = new LeafGeode(profile_spline, 3, 0.5, "blatt.png");
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

    osg::ref_ptr<BranchNode> branch = new BranchNode(0, branch_points, true, leaf_list, 6);
    branch->buildBranch();
    root->addChild( branch.get() );

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

void PlantStringTest(osg::ref_ptr<osg::Group> &root)
{

    map<char, string> rules;

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

    rules['S'] = "[A(x)]+(66)[A(1.0)]+(66)[A(1.0)]+(66)[A(1.0)]F";
    rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)[S]";

    // // Test-Baum ausm Buch
    // rules['S'] = "F[&S][^S]FS";
    // rules['F'] = "FF";
    
    float delta = 22.5;
    osg::Vec4 dist (0.0, 0.0, 5.0, 1.0);
    osg::Matrix rot_mat;

    LSysPlant plant(5, delta, rules, rules['S'],  dist, rot_mat);

    root->addChild( plant.buildPlant() );
}

void DynamicTest(osg::ref_ptr<osg::Group> &root)
{
    osg::Geometry* quad = createQuad();
    quad->setDataVariance( osg::Object::DYNAMIC );
    quad->setUpdateCallback( new DynamicQuadCallback );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( quad );
    root->addChild( gd );
}

int main( int argc, char** argv)
{

    osg::ref_ptr<osg::Group> root = new osg::Group;

    // extrudeSplineAlongSpline( root );
    // LeafTest( root );
    FlowerTest( root );
    // DynamicTest( root );
    // PlantStringTest( root );

    osgViewer::Viewer viewer;

    // osgUtil::CullVisitor::apply(*(root.get()));

    // osg::ref_ptr<osgUtil::CullVisitor> cv = osgUtil::CullVisitor::create();
    // root->accept(*(cv.get()));
    viewer.setSceneData( root );

    return viewer.run();
}

