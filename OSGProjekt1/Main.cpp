#pragma once
#include <iostream>
#include <fstream>

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/Plane>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/AutoTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>
// #include <osgManipulator/CompositeDragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/ScaleAxisDragger>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/RotateSphereDragger>
#include <osgUtil/LineSegmentIntersector>

#include "LSysPlant.h"
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include "FlowerGroup.h"
#include "FlowerBucket.h"
#include "RoseFlower.h"
#include "RoseLeaf.h"
#include "FencePart.h"
#include "FencePartController.h"
#include "FlowerBucketController.h"

#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class ModelController : public osgGA::GUIEventHandler
{
public:
    ModelController(osg::ref_ptr<osg::Geometry> &geom, osg::Group* root) {
        _geom = geom.get();
        _vertices = static_cast<osg::Vec3Array*>(geom->getVertexArray() );
        _root = root;
    }
    
    ~ModelController()
        {
            _geom.release();
        }
    
    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa );
protected:
    osg::ref_ptr<osg::Geometry> _geom;
    osg::Vec3Array* _vertices;
    osg::Group* _root;
    bool _turn;
};

bool ModelController::handle( const osgGA::GUIEventAdapter& ea,
                              osgGA::GUIActionAdapter& aa )
{
    switch ( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::RELEASE:
        
        if(ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
           ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
        {
            cout << ea.getX() << " " << ea.getY() << endl;
            
            _vertices->push_back(osg::Vec3(ea.getX(), ea.getY(), 0));
            _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );
        
            _geom->dirtyDisplayList();
            _geom->dirtyBound();
        }
        break;
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch ( ea.getKey() )
        {
        case 'a': case 'A':
        {
            osg::ref_ptr<FencePart> fence_part = new FencePart(_geom);
            _root->addChild( fence_part.release() );
            break;
        }
        case 'd': case 'D':
        {
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            _geom->setVertexArray( vertices.get() );    
            _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, vertices->getNumElements()) );
            _vertices = static_cast<osg::Vec3Array*>(_geom->getVertexArray() );

            _geom->dirtyDisplayList();
            _geom->dirtyBound();
        }
        }
    }
    return false;
}

void DrawLineTest(osg::ref_ptr<osg::Group> &root, osgViewer::Viewer &viewer)
{

    int window_height = 1050;
    int window_width  = 1680;

    // Projection node for defining view frustrum for HUD:
    osg::Projection* HUDProjectionMatrix = new osg::Projection;

    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,window_width,0,window_height));
      
    // For the HUD model view matrix use an identity matrix:
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph:
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    // Add the HUD projection matrix as a child of the root node
    // and the HUD model view matrix as a child of the projection matrix
    // Anything under this node will be viewed using this projection matrix
    // and positioned with this model view matrix.
    root->addChild(HUDProjectionMatrix);
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    geom->setVertexArray( vertices.get() );    
    geom->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( geom.get() );

    HUDModelViewMatrix->addChild( gd.get() );

    // Create and set up a state set using the texture from above:
    osg::StateSet* HUDStateSet = new osg::StateSet();
    gd->setStateSet(HUDStateSet);

    // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( 11, "RenderBin");

    osg::ref_ptr<osg::Vec4Array> points = new osg::Vec4Array();
    osg::ref_ptr<ModelController> ctrler = new ModelController(geom, root.get());
    viewer.addEventHandler( ctrler.get() );
    // viewer.getCamera()->setAllowEventFocus( false );
}

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
                                                       leaf_list,
                                                       0.2, 1.0,
                                                       137.2);
    root->addChild( flower );

}

void LeafTest(osg::ref_ptr<osg::Group> &root)
{
    osg::ref_ptr<RoseLeaf> rose_leaf = new RoseLeaf();
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( rose_leaf.release() );

    osg::ref_ptr<osg::Vec4Array> branch_points = new osg::Vec4Array;
    branch_points->push_back(osg::Vec4(0,0,0,1));
    branch_points->push_back(osg::Vec4(0,0,2,1));
    branch_points->push_back(osg::Vec4(2,0,5,1));

    osg::ref_ptr<BranchNode> branch = new BranchNode(0,
                                                     branch_points,
                                                     0,
                                                     leaf_list);
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

    rules['S'] = "[A(x)]+(66)[A(1.0)]+(66)[A(1.0)]+(66)[A(1.0)]F@";
    rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)[S]";

    // // Test-Baum ausm Buch
    // rules['S'] = "F[&S][^S]FS";
    // rules['F'] = "FF";

    float delta = 22.5;
    osg::Vec4 dist (0.0, 0.0, 1.0, 1.0);
    osg::Matrix rot_mat;

    osg::ref_ptr<RoseLeaf> rose_leaf = new RoseLeaf();
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( rose_leaf.release() );

    osg::ref_ptr<osg::Vec4Array> spline_points = new osg::Vec4Array;
    spline_points->push_back(osg::Vec4(0.0,0.0,0,1));
    spline_points->push_back(osg::Vec4(0.8,1.3,0,1));
    spline_points->push_back(osg::Vec4(1,0.0,0,1));
    NaturalCubicSpline spline(spline_points, 3);
        
    LSysPlant plant(3, delta, dist, rot_mat,
                    new RoseFlower(),
                    leaf_list, 0, 5, 144.0,
                    spline, NaturalCubicSpline(),
                    rules, rules['S']);

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

void DynamicKuebelTest(osg::ref_ptr<osg::Group> &root, osgViewer::Viewer &viewer)
{
    osg::ref_ptr<FlowerBucket> flower_bucket = new FlowerBucket();

    osg::ref_ptr<FencePartController> ctrler =
        new FencePartController(root.get(), flower_bucket.get());
    viewer.addEventHandler( ctrler.release() );

    root->addChild( flower_bucket.release() );
}


void KuebelTest(osg::ref_ptr<osg::Group> &root)
{
    osg::ref_ptr<FlowerBucket> flower_bucket = new FlowerBucket();

    root->addChild( flower_bucket );
}

void KuebelTestOld(osg::ref_ptr<osg::Group> &root)
{

    osg::ref_ptr<osg::Node> latte = osgDB::readNodeFile("3d/latte.obj");
    float latte_width = 0.533;
    float latte_height = 1.569;
    float latte_depth = -0.09244;

    unsigned int latten_count_x = 12;
    unsigned int latten_count_y = 3;
    
    // for(int i=4; i < verts->getNumElements(); i++)
    // {
    //     osg::Matrix mat;
    //     osg::Vec3 vec(0.25, 0.25, 0);
    //     mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0)*i, osg::Z_AXIS));
    //     vec = vec * mat;
    //     (*verts)[i] += vec;
    // }

    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();

    verts->push_back(osg::Vec3(0,0,latte_height)); // 0
    verts->push_back(osg::Vec3(latten_count_x * latte_width,0,latte_height)); // 1
    verts->push_back(osg::Vec3(latten_count_x * latte_width, latten_count_y * latte_width,latte_height)); // 2
    verts->push_back(osg::Vec3(0, latten_count_y * latte_width,latte_height)); // 3

    verts->push_back(osg::Vec3(0,0,0)); // 4
    verts->push_back(osg::Vec3(latten_count_x * latte_width,0,0)); // 5
    verts->push_back(osg::Vec3(latten_count_x * latte_width, latten_count_y * latte_width,0)); // 6
    verts->push_back(osg::Vec3(0, latten_count_y * latte_width,0)); // 7

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
    osg::Vec3 normal(0,-1,0);
    osg::ref_ptr<osg::Vec3Array> verts2 = new osg::Vec3Array();

    for(int i=0; i < 3; i++)
    {
        verts2->push_back( (*verts)[i] );
        verts2->push_back( (*verts)[i+1] );
        verts2->push_back( (*verts)[i+5] );
        verts2->push_back( (*verts)[i+4] );

        osg::Matrix mat;
        mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0)*i, osg::Z_AXIS));

        for(int j=0; j<4; j++)
            normals->push_back( normal * mat);
    }

    verts2->push_back((*verts)[3]);
    verts2->push_back((*verts)[0]);
    verts2->push_back((*verts)[4]);
    verts2->push_back((*verts)[7]);
    for(int j=0; j<4; j++)
        normals->push_back( osg::Vec3(-1,0,0));
    
    verts2->push_back((*verts)[4]);
    verts2->push_back((*verts)[5]);
    verts2->push_back((*verts)[6]);
    verts2->push_back((*verts)[7]);
    for(int j=0; j<4; j++)
        normals->push_back( osg::Vec3(0,0,-1));
    
    osg::ref_ptr<osg::Geode> kasten_gd = new osg::Geode;
    osg::ref_ptr<osg::Geometry> kasten_geom = new osg::Geometry;
    kasten_geom->setVertexArray( verts2.get() );
    kasten_geom->setNormalArray( normals.get() );
    kasten_geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    // kasten_geom->setTexCoordArray(0,texc.get());
    // kasten_geom->addPrimitiveSet( face_indices.get() );
    kasten_geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, verts2->getNumElements()) );
    
    kasten_gd->addDrawable( kasten_geom.release() );
    root->addChild( kasten_gd.release() );

    osg::ref_ptr<osg::Vec3Array> normals_innen = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec3Array> verts_innen = new osg::Vec3Array();
    osg::Vec3 offset(0.25,0.25,0);
    osg::Vec3 offset_z(0,0,0.25);

    for(int i=0; i < verts2->getNumElements(); i++)
    {
        osg::Matrix mat;
        mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0)*i, osg::Z_AXIS));        
        verts_innen->push_back( (*verts2)[i] + offset * mat );
        
        // if((*verts2)[i].z() == 0)
        //     (*verts_innen)[i] += offset_z;

        normals_innen->push_back( (*normals)[i] * -1.0 );
    }

    osg::ref_ptr<osg::Geode> kasten_innen_gd = new osg::Geode;
    osg::ref_ptr<osg::Geometry> kasten_innen_geom = new osg::Geometry;
    kasten_innen_geom->setVertexArray( verts_innen.get() );
    kasten_innen_geom->setNormalArray( normals_innen.get() );
    kasten_innen_geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    kasten_innen_geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, verts_innen->getNumElements()) );
    
    kasten_innen_gd->addDrawable( kasten_innen_geom.release() );
    root->addChild( kasten_innen_gd.release() );

    return;

    for(int i=0; i < latten_count_x; i++)
    {
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        osg::Matrix mat;
        mat.postMult(osg::Matrix().translate(latte_width * i, 0, 0));
        trans->setMatrix( mat );
        trans->addChild( latte.get() );
        root->addChild( trans.release() );

        osg::ref_ptr<osg::MatrixTransform> trans2 = new osg::MatrixTransform();
        osg::Matrix mat2;
        mat2.postMult(osg::Matrix().translate(latte_width * i, latte_width*latten_count_y, 0));
        trans2->setMatrix( mat2 );
        trans2->addChild( latte.get() );
        root->addChild( trans2.release() );
    }
     
    for(int i=0; i < latten_count_y; i++)
    {
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        osg::Matrix mat;
        mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));
        mat.postMult(osg::Matrix().translate(0, latte_width * i, 0));        
        trans->setMatrix( mat );
        trans->addChild( latte.get() );
        root->addChild( trans.release() );

        osg::ref_ptr<osg::MatrixTransform> trans2 = new osg::MatrixTransform();
        osg::Matrix mat2;
        mat2.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));
        mat2.postMult(osg::Matrix().translate( latte_width*latten_count_x, latte_width * i, 0));
        trans2->setMatrix( mat2 );
        trans2->addChild( latte.get() );
        root->addChild( trans2.release() );
    }

}

void RundkuebelTest(osg::ref_ptr<osg::Group> &root)
{

    osg::ref_ptr<osg::Node> latte = osgDB::readNodeFile("3d/latte.obj");
    float latte_width = 0.533;

    float radius = 1.0;
    unsigned int latten_count = 12;
    float secant_length = 2*radius * sin(osg::DegreesToRadians((360.0/latten_count)/2.0));

    float scale_x = secant_length / latte_width;

    float PI_step = (2*M_PI)/(float)latten_count;
    float PI_2 =  2.0f*M_PI;

    for(float j=0; j <= (PI_2); j+=PI_step)
    {
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        osg::Matrix mat;
        mat.postMult(osg::Matrix().scale(scale_x, 1.0, 1.0));
        mat.postMult(osg::Matrix().translate(0,radius,0));
        mat.postMult(osg::Matrix().rotate(j, osg::Z_AXIS));

        trans->setMatrix( mat );
        trans->addChild( latte.get() );
        root->addChild( trans.release() );
    }

}

class TrackballAxisDragger : public osgManipulator::CompositeDragger
{
    protected:
    osg::ref_ptr<osgManipulator::TrackballDragger> _rotDragger;
    osg::ref_ptr<osgManipulator::TranslateAxisDragger> _transDragger;
    osg::ref_ptr<osgManipulator::ScaleAxisDragger> _scaleDragger;
    osg::ref_ptr<osg::AutoTransform> _autoTransform;
    osg::ref_ptr<osg::MatrixTransform> _sizeTransform;

  public:
    TrackballAxisDragger()
    {
        _autoTransform = new osg::AutoTransform;
        _autoTransform->setAutoScaleToScreen(true);
        addChild(_autoTransform.get());

        _sizeTransform = new osg::MatrixTransform;
        _sizeTransform->setMatrix(
            osg::Matrix::scale(200, 200, 200));
        _autoTransform->addChild(_sizeTransform.get());

        _rotDragger = new osgManipulator::TrackballDragger;
        _rotDragger->setName("TrackballDragger");
        _sizeTransform->addChild(_rotDragger.get());

        _scaleDragger = new osgManipulator::ScaleAxisDragger;
        _scaleDragger->setName("ScaleAxisDragger");
        _sizeTransform->addChild(_scaleDragger.get());

        _transDragger = new osgManipulator::TranslateAxisDragger;
        _transDragger->setName("TranslateAxisDragger");
        _sizeTransform->addChild(_transDragger.get());


        this->addDragger(_rotDragger.get());
        this->addDragger(_scaleDragger.get());
        this->addDragger(_transDragger.get());
        this->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
        this->setParentDragger(getParentDragger());
    
    }

    void setupDefaultGeometry()
    {
        _rotDragger->setupDefaultGeometry();

        float axesScale = 1.5;
        // _scaleDragger->setMatrix(
        //     osg::Matrix::scale(axesScale,axesScale,axesScale));
        _scaleDragger->setupDefaultGeometry();

        axesScale = 1.5;
        _transDragger->setMatrix(
            osg::Matrix::scale(axesScale,axesScale,axesScale));
        _transDragger->setupDefaultGeometry();
    }

};

void DraggerTest(osg::ref_ptr<osg::Group> &root, FlowerBucket* scene)
{

    scene->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    osg::ref_ptr<osg::MatrixTransform> selection = new osg::MatrixTransform;
    selection->addChild(scene);
    root->addChild( selection );

    osg::ref_ptr<osgManipulator::TabBoxTrackballDragger> dragger =
        new osgManipulator::TabBoxTrackballDragger();
    dragger->setupDefaultGeometry();

    root->addChild(dragger.get());

    float scale = 1.25;
    dragger->setMatrix(osg::Matrix::scale((scene->getWidth())*scale,
                                          (scene->getDepth())*scale,
                                          (scene->getHeight())*scale )
                       * osg::Matrix::translate(scene->getBound().center())
                      );
    dragger->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    dragger->addTransformUpdating(selection.release());
    dragger->setHandleEvents(true);
    dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_ALT);

}

class PickHandler : public osgGA::GUIEventHandler
{
    public:
    PickHandler(osg::Group* root)
        :_root(root), _hOffsetVec(osg::Vec3(0,0,0.01)) {}
    virtual bool handle( const osgGA::GUIEventAdapter& ea,
                         osgGA::GUIActionAdapter& aa );
    
    protected:
    osg::ref_ptr<osg::MatrixTransform> _selectionBox;
    osg::Plane _plane;
    osg::ref_ptr<osg::Group> _pickScene;
    osg::ref_ptr<osg::Camera> _cam;
    osg::ref_ptr<osg::Geometry> _geom;
    osg::ref_ptr<osg::Vec3Array> _verts;
    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::Geometry> _rectGeom;
    osg::ref_ptr<osg::Vec3Array> _rectVerts;
    osg::Vec3 _hOffsetVec;

};

bool PickHandler::handle( const osgGA::GUIEventAdapter& ea,
                          osgGA::GUIActionAdapter& aa )
{

    osgViewer::Viewer* viewer =  
        dynamic_cast<osgViewer::Viewer*>(&aa);
    
    if ( viewer )
    {

        if ( ea.getEventType() == osgGA::GUIEventAdapter::PUSH              &&
             ea.getButton()    == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
             (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
        {
    
            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
                new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

            osgUtil::IntersectionVisitor iv( intersector.get() );
            // iv.setTraversalMask( ~0x1 );
            viewer->getCamera()->accept( iv );

            if(_pickScene.get())
                _pickScene.release();
            _pickScene = new osg::Group();
            
            if ( intersector->containsIntersections() )
            {
                osgUtil::LineSegmentIntersector::Intersection& result =
                    *(intersector->getIntersections().begin());

                osg::Vec3 hit_vec = result.getWorldIntersectPoint();

                if(_rectGeom.get())
                    _rectGeom.release();
                _rectGeom = new osg::Geometry();

                if(_rectVerts.get())
                    _rectVerts.release();
                _rectVerts = new osg::Vec3Array();

                _rectVerts->push_back( hit_vec + _hOffsetVec );
                _rectVerts->push_back( hit_vec + _hOffsetVec );
                _rectVerts->push_back( hit_vec + _hOffsetVec );
                _rectVerts->push_back( hit_vec + _hOffsetVec );

                _rectGeom->setVertexArray( _rectVerts.get() );
                _rectGeom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, _rectVerts->getNumElements()) );

                osg::ref_ptr<osg::Geode> rect_gd = new osg::Geode();
                rect_gd->addDrawable( _rectGeom.get() );
                _root->addChild( rect_gd.get() );
                
                if(_geom.get())
                    _geom.release();
                _geom = new osg::Geometry();

                if(_verts.get())
                    _verts.release();
                _verts = new osg::Vec3Array();

                float expansion = 10.0;
                osg::Vec3 expansion_vec( expansion, expansion, 0 );
                osg::Matrix mat90;      
                mat90.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));

                for(int i=0; i<4; i++)
                {
                    _verts->push_back( hit_vec + expansion_vec );
                    expansion_vec = expansion_vec * mat90;
                }

                
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );
                colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );
                colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );
                colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );

                _geom->setVertexArray( _verts.get() );
                _geom->setColorArray( colors.get() );
                _geom->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
                _geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, _verts->getNumElements()) );

                _geom->setName("PlaneGeom");

                osg::ref_ptr<osg::Geode> gd = new osg::Geode();
                gd->addDrawable( _geom.get() );

                osg::StateSet* state = gd->getOrCreateStateSet();
                state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
                state->setMode(GL_BLEND, osg::StateAttribute::ON);

                osg::BlendFunc* blend = new osg::BlendFunc;
                blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);

                _root->addChild( gd.get() );
            }
        }
        
        if ( ea.getEventType() == osgGA::GUIEventAdapter::DRAG  &&
             // ea.getButton()    == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
             (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
        {

            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
                new osgUtil::LineSegmentIntersector(
                    osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

            osgUtil::IntersectionVisitor iv( intersector.get() );
            viewer->getCamera()->accept( iv );

            if ( intersector->containsIntersections() )
            {
                osgUtil::LineSegmentIntersector::Intersections intersections =
                    intersector->getIntersections();

                osgUtil::LineSegmentIntersector::Intersections::iterator itr =
                    intersections.begin();

                for(; itr != intersections.end(); itr++)
                {
                    if(itr->drawable == _geom)
                    {
                        osgUtil::LineSegmentIntersector::Intersection& result = *(itr);
                
                        osg::Vec3 hit_vec = result.getWorldIntersectPoint();

                        osg::Vec3 origin_vec = (*_rectVerts)[0];
                        (*_rectVerts)[2] = hit_vec; // andere Ecke
                        (*_rectVerts)[1] = osg::Vec3(origin_vec.x(), hit_vec.y(), origin_vec.z());
                        (*_rectVerts)[3] = osg::Vec3(hit_vec.x(), origin_vec.y(), origin_vec.z());

                        _rectGeom->dirtyDisplayList();
                        _rectGeom->dirtyBound();

                        break;
                    }
                }
            }
                    
            return true;
        }

        // if ( ea.getEventType() == osgGA::GUIEventAdapter::RELEASE           &&
        //      ea.getButton()    == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
        //      (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL) )
        // {
        //     osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
        //         new osgUtil::LineSegmentIntersector(
        //             osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

        //     osgUtil::IntersectionVisitor iv( intersector.get() );
        //     viewer->getCamera()->accept( iv );

        //     // // iv.setTraversalMask( ~0x1 );
        //     // // _cam->accept( iv );
        //     // _pickScene->accept( iv );

        //     if ( intersector->containsIntersections() )
        //     {
        //         osgUtil::LineSegmentIntersector::Intersection& result =
        //             *(intersector->getIntersections().begin());

        //         cout << result.drawable->getName() << endl;

        //         osg::Vec3 hit_vec = result.getWorldIntersectPoint();
        //         cout << hit_vec.x() << " " << hit_vec.y() << " " << hit_vec.z() << endl;
        //     }

        // }
    }
    return false;
}

void PickTest(osg::ref_ptr<osg::Group> &root, osgViewer::Viewer &viewer)
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(1.0f, 1.0f, 0.0f) );
    vertices->push_back( osg::Vec3(-1.0f, 1.0f, 0.0f) );
    vertices->push_back( osg::Vec3(-1.0f, -1.0f, 0.0f) );
    vertices->push_back( osg::Vec3(1.0f, -1.0f, 1.0f) );

    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, vertices->getNumElements()) );
    
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( quad.get() );

    root->addChild( gd.release() );

    vector< osg::ref_ptr<osg::Geometry> > groundGeoms;
    groundGeoms.push_back( quad );

    osg::ref_ptr<FlowerBucketController> picker =
        new FlowerBucketController(root.get(), groundGeoms);

    

    viewer.addEventHandler( picker.get() );
}

void PointTest(osg::ref_ptr<osg::Group> &root, osgViewer::Viewer &viewer)
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,-1.0f,0.0f) );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f) );
    colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
    colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
    colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->setNormalArray( normals.get() );
    // siehe BIND_OVERALL weiter oben
    quad->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    quad->setColorArray( colors.get() );
    quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    quad->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, vertices->getNumElements()) );
    
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( quad.get() );
       
    osg::StateSet* set = gd->getOrCreateStateSet();
    set->setMode(GL_BLEND, osg::StateAttribute::ON);
    osg::BlendFunc *fn = new osg::BlendFunc();
    fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
    set->setAttributeAndModes(fn, osg::StateAttribute::ON);

    /// Setup the point sprites
    osg::PointSprite *sprite = new osg::PointSprite();
    set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);

    /// Give some size to the points to be able to see the sprite
    osg::Point *point = new osg::Point();
    point->setSize(5);
    set->setAttribute(point);

    /// Disable depth test to avoid sort problems and Lighting
    set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    root->addChild( gd.release() );

}

int main( int argc, char** argv)
{

    osg::ref_ptr<osg::Group> root = new osg::Group;
    osgViewer::Viewer viewer;

    osg::ref_ptr<osg::Vec3Array> origin_verts = new osg::Vec3Array();
    origin_verts->push_back( osg::Vec3(1,0,0) );
    origin_verts->push_back( osg::Vec3(-1,0,0) );
    origin_verts->push_back( osg::Vec3(0,1,0) );
    origin_verts->push_back( osg::Vec3(0,-1,0) );
    origin_verts->push_back( osg::Vec3(0,0,1) );
    origin_verts->push_back( osg::Vec3(0,0,-1) );
    osg::ref_ptr<osg::Geometry>  origin_geom  = new osg::Geometry();
    origin_geom->setVertexArray( origin_verts.get() );
    origin_geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, origin_verts->getNumElements()) );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( origin_geom );
    root->addChild( gd );

    // extrudeSplineAlongSpline( root );
    // LeafTest( root );
    // FlowerTest( root );
    // DynamicTest( root );
    // PlantStringTest( root );
    // KuebelTestOld( root );
    // DynamicKuebelTest( root, viewer );
    KuebelTest( root );
    // DrawLineTest(root, viewer);
    PickTest( root, viewer );

    // osg::ref_ptr<FlowerBucket> fb = new FlowerBucket();
    // DraggerTest( root, fb.get() );

    viewer.setSceneData( root );

    return viewer.run();
}

