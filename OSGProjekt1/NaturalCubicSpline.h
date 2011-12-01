#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include "CubicPolynomial.h"

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>

#pragma once
using namespace std;

class NaturalCubicSpline
{
    public:
        NaturalCubicSpline(osg::ref_ptr<osg::Vec4Array> knots, int curveSteps=12);
        ~NaturalCubicSpline();

        void calcPolynomialsXYZ();
        vector<CubicPolynomial> calcPolynomials(float *coords, int count);
        void calcSpline();
        osg::Geometry* drawSpline();
        osg::Geometry* drawCylinderAlongSpline();
        inline osg::Vec3 calcAt(float t);

        void calcDoubleReflection(osg::ref_ptr<osg::Geode> root);
        void calcTangentCoordinateSystems(osg::ref_ptr<osg::Geode> root);
        osg::Vec3 calcTangentAt(float t);
        osg::Geometry* getPointSprites(osg::ref_ptr<osg::Geode> root);

        // Auflösung des Splines
        int _curveSteps;
        // Stützpunkte der Splines
        osg::ref_ptr<osg::Vec4Array> _knots;
        // Scheitelpunkte des Splines
        osg::ref_ptr<osg::Vec3Array> _vertices;
        osg::ref_ptr<osg::Vec3Array> _tangents;

        vector<CubicPolynomial> _polynomialsX;
        vector<CubicPolynomial> _polynomialsY;
        vector<CubicPolynomial> _polynomialsZ;

        osg::ref_ptr<osg::Geometry> _geometry;
        osg::ref_ptr<osg::Geometry> _tangentCoordinateSysGeo;

};
