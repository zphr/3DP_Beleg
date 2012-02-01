#include "PetalTransform.h"

PetalTransform::PetalTransform(unsigned int index,
                               float startRadius,
                               float distributionAngle)
    : _index(index),
   _startRadius(startRadius),
   _distributionAngle(distributionAngle)
{
    animationStep(0.0001);
    setUpdateCallback(new PetalTransformCallback);
}

PetalTransform::~PetalTransform()
{

}


void PetalTransform::animationStep(float percent)
{
    if(percent <= 0)
        return;

    float delta_rot_y = (osg::DegreesToRadians(10.0) * sqrt((float)_index));

    // Höhenverschiebung der Blätter
    float h_offset = 0.275;

    osg::Matrix mat;
    mat.postMult(osg::Matrix::scale(
                         osg::Vec3(sqrt((float)_index) / sqrt((float)_index+1),
                                   sqrt((float)_index) / sqrt((float)_index+1),
                                   sqrt((float)_index) / sqrt((float)_index+1) )));

    mat.postMult(osg::Matrix::rotate(delta_rot_y * percent, 0,1,0));
    mat.postMult(osg::Matrix::translate(0.006*_index + _startRadius/sqrt((float)_index),
                                        0, h_offset));
    mat.postMult(osg::Matrix::rotate(osg::DegreesToRadians(_distributionAngle) * _index, 0,0,1));
    mat.postMult(osg::Matrix::scale(osg::Vec3(.4,.4,.45)));

    setMatrix(mat);
} 
