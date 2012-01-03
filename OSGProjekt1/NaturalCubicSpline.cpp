#include "NaturalCubicSpline.h"

NaturalCubicSpline::NaturalCubicSpline(
        int curveSteps,
        BaseCurve *extrudeShape,
        NaturalCubicSpline* profile,
        osg::Vec3 firstFrameX,
        osg::Vec3 firstFrameY,
        osg::Vec3 firstFrameZ)
:  _curveSteps(curveSteps)
{
    _extrudeShape = extrudeShape;

    _knots = new osg::Vec4Array;

    _knots->push_back( osg::Vec4(0,1,0,1));
    _knots->push_back( osg::Vec4(1,0,0,1));

    _firstFrameX = firstFrameX;
    _firstFrameY = firstFrameY;
    _firstFrameZ = firstFrameZ;

    _profile = profile;

    _geometry = new osg::Geometry;
}

NaturalCubicSpline::NaturalCubicSpline(
        osg::ref_ptr<osg::Vec4Array> knots,
        int curveSteps,
        BaseCurve *extrudeShape,
        NaturalCubicSpline* profile,
        osg::Vec3 firstFrameX,
        osg::Vec3 firstFrameY,
        osg::Vec3 firstFrameZ)
:  _knots(knots.get()), _curveSteps(curveSteps)
{
    _extrudeShape = extrudeShape;

    _firstFrameX = firstFrameX;
    _firstFrameY = firstFrameY;
    _firstFrameZ = firstFrameZ;

    _profile = profile;

    _geometry = new osg::Geometry;
    calcSpline();
}

NaturalCubicSpline::~NaturalCubicSpline()
{
    _vertices.release();
    _tangents.release();
}

void NaturalCubicSpline::getFirstFrame(osg::Vec3 &firstFrameX,
                                       osg::Vec3 &firstFrameY,
                                       osg::Vec3 &firstFrameZ)
{
    firstFrameX = _firstFrameX;
    firstFrameY = _firstFrameY;
    firstFrameZ = _firstFrameZ;
}

void NaturalCubicSpline::setFirstFrame(osg::ref_ptr<osg::Vec3Array> vecs)
{
    _firstFrameX = (*vecs)[0];
    _firstFrameY = (*vecs)[1];
    _firstFrameZ = (*vecs)[2];
}

void NaturalCubicSpline::setFirstFrame(osg::Vec3 firstFrameX,
                                       osg::Vec3 firstFrameY,
                                       osg::Vec3 firstFrameZ)
{
    _firstFrameX = firstFrameX;
    _firstFrameY = firstFrameY;
    _firstFrameZ = firstFrameZ;
}

inline osg::Matrix NaturalCubicSpline::getFrame(int n, int stepping)
{
    // TODO: Exception auslösen
    if(n >= _matrices.size())
         return osg::Matrix();
    else
         return _matrices[stepping * n];
}


inline osg::Matrix NaturalCubicSpline::getKnotFrame(int n)
{
    return getFrame(n, _curveSteps);
}

osg::Vec3Array* NaturalCubicSpline::getKnotFrameVectors(int n)
{
    return getFrameVectors(n, _curveSteps);
}

osg::Vec3Array* NaturalCubicSpline::getFrameVectors(int n, int stepping)
{
    osg::ref_ptr<osg::Vec3Array> vec_array = new osg::Vec3Array;

    // TODO: Exception auslösen
    if(n >= _matrices.size())
    {
        vec_array->push_back(osg::Vec3(1,0,0));
        vec_array->push_back(osg::Vec3(0,1,0));
        vec_array->push_back(osg::Vec3(0,0,1));
    }
    else
    {
        osg::Matrix mat = _matrices[stepping * n];

        vec_array->push_back(osg::Vec3(mat(0,0), mat(1,0), mat(2,0))); // X-Achse
        vec_array->push_back(osg::Vec3(mat(0,1), mat(1,1), mat(2,1))); // Y-Achse
        vec_array->push_back(osg::Vec3(mat(0,2), mat(1,2), mat(2,2))); // Z-Achse
    }

    return vec_array.release();
}


void NaturalCubicSpline::setKnots(osg::Vec4Array* knots)
{
    // TODO: hier wird doch sicherlich nur der Pointer übergeben ->
    // könnte Probleme geben, allerdings sind dann die Daten auch
    // neu...
    _knots = knots;
}

void NaturalCubicSpline::calcPolynomialsXYZ()
{
    float *coords_x = new float[_knots->getNumElements()];
    float *coords_y = new float[_knots->getNumElements()];
    float *coords_z = new float[_knots->getNumElements()];

    for(int i=0; i < _knots->getNumElements(); i++)
    {
        coords_x[i] = (*_knots)[i].x();
        coords_y[i] = (*_knots)[i].y();
        coords_z[i] = (*_knots)[i].z();
    }

    _polynomialsX = calcPolynomials(coords_x, _knots->getNumElements()-1);
    _polynomialsY = calcPolynomials(coords_y, _knots->getNumElements()-1);
    _polynomialsZ = calcPolynomials(coords_z, _knots->getNumElements()-1);

    delete coords_x;
    delete coords_y;
    delete coords_z;
}

vector<CubicPolynomial> NaturalCubicSpline::calcPolynomials(float *coords, int count)
{
    // basierend auf "An Introduction to Splines for Use in Computer
    // Graphics and Geometric Modeling"
    float *gamma = new float[count+1];
    float *delta = new float[count+1];
    float *derivative_result = new float[count+1];

    gamma[0] = 1.0/2.0;
    for(int i = 1; i < (count); i++)
    {
        gamma[i] = 1 / (4.0 - gamma[i-1]);
    }
    // das letzte gamma Element wird nur für die Ergebnisse (delta)
    // verwendet die mit diesem multipliziert werden
    gamma[count] = 1 / (2.0 - gamma[count-1]);

    // Dreiecksmatrix lösen -> Gauß-Verfahren
    delta[0] = 3*(coords[1] - coords[0]) * gamma[0];
    for(int i = 1; i < (count); i++)
    {
        delta[i] = (3*(coords[i+1] - coords[i-1]) - delta[i-1]) * gamma[i];
    }
    delta[count] = (3*(coords[count] - coords[count-1]) - delta[count-1]) * gamma[count];

    // rechte Seite der Matrix im Gauß-Verfahren berücksichtigen
    derivative_result[count] = delta[count];
    for(int i = (count-1); i >= 0; i--)
    {
        derivative_result[i] = delta[i] - derivative_result[i+1] * gamma[i];
    }

    vector<CubicPolynomial> polynomials;

    for(int i = 0; i < (count); i++)
    {
        // Koeffizienten der Polynome berechnen
        polynomials.push_back( CubicPolynomial(
                    osg::Vec4(
                        coords[i],      // d
                        derivative_result[i], // c
                        3*(coords[i+1]-coords[i]) - 2*derivative_result[i] - derivative_result[i+1], // b
                        2*(coords[i]-coords[i+1]) + derivative_result[i] + derivative_result[i+1] // a
                        )
                    ));
    }

    delete gamma;
    delete delta;
    delete derivative_result;

    return polynomials;
}

inline void NaturalCubicSpline::calcPolyIndexAndU(float t, int &i, float &u)
{
    i = (int) t;
    u = t - (float) i;

    if((i == _polynomialsY.size()) && (u == 0.0))
    {
        i -= 1;
        u = 1.0;
    }
}

inline float NaturalCubicSpline::calcShare(float percent)
{
    return (float)_polynomialsY.size() * percent;
}

osg::Vec3 NaturalCubicSpline::calcTangentAtPercent(float percent)
{
    float t = calcShare(percent);
    int i = 0;
    float u = 0.0;
    calcPolyIndexAndU(t, i, u);

    return calcTangentAt(i, u);
}


osg::Vec3 NaturalCubicSpline::calcTangentAt(int i, float t)
{
    float tangent_x = _polynomialsX[i].firstDerivCalcAt(t);
    float tangent_y = _polynomialsY[i].firstDerivCalcAt(t);
    float tangent_z = _polynomialsZ[i].firstDerivCalcAt(t);
    osg::Vec3 tangent(tangent_x,tangent_y,tangent_z); // y
    tangent.normalize();

    return tangent;
}

osg::Geometry* NaturalCubicSpline::drawExtrudedCylinder(unsigned int resolution, float scale)
{
    // wenn die Basisdaten noch nicht berechnet wurden dann sollte man
    // das mal anstoßen
    if((_vertices == 0) || (_matrices.size() == 0))
         calcSpline();

    return (_extrudeShape->buildMeshAlongPath(resolution,
                                              scale,
                                              _matrices,
                                              _vertices,
                                              _profileScale));
}

osg::Geometry* NaturalCubicSpline::drawTangentFrames()
{
    osg::Vec4 vec;               // Ortsvektor
    osg::Vec4 x_axis(1,0,0,1);
    osg::Vec4 y_axis(0,1,0,1);
    osg::Vec4 z_axis(0,0,1,1);
    osg::Matrix mat;

    osg::ref_ptr<osg::Vec4Array>tangent_vertices = new osg::Vec4Array;

    for(int i = 0; i < _matrices.size(); i++)
    {
        vec = osg::Vec4(
                (*_vertices)[i].x(),
                (*_vertices)[i].y(),
                (*_vertices)[i].z(),
                1
                );

        mat = _matrices[i];

        tangent_vertices->push_back(vec);
        tangent_vertices->push_back(mat*x_axis);

        tangent_vertices->push_back(vec);
        tangent_vertices->push_back(mat*y_axis);

        tangent_vertices->push_back(vec);
        tangent_vertices->push_back(mat*z_axis);
    }

    osg::ref_ptr<osg::Geometry> tangent_geo = new osg::Geometry;
    tangent_geo->setVertexArray( tangent_vertices );
    tangent_geo->addPrimitiveSet(
            new osg::DrawArrays(GL_LINES, 0, tangent_vertices->getNumElements()) );

    return tangent_geo.release();
}

osg::Geometry* NaturalCubicSpline::drawTangentFrame(osg::Matrix mat)
{
    osg::Vec4 vec;               // Ortsvektor
    osg::Vec4 x_axis(1,0,0,1);
    osg::Vec4 y_axis(0,1,0,1);
    osg::Vec4 z_axis(0,0,1,1);

    osg::ref_ptr<osg::Vec4Array>tangent_vertices = new osg::Vec4Array;

    vec = osg::Vec4( mat(0,3), mat(1,3), mat(2,3), 1 );

    tangent_vertices->push_back(vec);
    tangent_vertices->push_back(mat*x_axis);

    tangent_vertices->push_back(vec);
    tangent_vertices->push_back(mat*y_axis);

    tangent_vertices->push_back(vec);
    tangent_vertices->push_back(mat*z_axis);

    osg::ref_ptr<osg::Geometry> tangent_geo = new osg::Geometry;
    tangent_geo->setVertexArray( tangent_vertices );
    tangent_geo->addPrimitiveSet(
            new osg::DrawArrays(GL_LINES, 0, tangent_vertices->getNumElements()) );

    return tangent_geo.release();
}


void NaturalCubicSpline::calcTangentFrames()
{
    osg::ref_ptr<osg::Vec3Array> r = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> s = new osg::Vec3Array;

    // ersten Frame/erstes Koordinatensystem vorgeben
    r->push_back( _firstFrameX );   // x
    s->push_back( _firstFrameY );   // y
    (*_tangents)[0] = _firstFrameZ; // z

    osg::Matrix mat(
            (*r)[0].x(), (*s)[0].x(), (*_tangents)[0].x(), (*_vertices)[0].x(),
            (*r)[0].y(), (*s)[0].y(), (*_tangents)[0].y(), (*_vertices)[0].y(),
            (*r)[0].z(), (*s)[0].z(), (*_tangents)[0].z(), (*_vertices)[0].z(),
            0,           0,          0,       1
            );

    _matrices.push_back(mat);

    // Algorithmus = "Double Reflection" basierend auf dem Paper
    // "Computation of Rotation Minimizing Frame in Computer Graphics"
    for(int i = 0; i < _vertices->getNumElements()-1; i++)
    {
        osg::Vec3 v1 = (*_vertices)[i+1] - (*_vertices)[i];
        float c1 = v1 * v1;

        osg::Vec3 rLi = (*r)[i] - v1 * (v1 * ((*r)[i])) * (2/c1);
        osg::Vec3 tLi = (*_tangents)[i] -  v1 * (v1 * (*_tangents)[i]) * (2/c1);

        osg::Vec3 v2 = (*_tangents)[i+1] - tLi;
        float c2 = v2 * v2;
        r->push_back( osg::Vec3( rLi - v2 * (v2 * rLi) * (2/c2) ) );
        s->push_back( (*_tangents)[i+1] ^ (*r)[i+1] );

        mat = osg::Matrix(
                (*r)[i+1].x(), (*s)[i+1].x(), (*_tangents)[i+1].x(), (*_vertices)[i+1].x(),
                (*r)[i+1].y(), (*s)[i+1].y(), (*_tangents)[i+1].y(), (*_vertices)[i+1].y(),
                (*r)[i+1].z(), (*s)[i+1].z(), (*_tangents)[i+1].z(), (*_vertices)[i+1].z(),
                0,           0,          0,       1
                );

        _matrices.push_back(mat);
    }
}

osg::Geometry* NaturalCubicSpline::drawSpline()
{
    // TODO: muss ich _geometry wirklich immer neu allokieren?
    if(_geometry)
        _geometry.release();
    _geometry = new osg::Geometry;

    _geometry->setVertexArray( _vertices );
    _geometry->addPrimitiveSet(
            new osg::DrawArrays(GL_LINE_STRIP, 0, _vertices->getNumElements()) );

    return _geometry.get();
}

inline osg::Vec3 NaturalCubicSpline::calcAtPercent(float percent)
{
    float t = calcShare(percent);
    int i = 0;
    float u = 0.0;
    calcPolyIndexAndU(t, i, u);

    return calcAt(i, u);
}


inline osg::Vec3 NaturalCubicSpline::calcAt(int i, float t)
{
    float x = _polynomialsX[i].calcAt(t);
    float y = _polynomialsY[i].calcAt(t);
    float z = _polynomialsZ[i].calcAt(t);

    return osg::Vec3(x,y,z);
}

float NaturalCubicSpline::calcProfileAtPercent(float percent)
{
    float t = (float)_polynomialsY.size() * percent;

    return calcProfileAt(t);
}

float NaturalCubicSpline::calcProfileAt(float t)
{
    int i = (int) t;
    float u = t - (float) i;

    if((i == _polynomialsY.size()) && (u == 0.0))
    {
        i -= 1;
        u = 1.0;
    }

    float max_x = 0.0;
    float max_y = 0.0;
    float max_z = 0.0;

    for( int i=0; i<_knots->getNumElements(); i++)
    {
        if((*_knots)[i].x() > max_x)
            max_x = (*_knots)[i].x();

        if((*_knots)[i].y() > max_y)
            max_y = (*_knots)[i].y();

        if((*_knots)[i].z() > max_z)
            max_z = (*_knots)[i].z();
    }

    float x = _polynomialsX[i].calcAt(u);
    float y = _polynomialsY[i].calcAt(u);
    float z = _polynomialsZ[i].calcAt(u);

    if(max_x == 0)
        max_x = 1;

    if(max_y == 0)
        max_y = 1;

    if(max_z == 0)
        max_z = 1;

    // Normalisieren
    osg::Vec3 vec(x/max_x,
                  y/max_y,
                  z/max_z);

    return y;
}

void NaturalCubicSpline::calcSpline()
{
    calcPolynomialsXYZ();

    if(_vertices)
        _vertices.release();
    _vertices = new osg::Vec3Array;

    if(_tangents)
        _tangents.release();
    _tangents = new osg::Vec3Array;

    _vertices->push_back( calcAt(0, 0) );
    _tangents->push_back( calcTangentAt(0, 0) );

    if(_profile)
        _profileScale.push_back( _profile->calcProfileAtPercent( 0 ) );

    int polynomial_count = _polynomialsX.size();

    float u = 0.0f;
    for(int i=0; i < _knots->getNumElements()-1; i++)
    {
        for(int j = 1; j <=_curveSteps; j++)
        {
            u = j / (float) _curveSteps;

            _vertices->push_back( calcAt(i, u) );
            _tangents->push_back( calcTangentAt(i, u) );

            if(_profile)
                _profileScale.push_back(
                        _profile->calcProfileAtPercent(
                                ((float)i + u) / (float) polynomial_count ));

        }
    }

    calcTangentFrames();
}

inline int NaturalCubicSpline::getKnotCount()
{
    return _knots->getNumElements();
}

osg::Geometry* NaturalCubicSpline::getPointSprites(osg::ref_ptr<osg::Geode> root)
{
    osg::ref_ptr<osg::Geometry> sprites = new osg::Geometry;
    sprites->setVertexArray( _knots.get() );

    sprites->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, _knots->getNumElements()) );

    osg::StateSet* set = root->getOrCreateStateSet();
    set->setMode(GL_BLEND, osg::StateAttribute::ON);
    osg::BlendFunc *fn = new osg::BlendFunc();
    fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
    set->setAttributeAndModes(fn, osg::StateAttribute::ON);

    /// Setup the point sprites
    osg::PointSprite *sprite = new osg::PointSprite();
    set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);

    /// Give some size to the points to be able to see the sprite
    osg::Point *point = new osg::Point();
    point->setSize(5);
    set->setAttribute(point);

    /// Disable depth test to avoid sort problems and Lighting
    set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    return sprites.release();
}

inline osg::Vec3Array* NaturalCubicSpline::cloneVec4ArrayTo3( osg::ref_ptr<osg::Vec4Array> vec4_array)
{
    osg::ref_ptr<osg::Vec3Array> vec = new osg::Vec3Array();

    for(int i=0; i < vec4_array->getNumElements(); i++)
        vec->push_back( osg::Vec3((*vec4_array)[i].x(),
                                   (*vec4_array)[i].y(),
                                   (*vec4_array)[i].z()));

    return vec.release();
}


inline osg::Vec3 NaturalCubicSpline::convVec4To3(const osg::Vec4 &vec4)
{
    return osg::Vec3(vec4.x(), vec4.y(), vec4.z());
}

osg::Geometry* NaturalCubicSpline::buildMeshAlongPath(unsigned int resolution,
                                                   float scale,
                                                   const vector<osg::Matrix> &matrices,
                                                   const osg::ref_ptr<osg::Vec3Array> &vertices,
                                                   const vector<float> &profileScale)
{

    unsigned int elements = vertices->getNumElements();
    int k = 0;

    osg::ref_ptr<osg::Vec4Array> shape_verts = calcPoints(resolution);

    // die Auflösung wird für ein NaturalCubicSpline zwischen den
    // Segmenten angegeben, daher muss noch die absolute Auflösung
    // geholten werden!
    resolution = shape_verts->getNumElements();

    osg::Vec4 vert;
    osg::ref_ptr<osg::Vec4Array> verts = new osg::Vec4Array;

    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    for(int i=0; i < elements; i++)
    {
        for(int j=0; j < shape_verts->getNumElements(); j++)
        {
            
            vert = osg::Vec4((*shape_verts)[j]);
            vert *= scale;
            
            if(profileScale.size() == elements)
                vert *= profileScale[i];

            vert[3] = 1;
            vert = matrices[i] * vert;

            verts->push_back(vert);

            if((i < (elements-1)) && (j < (resolution-1)))
            {
                face_indices->push_back( i * resolution + j );
                face_indices->push_back( i * resolution + (j + 1));
                face_indices->push_back( (i+1) * resolution + (j + 1));
                face_indices->push_back( (i+1) * resolution + j);
            }
        }
    }

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    int vert_count = verts->getNumElements();
    osg::Vec3 n;

    osg::Vec3 e1, e2, e3, e4;


    for(int i=0; i < vert_count; i++)
    {
        
        int imodr = (i % resolution);
        if(imodr == 0)
        {
            // Unten ganz links
            if(i == 0)
            {
                e2 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));
                n = e3 ^ e2;
            }
            // Oben ganz links
            else if(i == (vert_count-resolution))
            {
                e1 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));
                n = e2 ^ e1;
            }
            // am linken Rand 
            else
            {
                e1 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n /= 2;
            }

        }
        else if(imodr == (resolution-1))
        {

            // Unten ganz rechts
            if(i == (resolution-1))
            {
                e1 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));

                n = e2 ^ e1;
            }
            // Oben ganz rechts
            else if(i == (vert_count-1))
            {
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));

                n = e3 ^ e2;
            }
            // rechter Rand
            else
            {
                e1 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n /= 2;
            }

        }
        else
        {
            // Unterer Rand
            if( (i / resolution) == 0)
            {
                e1 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));
                e4 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));

                n = e2 ^ e1;
                n += e1 ^ e4;
                n /= 2;
            }
            // Oberer Rand
            else if((i/(vert_count - resolution) <= 1))
            {
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));
                e4 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));

                n = e3 ^ e2;
                n += e4 ^ e3;
                n /= 2;
            }
            // in der Mitte
            else
            {
                e1 = (convVec4To3((*verts)[i+resolution]) - convVec4To3((*verts)[i]));
                e2 = (convVec4To3((*verts)[i-1]) - convVec4To3((*verts)[i]));
                e3 = (convVec4To3((*verts)[i-resolution]) - convVec4To3((*verts)[i]));
                e4 = (convVec4To3((*verts)[i+1]) - convVec4To3((*verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n += e4 ^ e3;
                n += e1 ^ e4;
                n /= 4;
            }
        }

        n.normalize();
        normals->push_back( n );
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray( verts.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->addPrimitiveSet( face_indices.get() );

    return geom.release();
}

osg::Vec4Array* NaturalCubicSpline::calcPoints(unsigned int resolution)
{
    _curveSteps = resolution;
    calcSpline();

    osg::ref_ptr<osg::Vec4Array> vecs = new osg::Vec4Array();

    for(int i=0; i < _vertices->getNumElements(); i++)
        vecs->push_back( osg::Vec4(
                                 (*_vertices)[i].x(),
                                 (*_vertices)[i].y(),
                                 (*_vertices)[i].z(),
                                 1.0)
                       );

    return vecs.release();
}

osg::Matrix NaturalCubicSpline::calcFrameAt(float u)
{
    int frame_count = _vertices->getNumElements();
    float prev_framef = (u * (frame_count - 1));
    int prev_frame = (int) prev_framef;

    if((prev_framef - prev_frame) < 0.005)
        return getFrame( prev_frame );

    osg::ref_ptr<osg::Vec3Array> prev_vecs = getFrameVectors( prev_frame );

    osg::Vec3 r( (*prev_vecs)[0] );
    osg::Vec3 s( (*prev_vecs)[1] );
    osg::Vec3 t( (*prev_vecs)[2] );
    osg::Vec3 vertex( (*_vertices)[prev_frame] );

    osg::Vec3 target_r;
    osg::Vec3 target_s;
    osg::Vec3 target_t( calcTangentAtPercent(u) );
    osg::Vec3 target_v( calcAtPercent(u) );

    // Double Reflection
    osg::Vec3 v1 = target_v - vertex;
    float c1 = v1 * v1;

    osg::Vec3 rLi = r - v1 * (v1 * r) * (2/c1);
    osg::Vec3 tLi = t -  v1 * (v1 * t) * (2/c1);

    osg::Vec3 v2 = target_t - tLi;
    float c2 = v2 * v2;
    target_r =  osg::Vec3( rLi - v2 * (v2 * rLi) * (2/c2) );
    target_s = target_t ^ target_r;

    osg::Matrix mat = osg::Matrix(
            target_r.x(), target_s.x(), target_t.x(), target_v.x(),
            target_r.y(), target_s.y(), target_t.y(), target_v.y(),
            target_r.z(), target_s.z(), target_t.z(), target_v.z(),
            0,           0,          0,       1
    );

    return mat;
}
