#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include "CubicPolynomial.h"
#include "CircleCurve.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>

#pragma once
using namespace std;

class NaturalCubicSpline
{
    private:
         BaseCurve _extrudeShape;

    public:
        NaturalCubicSpline(
                int curveSteps=3,
                BaseCurve extrudeShape=CircleCurve(3),
                osg::Vec3 firstFrameX = osg::Vec3(1,0,0),
                osg::Vec3 firstFrameY = osg::Vec3(0,1,0),
                osg::Vec3 firstFrameZ = osg::Vec3(0,0,1));

        NaturalCubicSpline(
                osg::ref_ptr<osg::Vec4Array> knots,
                int curveSteps=3,
                BaseCurve extrudeShape=CircleCurve(3),
                osg::Vec3 firstFrameX = osg::Vec3(1,0,0),
                osg::Vec3 firstFrameY = osg::Vec3(0,1,0),
                osg::Vec3 firstFrameZ = osg::Vec3(0,0,1));

        ~NaturalCubicSpline();

        void setFirstFrame(osg::ref_ptr<osg::Vec3Array> vecs);

        void setFirstFrame(osg::Vec3 firstFrameX,
                           osg::Vec3 firstFrameY,
                           osg::Vec3 firstFrameZ);

        void getFirstFrame(osg::Vec3 &firstFrameX,
                           osg::Vec3 &firstFrameY,
                           osg::Vec3 &firstFrameZ);
        inline osg::Matrix getKnotFrame(int n);
        osg::Vec3Array* getKnotFrameVectors(int n);

        void setKnots(osg::Vec4Array* knots);

        void calcPolynomialsXYZ();
        vector<CubicPolynomial> calcPolynomials(float *coords, int count);
        void calcSpline();
        osg::Geometry* drawSpline();
        inline osg::Vec3 calcAt(int i, float t);

        osg::Geometry* drawTangentFrames();
        void calcTangentFrames();
        osg::Vec3 calcTangentAt(int i, float t);
        osg::Geometry* getPointSprites(osg::ref_ptr<osg::Geode> root);

        osg::Geometry* drawExtrudedCylinder(unsigned int resolution=3, float scale=1.0f);

        // Auflösung des Splines
        int _curveSteps;
        // Stützpunkte der Splines
        osg::ref_ptr<osg::Vec4Array> _knots;
        // Scheitelpunkte des Splines
        osg::ref_ptr<osg::Vec3Array> _vertices;
        osg::ref_ptr<osg::Vec3Array> _tangents;
        vector<osg::Matrix> _matrices;

        vector<CubicPolynomial> _polynomialsX;
        vector<CubicPolynomial> _polynomialsY;
        vector<CubicPolynomial> _polynomialsZ;

        osg::ref_ptr<osg::Geometry> _geometry;
        osg::ref_ptr<osg::Geometry> _tangentCoordinateSysGeo;

        osg::Vec3 _firstFrameX;
        osg::Vec3 _firstFrameY;
        osg::Vec3 _firstFrameZ;
};
