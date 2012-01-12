#include "FlowerGroup.h"

FlowerGroup::FlowerGroup(
    osg::ref_ptr<osg::Node> body,
    osg::ref_ptr<osg::Node> stamen,      /* stamen -> Staubblätter */
    int stamenCount,
    float stamenStartRadius,
    float stamenEndRadius,
    vector<osg::ref_ptr<LeafGeode>> petalGeodes,
    float petalStartRadius,
    float petalEndRadius,
    int petalCount)
{
    _body = body;
    
    _stamen            = stamen;
    _stamenCount       = stamenCount;
    _stamenStartRadius = stamenStartRadius;
    _stamenEndRadius   = stamenEndRadius;

    _petalGeodes      = petalGeodes;
    _petalStartRadius = petalStartRadius;
    _petalEndRadius   = petalEndRadius;
    _petalCount       = petalCount;

    buildFlower();
}

FlowerGroup::FlowerGroup(
    string body_file,
    string stamen_file,
    int stamenCount,
    float stamenStartRadius,
    float stamenEndRadius,
    vector<osg::ref_ptr<LeafGeode>> petalGeodes,
    float petalStartRadius,
    float petalEndRadius,
    int petalCount)
{
    
    if(body_file != "")
        _body = osgDB::readNodeFile(body_file);
    
    if(stamen_file != "")
        _stamen = osgDB::readNodeFile(stamen_file);
    
    _stamenCount       = stamenCount;
    _stamenStartRadius = stamenStartRadius;
    _stamenEndRadius   = stamenEndRadius;

    _petalGeodes      = petalGeodes;
    _petalStartRadius = petalStartRadius;
    _petalEndRadius   = petalEndRadius;
    _petalCount       = petalCount;

    buildFlower();
}

FlowerGroup::~FlowerGroup()
{

}

void FlowerGroup::buildFlower()
{
    float alpha;
    float distributionAngle = 360.0 / _petalCount;
    distributionAngle = osg::DegreesToRadians(137.2);
    float r=_petalStartRadius;
    
    getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    // getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    
    addChild(_stamen);
    addChild(_body);
    
    osg::ref_ptr<osgAnimation::BasicAnimationManager> manager =
        new osgAnimation::BasicAnimationManager;
    
    for(int i=1;
        r <= _petalEndRadius;
        i++, r = _petalStartRadius * sqrt((float)i))
    {
        ostringstream s;
        s << i;
        
        osg::ref_ptr<osgAnimation::MatrixLinearChannel> ch1 =
            new osgAnimation::MatrixLinearChannel();
        ch1->setName("matrix" + s.str());
        ch1->setTargetName("MatrixCallback" + s.str());

        calcAnimation(i, 6.0, 8, ch1->getOrCreateSampler()->getOrCreateKeyframeContainer());
        
        osg::ref_ptr<osgAnimation::Animation> animation = new 
            osgAnimation::Animation;
        animation->setPlayMode( osgAnimation::Animation::PPONG );
        animation->addChannel( ch1.get() );
        
        osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
            new osgAnimation::UpdateMatrixTransform("MatrixCallback"+s.str());
        updater->getStackedTransforms().push_back(
            new osgAnimation::StackedMatrixElement("matrix"+s.str(), osg::Matrix()) );

        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
        
        trans->addChild(_petalGeodes[0]);
        trans->setDataVariance( osg::Object::DYNAMIC );
        trans->setUpdateCallback( updater.get() );
        
        manager->registerAnimation( animation.get() );
        insertChild(0, trans);
        manager->playAnimation( animation.get() );

        updater.release();
        animation.release();
        ch1.release();
        trans.release();
    }
    setUpdateCallback( manager.get() );
}


void FlowerGroup::calcAnimation(unsigned int index,
                                float time,
                                unsigned int samples,
                                osgAnimation::MatrixKeyframeContainer* matContainer)
{
    float delta_rot_y = (osg::DegreesToRadians(10.0) * sqrt((float)index)) / (float)samples;
    float delta_rot_z = osg::DegreesToRadians(137.2);
    float delta_time = time / (float) samples;
    
    float r=_petalStartRadius;
    
    for(int i=0; i<samples; ++i)
    {
        osg::Quat rot_y(delta_rot_y * (float)i, osg::Y_AXIS);
        
        osg::Matrix mat;
        
        // mat.preMult(osg::Matrix::scale(osg::Vec3(1.0/sqrt((float)index),1,1)));
        // mat.preMult(osg::Matrix::rotate(delta_rot_y * i, 0,1,0));
        // mat.postMult(osg::Matrix::translate(r/sqrt((float)index),0,1));
        // mat.postMult(osg::Matrix::rotate(delta_rot_z * index, 0,0,1));
        // mat.postMult(osg::Matrix::scale(osg::Vec3(.4,.4,.4)));
        
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
    
}
