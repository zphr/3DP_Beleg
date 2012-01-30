#pragma once
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include "CubicPolynomial.h"
#include "BaseCurve.h"
#include "CircleCurve.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>

using namespace std;

class NaturalCubicSpline: public BaseCurve
{

  public:
    NaturalCubicSpline();

    NaturalCubicSpline(
        int splineRes,
        BaseCurve *extrudeShape= new CircleCurve(3),
        NaturalCubicSpline* profile = 0,
        osg::Vec3 firstFrameX = osg::Vec3(1,0,0),
        osg::Vec3 firstFrameY = osg::Vec3(0,1,0),
        osg::Vec3 firstFrameZ = osg::Vec3(0,0,1));

    NaturalCubicSpline(
        osg::ref_ptr<osg::Vec4Array> knots,
        int splineRes = 3,
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

    void recalcSpline(unsigned int splineRes);
    void setKnots(osg::Vec4Array* knots);
    inline int getKnotCount();
    void setExtrudeShape(BaseCurve* extrudeShape);

    void calcPolynomialsXYZ();
    vector<CubicPolynomial> calcPolynomials(float *coords, int count);

    void calcSpline();
    inline osg::Vec3 calcAtPercent(float percent);
    inline osg::Vec3 calcAt(int i, float t);

    void calcTangentFrames();
    osg::Matrix calcFrameAt(float u);
        
    osg::Vec3 calcTangentAt(int i, float t);
    osg::Vec3 calcTangentAtPercent(float percent);
    osg::Geometry* getPointSprites(osg::ref_ptr<osg::Geode> root);

    float calcProfileAt(float t);
    float calcProfileAtPercent(float percent);

    inline void calcPolyIndexAndU(float t, int &i, float &u);
    inline float calcShare(float percent);
        
    osg::Geometry* drawSpline();
    osg::Geometry* drawTangentFrames();
    osg::Geometry* drawTangentFrameAt(float t);
    osg::Geometry* drawTangentFrame(osg::Matrix mat);
    osg::Geometry* buildExtrudedShape( unsigned int extrudeRes,
                                         float scale,
                                         NaturalCubicSpline *profileCurve = 0);
    osg::Vec3Array* getVertices();

    template <class T> float getLength(osg::ref_ptr<T> vertices);
    template <class T> vector<float> mapLengthForUV(osg::ref_ptr<T> vertices);

    inline osg::Vec3 convVec4To3(const osg::Vec4 &vec4);
    inline osg::Vec3Array* cloneVec4ArrayTo3( osg::ref_ptr<osg::Vec4Array> vec4_array);

    osg::Vec4Array* calcPoints(unsigned int splineRes);
    osg::Geometry* buildMeshAlongPath(unsigned int extrudeShapeRes,
                                      float scale,
                                      const vector<osg::Matrix> &matrices,
                                      const osg::ref_ptr<osg::Vec3Array> &vertices,
                                      BaseCurve* profileCurve);


  protected:
    NaturalCubicSpline* _profile;
    vector<float> _profileScale;
    BaseCurve *_extrudeShape;

    // Auflösung des Splines
    int _splineRes;
    // Stützpunkte der Splines
    osg::ref_ptr<osg::Vec4Array> _knots;
    // Scheitelpunkte des Splines
    osg::ref_ptr<osg::Vec3Array> _vertices;
    osg::ref_ptr<osg::Vec3Array> _tangents;
    vector<osg::Matrix> _matrices;

    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Geometry> _tangentCoordinateSysGeo;

    osg::Vec3 _firstFrameX;
    osg::Vec3 _firstFrameY;
    osg::Vec3 _firstFrameZ;


    vector<CubicPolynomial> _polynomialsX;
    vector<CubicPolynomial> _polynomialsY;
    vector<CubicPolynomial> _polynomialsZ;

};
