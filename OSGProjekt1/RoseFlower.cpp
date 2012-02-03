#include "RoseFlower.h"

const osg::ref_ptr<osg::Image> RoseFlower::_leafImg =
    osgDB::readImageFile("bluete1.png");

const osg::ref_ptr<osg::Image> RoseFlower::_bodyImg =
    osgDB::readImageFile("3d/bluetenbasis.png");

const osg::ref_ptr<osg::Node>  RoseFlower::_bodyModel =
    osgDB::readNodeFile("3d/Bluete.obj");

const osg::ref_ptr<osg::Node> RoseFlower::_insideModel =
    osgDB::readNodeFile("3d/Bluete_innen.obj");

const osg::ref_ptr<osg::Image> RoseFlower::_insideImg =
    osgDB::readImageFile("bluete1.png");

RoseFlower::RoseFlower()
{
    // ---------------------------------------- Blütenblätter
    // -------------------- Profil Spline
    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(-0.75,  -1,0,1));
    profile_points->push_back(osg::Vec4(-0.15,-0.5,0,1));
    profile_points->push_back(osg::Vec4(   0,   0,0,1));
    profile_points->push_back(osg::Vec4(-0.15, 0.5,0,1));
    profile_points->push_back(osg::Vec4(-0.75,   1,0,1));
    
    NaturalCubicSpline profile_spline(profile_points, 1);

    // -------------------- Extrudier Spline
    osg::ref_ptr<osg::Vec4Array> leaf_points = new osg::Vec4Array;
    leaf_points->push_back(osg::Vec4(0,0,0,1));
    leaf_points->push_back(osg::Vec4(0.3,0,0.55,1));
    leaf_points->push_back(osg::Vec4(0,0,1,1));
    
    NaturalCubicSpline leaf_spline(leaf_points,
                                   2,
                                   new NaturalCubicSpline(profile_points, 1));
    
    // -------------------- Blatt-Objekt
    osg::ref_ptr<LeafGeode> leaf = new LeafGeode(leaf_spline, 2, 0.5,
                                                 RoseFlower::_leafImg.get());
    _petalGeodes.push_back( leaf );
    
    // ---------------------------------------- Körper Mesh
    _body = RoseFlower::_bodyModel.get();

    osg::ref_ptr<osg::Texture2D> body_texture = new osg::Texture2D;
    body_texture->setImage(RoseFlower::_bodyImg.get());

    osg::StateSet* body_state = _body->getOrCreateStateSet();
    body_state->setTextureAttributeAndModes(0,body_texture,osg::StateAttribute::ON);
    body_state->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::BlendFunc* body_blend = new osg::BlendFunc;
    body_blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        
    body_state->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    osg::AlphaFunc* body_alphaFunc = new osg::AlphaFunc;
    body_alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.6f);
    body_state->setAttributeAndModes( body_alphaFunc, osg::StateAttribute::ON );

    // ---------------------------------------- Inside Mesh
    _inside = RoseFlower::_insideModel.get();
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(RoseFlower::_insideImg.get());

    osg::StateSet* insige_state = _inside->getOrCreateStateSet();
    insige_state->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    
    _petalStartRadius  = 0.2;
    _petalEndRadius    = 1.0;
    _distributionAngle = 137.2;
    _time = 6.0;
    _samples = 8;

    // ---------------------------------------- Material

    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.8, 0.8, 1.0) );
    material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.2, 0.2, 1.0) );
    material->setShininess( osg::Material::FRONT_AND_BACK, 1.0 );
    material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.2, 0.2, 1.0) );
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    osg::StateSet* state = this->getOrCreateStateSet();
    state->setAttribute( material.release(),
                         osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );


    buildFlower(false);
}

RoseFlower::~RoseFlower()
{

}

void RoseFlower::calcAnimation(unsigned int index,
                                osg::MatrixTransform* trans)
{
    float delta_rot_y = (osg::DegreesToRadians(10.0) * sqrt((float)index)) / (float)_samples;
    float delta_rot_z = osg::DegreesToRadians(_distributionAngle);
    float delta_time = _time / (float) _samples;
    
    float r=_petalStartRadius;
    
    ostringstream s;
    s << index;
        
    osg::ref_ptr<osgAnimation::MatrixLinearChannel> channel =
        new osgAnimation::MatrixLinearChannel();
    channel->setName("matrix" + s.str());
    channel->setTargetName("MatrixCallback" + s.str());
    
    osg::ref_ptr<osgAnimation::MatrixKeyframeContainer> matContainer = 
        channel->getOrCreateSampler()->getOrCreateKeyframeContainer();

    // Höhenverschiebung der Blätter
    float h_offset = 0.275;

    for(int i=0; i<_samples; ++i)
    {
        osg::Quat rot_y(delta_rot_y * (float)i, osg::Y_AXIS);
        
        osg::Matrix mat;
        
        mat.postMult(osg::Matrix::scale(
                         osg::Vec3(sqrt(((float)index)) / sqrt(((float)index+1)),
                                   sqrt(((float)index)) / sqrt(((float)index+1)),
                                   sqrt(((float)index)) / sqrt(((float)index+1))
                                  )));
        
        // mat.postMult(osg::Matrix::scale(osg::Vec3(1.0/sqrt((float)index),1,1)));
        mat.postMult(osg::Matrix::rotate(delta_rot_y * i, 0,1,0));
        mat.postMult(osg::Matrix::translate(0.006*index + r/sqrt((float)index),0,h_offset));
        mat.postMult(osg::Matrix::rotate(delta_rot_z * index, 0,0,1));
        mat.postMult(osg::Matrix::scale(osg::Vec3(.4,.4,.45)));
        
        matContainer->push_back(
            osgAnimation::MatrixKeyframe(delta_time * (float)i, mat));
    }
    
    osg::ref_ptr<osgAnimation::Animation> animation = new 
        osgAnimation::Animation;
    animation->setPlayMode( osgAnimation::Animation::PPONG );
    animation->addChannel( channel.get() );
    
    _manager->registerAnimation( animation.get() );
    _manager->playAnimation( animation.get() );
        
    osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
        new osgAnimation::UpdateMatrixTransform("MatrixCallback"+s.str());
    updater->getStackedTransforms().push_back(
        new osgAnimation::StackedMatrixElement("matrix"+s.str(), osg::Matrix()) );
    
    trans->setUpdateCallback( updater.get() );
        
}

osg::Matrix RoseFlower::animationStep(unsigned int index, float percent)
{
    float delta_rot_y = (osg::DegreesToRadians(10.0) * sqrt((float)index));
    float delta_time = _time / (float) _samples;

    // Höhenverschiebung der Blätter
    float h_offset = 0.275;

    osg::Matrix mat;
    mat.postMult(osg::Matrix::scale(
                     osg::Vec3(sqrt((float)index) / sqrt((float)index+1),
                               sqrt((float)index) / sqrt((float)index+1),
                               sqrt((float)index) / sqrt((float)index+1) )));

    mat.postMult(osg::Matrix::rotate(delta_rot_y * percent, 0,1,0));
    mat.postMult(osg::Matrix::translate(0.006*index + _petalStartRadius/sqrt((float)index),
                                        0, h_offset));
    mat.postMult(osg::Matrix::rotate(osg::DegreesToRadians(_distributionAngle) * index, 0,0,1));
    mat.postMult(osg::Matrix::scale(osg::Vec3(.4,.4,.45)));

    return mat;

}
