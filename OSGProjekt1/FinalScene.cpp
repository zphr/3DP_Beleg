#include "FinalScene.h"

FinalScene::FinalScene()
:_cullMask(0x99),
_rcvShadowMask(0x1),
_castShadowMask(0x2),
_intersectMask(0x16)
{
    _root = new osgShadow::ShadowedScene;
    _root->setReceivesShadowTraversalMask( _rcvShadowMask );
    _root->setCastsShadowTraversalMask( _castShadowMask );

    _viewer.getCamera()->setCullMask( ~_cullMask );

    setupBackground();
    setupRose();
    setupLight();
    
    _viewer.setSceneData( _root );
}

FinalScene::~FinalScene()
{
    
}

void FinalScene::setupLight()
{

    ColorGradient colorGradient;
    // colorGradient.addColorMarkerAt(0.0, osg::Vec4(0.535, 0.330, 0.270, 1));
    colorGradient.addColorMarkerAt(0.0, osg::Vec4(0.13375, 0.0825, 0.0675, 1));
    colorGradient.addColorMarkerAt(0.09932, osg::Vec4(0.793, 0.487, 0.404, 1));
    colorGradient.addColorMarkerAt(0.315, osg::Vec4(0.354, 0.353, 0.450, 1));
    colorGradient.addColorMarkerAt(0.432, osg::Vec4(0.374, 0.373, 0.468, 1));
    colorGradient.addColorMarkerAt(1.0, osg::Vec4(0.785, 0.844, 1.000, 1));

    _sun = new Sun(40.0, colorGradient, &_viewer);

    // _root->setShadowTechnique( _sun->getShadowMap() );
    _root->addChild( _sun.get() );

    osg::ref_ptr<SunController> sun_ctrl = new SunController(_sun.get());
    _viewer.addEventHandler( sun_ctrl.release() );
}

void FinalScene::setupController()
{

}

void FinalScene::setupBackground()
{
    
    // ---------------------------------------- Wand-Modell

    osg::ref_ptr<osg::Node> wall_model  = osgDB::readNodeFile("3d/Wand.obj");

    osg::ref_ptr<osg::Texture2D> wall_tex = new osg::Texture2D;
    wall_tex->setImage( osgDB::readImageFile("3d/Wand_compl.png") );
    osg::StateSet* wall_state = wall_model->getOrCreateStateSet();
    wall_state->setTextureAttributeAndModes(0,wall_tex,osg::StateAttribute::ON);
    wall_model->setNodeMask( _rcvShadowMask | _castShadowMask );

    // ---------------------------------------- Rasen-Modell

    osg::ref_ptr<osg::Node> rasen_model = osgDB::readNodeFile("3d/Rasen.obj");
    osg::ref_ptr<osg::Texture2D> rasen_tex = new osg::Texture2D;
    rasen_tex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    rasen_tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    rasen_tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    rasen_tex->setImage( osgDB::readImageFile("3d/Rasen.png") );
    osg::StateSet* rasen_state = rasen_model->getOrCreateStateSet();
    rasen_state->setTextureAttributeAndModes(0,rasen_tex,osg::StateAttribute::ON);
    rasen_model->setNodeMask( _rcvShadowMask | _castShadowMask );

    // ---------------------------------------- Pick Fläche

    float bound_radius = rasen_model->getBound().radius();
    osg::Vec3 radius_vec(bound_radius, 0, 0);
    float gegenkathete = sin(osg::DegreesToRadians(45.0)) * radius_vec.length();

    osg::ref_ptr<osg::Vec3Array> ground_vertices = new osg::Vec3Array;
    ground_vertices->push_back( osg::Vec3(-gegenkathete,0,0) );
    ground_vertices->push_back( osg::Vec3(gegenkathete, 0, 0) );
    ground_vertices->push_back( osg::Vec3(gegenkathete, -gegenkathete*2, 0) );
    ground_vertices->push_back( osg::Vec3(-gegenkathete, -gegenkathete*2, 0) );

    osg::ref_ptr<osg::Geometry> ground_geom = new osg::Geometry;
    ground_geom->setVertexArray( ground_vertices.get() );
    ground_geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, ground_vertices->getNumElements()) );
    osg::ref_ptr<osg::Geode> ground_gd = new osg::Geode;
    ground_gd->addDrawable( ground_geom.get() );
    ground_gd->setNodeMask( _cullMask );

    vector< osg::ref_ptr<osg::Geometry> > ground_geoms;
    ground_geoms.push_back( ground_geom.release() );

    osg::ref_ptr<FlowerBucketController> flower_bucket_ctrl =
        new FlowerBucketController(_root.get(), ground_geoms,
                                   _rcvShadowMask | _castShadowMask | _intersectMask );
    _viewer.addEventHandler( flower_bucket_ctrl.release() );

    // erster Blumenkasten
    osg::ref_ptr<FlowerBucket> flower_bucket = new FlowerBucket();
    _root->addChild( flower_bucket.release() );

    osg::ref_ptr<FencePartController> fence_part_ctrl =
        new FencePartController(_root.get(), flower_bucket.get());
    _viewer.addEventHandler( fence_part_ctrl.release() );
    
    // ---------------------------------------- Hauptknoten

    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->addChild( wall_model.release() );
    trans->addChild( rasen_model.release() );
    trans->addChild( ground_gd.release() );

    float scale = 1.5;
    trans->setMatrix( osg::Matrix().scale(scale, scale, scale) *
                      osg::Matrix().translate(0,10,0) );

    _root->addChild( trans.release() );
}

void FinalScene::setupRose()
{
    float delta = 22.5;
    osg::Vec4 dist (0.0, 0.0, 1.0, 1.0);
    osg::Matrix rot_mat;

    map<char, string> rules;
    rules['S'] = "[A(x)]+(66)[A(1.0)]+(66)[A(1.0)]+(66)[A(1.0)]F@";
    rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)[S]";

    osg::ref_ptr<RoseLeaf> rose_leaf = new RoseLeaf();
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( rose_leaf.release() );

    osg::ref_ptr<osg::Vec4Array> spline_points = new osg::Vec4Array;
    spline_points->push_back(osg::Vec4(0.0,0.0,0,1));
    spline_points->push_back(osg::Vec4(0.8,1.3,0,1));
    spline_points->push_back(osg::Vec4(1,0.0,0,1));
    NaturalCubicSpline spline(spline_points, 3);

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0, 1.0, 0, 1));
    profile_points->push_back(osg::Vec4(1, 0.5, 0, 1));
    NaturalCubicSpline profile(profile_points, 3);

    LSysPlant plant(3, delta, dist, rot_mat, 0.04, 0.70,
                    30, 7,      // Jitter Prozente
                    "3d/Stengel.png", profile,
                    0.75, 0.90, new RoseFlower(),
                    leaf_list, 0, 5, 144.0, 1.0, 0.85,
                    spline, NaturalCubicSpline(),
                    rules, rules['S']);

    osg::ref_ptr<osg::Group> plant_group = plant.buildPlant();
    plant_group->setNodeMask( _intersectMask ); 

    _root->addChild( plant_group.release() );

}

void FinalScene::run()
{
    _viewer.run();
}

