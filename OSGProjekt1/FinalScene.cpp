#include "FinalScene.h"

bool FinalScene::_renderShadows = false;

FinalScene::FinalScene()
:_cullMask(0x99),
_rcvShadowMask(0x1),
_castShadowMask(0x2),
_intersectMask(0x16)
{
    _root = new osgShadow::ShadowedScene;
    _root->setReceivesShadowTraversalMask( 0x0 );
    _root->setCastsShadowTraversalMask( 0x0 );

    _viewer.getCamera()->setCullMask( ~_cullMask );

    setupModels();
    setupLight();

    osg::ref_ptr<FinalController> final_ctrl = new FinalController( this );
    _viewer.addEventHandler( final_ctrl.release() );
    
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

    _sun = new Sun(40.0, colorGradient, &_viewer, 512);

    _root->setShadowTechnique( _sun->getShadowMap() );
    _root->addChild( _sun.get() );

    osg::ref_ptr<SunController> sun_ctrl = new SunController(_sun.get());
    _viewer.addEventHandler( sun_ctrl.release() );
}

void FinalScene::setupController()
{

}

void FinalScene::setupModels()
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

    float height_offset = 0.1;

    osg::ref_ptr<osg::Vec3Array> ground_vertices = new osg::Vec3Array;
    ground_vertices->push_back( osg::Vec3(-gegenkathete,               0, height_offset) );
    ground_vertices->push_back( osg::Vec3( gegenkathete,               0, height_offset) );
    ground_vertices->push_back( osg::Vec3( gegenkathete, -gegenkathete*2, height_offset) );
    ground_vertices->push_back( osg::Vec3(-gegenkathete, -gegenkathete*2, height_offset) );

    osg::ref_ptr<osg::Geometry> ground_geom = new osg::Geometry;
    ground_geom->setVertexArray( ground_vertices.get() );
    ground_geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, ground_vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> ground_gd = new osg::Geode;
    ground_gd->addDrawable( ground_geom.get() );
    ground_gd->setNodeMask( _cullMask );

    vector< osg::ref_ptr<osg::Geometry> > ground_geoms;
    ground_geoms.push_back( ground_geom.release() );

    // ---------------------------------------- Controller

    osg::ref_ptr<FlowerBucketController> flower_bucket_ctrl =
        new FlowerBucketController(_root.get(), ground_geoms,
                                   _rcvShadowMask | _castShadowMask | _intersectMask );
    _viewer.addEventHandler( flower_bucket_ctrl.release() );

    // erster Blumenkasten
    osg::ref_ptr<FlowerBucket> flower_bucket = new FlowerBucket();

    osg::ref_ptr<FencePartController> fence_part_ctrl =
        new FencePartController(_root.get(),
                                _rcvShadowMask | _castShadowMask | _intersectMask,
                                flower_bucket.get());
    _viewer.addEventHandler( fence_part_ctrl.release() );
    

    osg::ref_ptr<PlantController> plant_ctrl =
        new PlantController(_root.get(), _intersectMask);
    _viewer.addEventHandler( plant_ctrl.release() );

    // ---------------------------------------- Hauptknoten

    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->addChild( wall_model.release() );
    trans->addChild( rasen_model.release() );
    trans->addChild( ground_gd.release() );

    float scale = 1.5;
    trans->setMatrix( osg::Matrix().scale(scale, scale, scale) *
                      osg::Matrix().translate(0,10,0) );

    _root->addChild( trans.release() );

    // ---------------------------------------- Rose

    RosePlant rose( _intersectMask );

    osg::ref_ptr<osg::MatrixTransform> rose_trans = new osg::MatrixTransform();
    rose_trans->addChild( rose.releasePlant() );
    rose_trans->setMatrix( osg::Matrix().translate(2.5,0.5,1.1) );

    // ---------------------------------------- Dragger

    osg::ref_ptr<osg::MatrixTransform> selection = new osg::MatrixTransform;
    selection->addChild( flower_bucket.get() );
    selection->addChild( rose_trans.release() );

    _root->addChild( selection.get() );

    osg::ref_ptr<TrackballAxisDragger> dragger =
        new TrackballAxisDragger( flower_bucket->getBound().center(),
                                  flower_bucket->getBound().radius() );
    dragger->setupDefaultGeometry();

    _root->addChild( dragger.get() );

    dragger->addTransformUpdating( selection.release() );
    dragger->setHandleEvents(true);
    dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_ALT);
    dragger->setNodeMask( ~_castShadowMask );
}

void FinalScene::run()
{
    _viewer.run();
}


void FinalScene::toggleShadows()
{
    FinalScene::_renderShadows = !_renderShadows;

    if(FinalScene::_renderShadows)
    {
        _root->setReceivesShadowTraversalMask( _rcvShadowMask );
        _root->setCastsShadowTraversalMask( _castShadowMask );
    }
    else
    {
        _root->setReceivesShadowTraversalMask( 0x0 );
        _root->setCastsShadowTraversalMask( 0x0 );
        _root->cleanSceneGraph();
    }
}


