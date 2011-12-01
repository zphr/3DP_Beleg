#include "NaturalCubicSpline.h"

NaturalCubicSpline::NaturalCubicSpline(
   osg::ref_ptr<osg::Vec4Array> knots,
   int curveSteps)
   :  _knots(knots.get()), _curveSteps(curveSteps)

{
   _geometry = new osg::Geometry;
   calcPolynomialsXYZ();
   calcSpline();

}

NaturalCubicSpline::~NaturalCubicSpline()
{
   _vertices.release();
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

   gamma[0] = 1/2.0;
   for(int i = 1; i < (count); i++)
   {
      gamma[i] = 1 / (4.0 - gamma[i-1]);
   }
   // das letzte gamma Element wird nur für die Ergebnisse (delta)
   // verwendet die mit diesem multipliziert werden
   gamma[count] = 1 / (2.0 - gamma[count]);

   // Dreiecksmatrix lösen -> Gauß-Verfahren
   delta[0] = 3*(coords[1] - coords[0]) * gamma[0];
   for(int i = 1; i < (count); i++)
   {
      delta[i] = 3*(coords[i+1] - coords[i]) * gamma[i];
   }
   delta[count] = 3*(coords[count] - coords[count-1]) * gamma[count];

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

osg::Vec3 NaturalCubicSpline::calcTangentAt(float t)
{

   int i = (int) t;
   float u = t - i;

   float tangent_x = _polynomialsX[i].firstDerivCalcAt(u);
   float tangent_y = _polynomialsY[i].firstDerivCalcAt(u);
   float tangent_z = _polynomialsZ[i].firstDerivCalcAt(u);
   osg::Vec3 tangent(tangent_x,tangent_y,tangent_z); // y
   tangent.normalize();

   return tangent;
}

void NaturalCubicSpline::calcTangentCoordinateSystems(osg::ref_ptr<osg::Geode> root)
{
   // for(int i=0; i < _knots->getNumElements()-2; i++)
   // {
   //    for(int j = 0; j <=_curveSteps; j++)
   //    {
   //       float u = i + (j / (float) _curveSteps);
   //       root->addDrawable( calcTangentAt(u) );
   //    }
   // }

}

void NaturalCubicSpline::calcDoubleReflection(osg::ref_ptr<osg::Geode> root)
{
   osg::Vec4 x_axis(1,0,0,1);
   osg::Vec4 y_axis(0,1,0,1);
   osg::Vec4 z_axis(0,0,1,1);

   osg::ref_ptr<osg::Vec3Array> r = new osg::Vec3Array;
   osg::ref_ptr<osg::Vec3Array> s = new osg::Vec3Array;

   // ersten Frame vorgeben
   r->push_back( osg::Vec3(1,0,0) );
   s->push_back( osg::Vec3(0,1,0) );
   (*_tangents)[0] = osg::Vec3(0,0,1);

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

      osg::Matrix mat(
         (*r)[i+1].x(), (*s)[i+1].x(), (*_tangents)[i+1].x(), (*_vertices)[i+1].x(),
         (*r)[i+1].y(), (*s)[i+1].y(), (*_tangents)[i+1].y(), (*_vertices)[i+1].y(),
         (*r)[i+1].z(), (*s)[i+1].z(), (*_tangents)[i+1].z(), (*_vertices)[i+1].z(),
         0,           0,          0,       1
         );

      osg::Vec4 vec4 = osg::Vec4(
         (*_vertices)[i+1].x(),
         (*_vertices)[i+1].y(),
         (*_vertices)[i+1].z(),
         1
         );

      osg::ref_ptr<osg::Vec4Array>tangent_vertices = new osg::Vec4Array;

      tangent_vertices->push_back(vec4);
      tangent_vertices->push_back(mat*x_axis);

      tangent_vertices->push_back(vec4);
      tangent_vertices->push_back(mat*y_axis);

      tangent_vertices->push_back(vec4);
      tangent_vertices->push_back(mat*z_axis);

      osg::ref_ptr<osg::Geometry> tangent_geo = new osg::Geometry;

      tangent_geo->setVertexArray( tangent_vertices ); 
      tangent_geo->addPrimitiveSet( 
         new osg::DrawArrays(GL_LINES, 0, tangent_vertices->getNumElements()) );

      root->addDrawable( tangent_geo );

   }
}

osg::Geometry* NaturalCubicSpline::drawCylinderAlongSpline()
{

   return _geometry;
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

inline osg::Vec3 NaturalCubicSpline::calcAt(float t)
{
   int i = (int) t;
   float u = t - i;

   float x = _polynomialsX[i].calcAt(u);
   float y = _polynomialsY[i].calcAt(u);
   float z = _polynomialsZ[i].calcAt(u);

   return osg::Vec3(x,y,z);
}

void NaturalCubicSpline::calcSpline()
{
   if(_vertices)
      _vertices.release();
   _vertices = new osg::Vec3Array;

   if(_tangents)
      _tangents.release();
   _tangents = new osg::Vec3Array;

   for(int i=0; i < _knots->getNumElements()-1; i++)
   {
      for(int j = 0; j <=_curveSteps-1; j++)
      {
         float u = j / (float) _curveSteps;
         u += i;
         _vertices->push_back( calcAt(u) );
         _tangents->push_back( calcTangentAt(u) );
      }
   }
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
