#include "FinalScene.h"

FinalScene::FinalScene()
{
    _cullMask = 0x99;
    _rcvShadowMask = 0x1;
    _castShadowMask = 0x2;
    
    osg::ref_ptr<osgShadow::ShadowedScene> _root = 
        new osgShadow::ShadowedScene;
    root->setReceivesShadowTraversalMask( _rcvShadowMask );
    root->setCastsShadowTraversalMask( _castShadowMask );

}

FinalScene::~FinalScene()
{

}

void FinalScene::setupLight()
{
    
}

void FinalScene::setupShadows()
{
    _shadowSrc = new osg::LightSource;
    _shadowSrc->getLight()->setPosition( osg::Vec4(4.0, -10.0, 10.0, 0.0) );
    _shadowSrc->getLight()->setAmbient( osg::Vec4(0.2, 0.2, 0.2, 1.0));
    _shadowSrc->getLight()->setDiffuse( osg::Vec4(0.8, 0.8, 0.8, 1.0));

    osg::ref_ptr<osgShadow::ShadowMap> _shadowMap = new osgShadow::ShadowMap;
    _shadowMap->setLight( _shadowSrc.get() );
    _shadowMap->setTextureSize( osg::Vec2s(1024, 1024) );
    _shadowMap->setTextureUnit( 1 );

    _root->setShadowTechnique( _shadowMap.get() );
}

void FinalScene::setupController()
{

}

void FinalScene::setupBackground()
{
    
    // ---------------------------------------- Wand-Modell
    osg::ref_ptr<osg::Node> wallModel  = osgDB::readNodeFile("3d/Wand.obj");

    osg::ref_ptr<osg::Texture2D> wallTex = new osg::Texture2D;
    wallTex->setImage( osgDB::readImageFile("3d/Wand_compl.png") );
    osg::StateSet* wallState = wallModel->getOrCreateStateSet();
    wallState->setTextureAttributeAndModes(0,wallTex,osg::StateAttribute::ON);
    wallModel->setNodeMask( rcvShadowMask | castShadowMask );

    // ---------------------------------------- Rasen-Modell
    osg::ref_ptr<osg::Node> rasenModel = osgDB::readNodeFile("3d/Rasen.obj");
    osg::ref_ptr<osg::Texture2D> rasenTex = new osg::Texture2D;
    rasenTex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    rasenTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    rasenTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    rasenTex->setImage( osgDB::readImageFile("3d/Rasen.png") );
    osg::StateSet* rasenState = rasenModel->getOrCreateStateSet();
    rasenState->setTextureAttributeAndModes(0,rasenTex,osg::StateAttribute::ON);
    rasenModel->setNodeMask( rcvShadowMask | castShadowMask );

}





