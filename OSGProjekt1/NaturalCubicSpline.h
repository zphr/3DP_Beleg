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

class NaturalCubicSpline: public BaseCurve
{

    public:
        NaturalCubicSpline(
                int curveSteps=3,
                BaseCurve *extrudeShape= new CircleCurve(3),
                NaturalCubicSpline* profile = 0,
                osg::Vec3 firstFrameX = osg::Vec3(1,0,0),
                osg::Vec3 firstFrameY = osg::Vec3(0,1,0),
                osg::Vec3 firstFrameZ = osg::Vec3(0,0,1));

        NaturalCubicSpline(
                osg::ref_ptr<osg::Vec4Array> knots,
                int curveSteps=3,
                BaseCurve* extrudeShape= new CircleCurve(3),
                NaturalCubicSpline* profile = 0,
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
        
        inline osg::Matrix getFrame(int n, int stepping=1);
        inline osg::Matrix getKnotFrame(int n);
        osg::Vec3Array* getKnotFrameVectors(int n);
        osg::Vec3Array* getFrameVectors(int n, int stepping=1);

        inline osg::Vec3 convVec4To3(const osg::Vec4 &vec4);
        inline osg::Vec3Array* cloneVec4ArrayTo3( osg::ref_ptr<osg::Vec4Array> vec4_array);

        osg::Vec4Array* calcPoints(unsigned int resolution);
        osg::Geometry* buildMeshAlongPath(unsigned int resolution,
                                          float scale,
                                          const vector<osg::Matrix> &matrices,
                                          const osg::ref_ptr<osg::Vec3Array> &vertices,
                                          const vector<float> &profile_scale = vector<float>());

        void setKnots(osg::Vec4Array* knots);
        inline int getKnotCount();

        void calcPolynomialsXYZ();
        vector<CubicPolynomial> calcPolynomials(float *coords, int count);
        void calcSpline();
        osg::Geometry* drawSpline();
        inline osg::Vec3 calcAtPercent(float percent);
        inline osg::Vec3 calcAt(int i, float t);

        float calcProfileAt(float t);
        float calcProfileAtPercent(float percent);

        osg::Geometry* drawTangentFrames();
        osg::Geometry* drawTangentFrameAt(float t);
        osg::Geometry* drawTangentFrame(osg::Matrix mat);
        void calcTangentFrames();
        osg::Vec3 calcTangentAt(int i, float t);
        osg::Vec3 calcTangentAtPercent(float percent);
        osg::Geometry* getPointSprites(osg::ref_ptr<osg::Geode> root);

        inline void calcPolyIndexAndU(float t, int &i, float &u);
        inline float calcShare(float percent);

        osg::Geometry* drawExtrudedCylinder(unsigned int resolution=3, float scale=1.0f);

        osg::Matrix calcFrameAt(float u);
        
        // Auflösung des Splines
        int _curveSteps;
        // Stützpunkte der Splines
        osg::ref_ptr<osg::Vec4Array> _knots;
        // Scheitelpunkte des Splines
        osg::ref_ptr<osg::Vec3Array> _vertices;
        osg::Vec3Array* getVertices();
        osg::ref_ptr<osg::Vec3Array> _tangents;
        vector<osg::Matrix> _matrices;

        osg::ref_ptr<osg::Geometry> _geometry;
        osg::ref_ptr<osg::Geometry> _tangentCoordinateSysGeo;

        osg::Vec3 _firstFrameX;
        osg::Vec3 _firstFrameY;
        osg::Vec3 _firstFrameZ;

        void setExtrudeShape(BaseCurve* extrudeShape);

    private:
        NaturalCubicSpline* _profile;
        vector<float> _profileScale;
        BaseCurve *_extrudeShape;

        vector<CubicPolynomial> _polynomialsX;
        vector<CubicPolynomial> _polynomialsY;
        vector<CubicPolynomial> _polynomialsZ;

};
