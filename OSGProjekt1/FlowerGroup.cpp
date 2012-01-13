#include "FlowerGroup.h"

FlowerGroup::FlowerGroup()
{
    _manager = new osgAnimation::BasicAnimationManager();
}

FlowerGroup::FlowerGroup(
    osg::ref_ptr<osg::Node> body,
    osg::ref_ptr<osg::Node> inside,
    vector<osg::ref_ptr<LeafGeode>> petalGeodes,
    float petalStartRadius,
    float petalEndRadius,
    float distributionAngle,
    float time,
    unsigned int samples) 
{
    _body   = body.get();
    _inside = inside.get();
    
    _petalGeodes      = petalGeodes;
    _petalStartRadius = petalStartRadius;
    _petalEndRadius   = petalEndRadius;
    
    _distributionAngle = distributionAngle;
    _samples           = samples;
    _time              = time;

    _manager = new osgAnimation::BasicAnimationManager();
    
    buildFlower();
}

FlowerGroup::FlowerGroup(
    string bodyFile,
    string insideFile,
    vector<osg::ref_ptr<LeafGeode>> petalGeodes,
    float petalStartRadius,
    float petalEndRadius,
    float distributionAngle,
    float time,
    unsigned int samples) 
{
    
    if(bodyFile != "")
        _body = osgDB::readNodeFile(bodyFile);
    
    if(insideFile != "")
        _inside = osgDB::readNodeFile(insideFile);
    
    _petalGeodes      = petalGeodes;
    _petalStartRadius = petalStartRadius;
    _petalEndRadius   = petalEndRadius;

    _distributionAngle = distributionAngle;
    _samples = samples;
    _time = time;
    
    _manager = new osgAnimation::BasicAnimationManager();
    
    buildFlower();
}

FlowerGroup::~FlowerGroup()
{

}

void FlowerGroup::buildFlower()
{
    getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    // getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    
    addChild(_inside);
    addChild(_body);
    
    float r = _petalStartRadius;
    
    for(int i=1;
        r <= _petalEndRadius;
        i++, r = _petalStartRadius * sqrt((float)i))
    {

        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
        
        trans->addChild(_petalGeodes[0]);
        trans->setDataVariance( osg::Object::DYNAMIC );
        
        calcAnimation(i, trans.get());
        
        insertChild(0, trans.release());
    }
    setUpdateCallback( _manager.get() );
}

void FlowerGroup::calcAnimation(unsigned int index,
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
