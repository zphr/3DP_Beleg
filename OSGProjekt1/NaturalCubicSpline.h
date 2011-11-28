#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#pragma once
using namespace std;

class NaturalCubicSpline
{
    public:
        NaturalCubicSpline(osg::ref_ptr<osg::Vec4Array> knots, int curveSteps=12);
        ~NaturalCubicSpline();

        void calcCoefficientsXYZ();
        osg::Vec4Array* calcCoefficients(float *coords, int count);
        void calcSpline();

        // Auflösung des Splines
        int _curveSteps;
        // Stützpunkte der Splines
        osg::ref_ptr<osg::Vec4Array> _knots;
        // Sheitelpunkte des Splines
        osg::ref_ptr<osg::Vec3Array> _vertices;
        // Polynomkoeffizienten
        osg::ref_ptr<osg::Vec4Array> _coefficientsX;
        osg::ref_ptr<osg::Vec4Array> _coefficientsY;
        osg::ref_ptr<osg::Vec4Array> _coefficientsZ;

        osg::ref_ptr<osg::Geometry> _geometry;

};
