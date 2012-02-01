#include "Sun.h"


// float Sun::_circleRadius = 1.0;
float Sun::_zPos = 0.0;
float Sun::_rotationAngle = 2.0;

Sun::Sun(float circleRadius,
         ColorGradient colorGradient,
         osgViewer::Viewer *viewer,
         unsigned int shadowMapRes,
         osg::Vec4 diffuseLight,
         osg::Vec4 specularLight,
         osg::Vec4 ambientLight)
    :_circleRadius(circleRadius),
  _colorGradient(colorGradient),
  _diffuseLight(diffuseLight),
  _specularLight(specularLight),
  _ambientLight(ambientLight),
  _viewer(viewer),
  _shadowMapRes(shadowMapRes),
  _manualRotation(true)
{
    _lastRotMatrix =  osg::Matrix().rotate(osg::DegreesToRadians(Sun::_rotationAngle), osg::X_AXIS);
    _autoTransform = new osg::AutoTransform;
    _autoTransform->setAutoScaleToScreen(true);
    addChild(_autoTransform.get());

    _sizeTransform = new osg::MatrixTransform;
    _sizeTransform->setMatrix(osg::Matrix::scale(200, 200, 200));
    _autoTransform->addChild(_sizeTransform.get());

    // _rotDragger = new osgManipulator::TrackballDragger;
    // _rotDragger->setName("TrackballDragger");
    // _rotDragger->setupDefaultGeometry();
    // _sizeTransform->addChild(_rotDragger.get());
    
    // _rotDragger->addTransformUpdating( this );
    // _rotDragger->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    // // _rotDragger->setHandleEvents(true);
    // _rotDragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_SHIFT);

    _vec = osg::Vec4(0.0, 0.0, _circleRadius, 1.0);

    _lightSrc = new osg::LightSource();
    _lightSrc->getLight()->setPosition( _vec );
    _lightSrc->getLight()->setDiffuse( _diffuseLight );
    _lightSrc->getLight()->setSpecular( _specularLight );
    _lightSrc->getLight()->setAmbient( _ambientLight );

    _shadowMap = new osgShadow::ShadowMap;
    _shadowMap->setLight( _lightSrc.get() );
    _shadowMap->setTextureSize( osg::Vec2s(_shadowMapRes, _shadowMapRes) );
    _shadowMap->setTextureUnit( 1 );

    addChild( _lightSrc.get() );

    setUpdateCallback(new SunCallback);
}

Sun::~Sun()
{
    
}

void Sun::setDraggerHandleEvents(bool handleEvents)
{
    _rotDragger->setHandleEvents(handleEvents);
}

void Sun::dimLight( float percent )
{
    if(percent <= 0.0)
        percent = 0.0001;

    osg::Vec4 gradient_color =
        _colorGradient.getColorAtPercent( percent );

    percent = cos((1-percent) * (M_PI/2.0));


    _viewer->getCamera()->setClearColor( osg::Vec4( gradient_color.x() * percent,
                                                    gradient_color.y() * percent,
                                                    gradient_color.z() * percent,
                                                    1.0) );

    _lightSrc->getLight()->setDiffuse( osg::Vec4( gradient_color.x() * percent,
                                                  gradient_color.y() * percent,
                                                  gradient_color.z() * percent,
                                                  1.0) );

    _lightSrc->getLight()->setSpecular( osg::Vec4( _specularLight.x() * percent,
                                                   _specularLight.y() * percent,
                                                   _specularLight.z() * percent,
                                                   1.0));
    _lightSrc->getLight()->setAmbient( osg::Vec4( _ambientLight.x() * percent,
                                                  _ambientLight.y() * percent,
                                                  _ambientLight.z() * percent,
                                                  1.0));


}

float Sun::getSunZPos()
{
    float zPos = ( _vec * getMatrix() ).z();

    return zPos / _circleRadius;
}


void Sun::toggleManualRotation()
{
    _manualRotation = !_manualRotation ;
}

bool Sun::getManualRotation()
{
    return _manualRotation;
}

void Sun::setManualRotation(bool manualRotation)
{
    _manualRotation = manualRotation;
}

void Sun::setLastRotationMatrix(osg::Matrix lastRotMatrix)
{
    _lastRotMatrix = lastRotMatrix;
}

void Sun::rotateWithLastRotMatrix()
{
    preMult(_lastRotMatrix);
}

osgShadow::ShadowMap* Sun::getShadowMap()
{
    return _shadowMap.get();
}
