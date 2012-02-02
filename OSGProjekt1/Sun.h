#pragma once
#include <iostream>
#include <fstream>
#include <osg/MatrixTransform>
#include <osg/AutoTransform>
#include <osgManipulator/TrackballDragger>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgViewer/Viewer>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>

#include "ColorGradient.h"
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

class Sun: public osg::MatrixTransform
{
  public:
    Sun(float     circleRadius,
        ColorGradient colorGradient,
        osgViewer::Viewer *viewer,
        unsigned int shadowMapRes = 1024,
        osg::Vec4 diffuseLight  = osg::Vec4(0.8, 0.8, 0.8, 1.0),
        osg::Vec4 specularLight = osg::Vec4(0.5, 0.5, 0.5, 1.0),
        osg::Vec4 ambientLight  = osg::Vec4(0.25, 0.25, 0.5, 1.0));

    ~Sun();

    static float _zPos;
    static float _rotationAngle;
    // static float _circleRadius;
    float getSunZPos();
    void dimLight( float percent );
    void setDraggerHandleEvents(bool handleEvents);

    void toggleManualRotation();
    bool getManualRotation();
    void setManualRotation(bool manualRotation);
    void setLastRotationMatrix(osg::Matrix lastRotMatrix);
    void rotateWithLastRotMatrix();

    osgShadow::ShadowMap* getShadowMap();

  protected:

    bool _manualRotation;
    float _circleRadius;
    osg::Vec4 _vec;

    osg::Vec4 _diffuseLight;
    osg::Vec4 _specularLight;
    osg::Vec4 _ambientLight;
    
    osg::ref_ptr<osgShadow::ShadowMap> _shadowMap;
    unsigned int _shadowMapRes;

    osg::ref_ptr<osg::AutoTransform> _autoTransform;
    osg::ref_ptr<osg::MatrixTransform> _sizeTransform;
    osg::ref_ptr<osgManipulator::TrackballDragger> _rotDragger;
    
    osg::ref_ptr<osg::LightSource> _lightSrc;
    ColorGradient _colorGradient;
    osgViewer::Viewer *_viewer;

    osg::Matrix _lastRotMatrix;
};

class SunCallback : public osg::NodeCallback 
{
  public:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::ref_ptr<Sun> sun = dynamic_cast<Sun*> (node);

        if(sun)
        {
            if(!sun->getManualRotation())
                sun->rotateWithLastRotMatrix();

            Sun::_zPos = sun->getSunZPos();
            sun->dimLight( Sun::_zPos );
            
        }

        traverse(node, nv); 
    }
};
