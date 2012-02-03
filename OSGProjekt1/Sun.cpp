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
    _lastRotMatrix = osg::Matrix().rotate(
        osg::DegreesToRadians(Sun::_rotationAngle), osg::X_AXIS);

    _vec = osg::Vec4(0.0, 0.0, _circleRadius, 0.0);

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

    preMult( osg::Matrix().rotate(osg::DegreesToRadians(20.0), osg::X_AXIS) );
}

Sun::~Sun()
{
    
}

void Sun::dimLight( float percent )
{
    if(percent <= 0.0)
        percent = 0.0001;

    // Cosinus Verlauf für die Helligkeit
    percent = cos((1-percent) * (M_PI/2.0));

    osg::Vec4 gradient_color =
        _colorGradient.getColorAtPercent( percent );

    _viewer->getCamera()->setClearColor( osg::Vec4( gradient_color.x() * percent,
                                                    gradient_color.y() * percent,
                                                    gradient_color.z() * percent,
                                                    1.0) );

    _lightSrc->getLight()->setDiffuse( osg::Vec4( _diffuseLight.x() * percent,
                                                  _diffuseLight.y() * percent,
                                                  _diffuseLight.z() * percent,
                                                  1.0) );

    _lightSrc->getLight()->setSpecular( osg::Vec4( _specularLight.x() * percent,
                                                   _specularLight.y() * percent,
                                                   _specularLight.z() * percent,
                                                   1.0));
    _lightSrc->getLight()->setAmbient( osg::Vec4( gradient_color.x() * percent,
                                                  gradient_color.y() * percent,
                                                  gradient_color.z() * percent,
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

osg::Matrix Sun::getLastRotationMatrix()
{
    return _lastRotMatrix;
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
