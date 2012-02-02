#include "TrackballAxisDragger.hpp"
    
TrackballAxisDragger::TrackballAxisDragger(osg::Vec3 center, float boundRadius)
{
    _autoTransform = new osg::AutoTransform;
    _autoTransform->setAutoScaleToScreen(true);
    addChild(_autoTransform.get());

    _sizeTransform = new osg::MatrixTransform;
    _sizeTransform->setMatrix(
        osg::Matrix::scale(100, 100, 100));
    _autoTransform->addChild(_sizeTransform.get());

    _rotDragger = new osgManipulator::TrackballDragger;
    _rotDragger->setName("TrackballDragger");
    _sizeTransform->addChild(_rotDragger.get());

    // _scaleDragger = new osgManipulator::ScaleAxisDragger;
    // _scaleDragger->setName("ScaleAxisDragger");
    // addChild(_scaleDragger.get());

    _transDragger = new osgManipulator::TranslateAxisDragger;
    _transDragger->setName("TranslateAxisDragger");
    _sizeTransform->addChild(_transDragger.get());

    float scale = 1.25;
    this->setMatrix( osg::Matrix::scale(boundRadius*scale,
                                        boundRadius*scale,
                                        boundRadius*scale)
                     * osg::Matrix::translate(center) );

    this->addDragger(_rotDragger.get());
    // this->addDragger(_scaleDragger.get());
    this->addDragger(_transDragger.get());
    this->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    this->setParentDragger(getParentDragger());
}

void TrackballAxisDragger::setupDefaultGeometry()
{
    _rotDragger->setupDefaultGeometry();

    float axesScale = 1.5;
    _transDragger->setMatrix(
        osg::Matrix::scale(axesScale,axesScale,axesScale));
    _transDragger->setupDefaultGeometry();
}
