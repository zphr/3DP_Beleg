#include "RoseFlower.h"

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
                                   12,
                                   new NaturalCubicSpline(profile_points, 1));
    
    // -------------------- Blatt-Objekt
    osg::ref_ptr<LeafGeode> leaf = new LeafGeode(leaf_spline, 3, 0.5, "bluete1.png");
    _petalGeodes.push_back( leaf );
    
    // ---------------------------------------- Körper Mesh
    _body = osgDB::readNodeFile("3d/Bluete.obj");
    
    // ---------------------------------------- Inside Mesh
    _inside = osgDB::readNodeFile("3d/Bluete_innen.obj");
    
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("bluete1.png");
    texture->setImage(image.get());

    osg::StateSet* state = _inside->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
    
    // ---------------------------------------- Blüte
    
    // // -------------------- Blend
    // state->setMode(GL_BLEND, osg::StateAttribute::ON);
    // osg::BlendFunc* blend = new osg::BlendFunc;
    // blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        
    // // -------------------- Alpha
    // state->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    // osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
    // alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.6f);
    // state->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );
    
    _petalStartRadius  = 0.2;
    _petalEndRadius    = 1.0;
    _distributionAngle = 137.2;
    _time = 6.0;
    _samples = 8;
    
    buildFlower();
    
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
        mat.postMult(osg::Matrix::translate(0.006*index + r/sqrt((float)index),0,0.75));
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
