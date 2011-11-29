#include "NaturalCubicSpline.h"

NaturalCubicSpline::NaturalCubicSpline(
   osg::ref_ptr<osg::Vec4Array> knots,
   int curveSteps)
   :  _knots(knots.get()), _curveSteps(curveSteps)
   
{
   _geometry = new osg::Geometry;
   calcCoefficientsXYZ();
   calcSpline();

}

// master test 3000
// master test 3000
// master test 3000
// master test 3000
// master test 3000
// master test 3000


NaturalCubicSpline::~NaturalCubicSpline()
{
   _vertices.release();
   _coefficientsX.release();
   _coefficientsY.release();
   _coefficientsZ.release();
}

void NaturalCubicSpline::calcCoefficientsXYZ()
{
   // Git Test 1 2 3
   
   float *coords_x = new float[_knots->getNumElements()];
   float *coords_y = new float[_knots->getNumElements()];
   float *coords_z = new float[_knots->getNumElements()];

   for(int i=0; i < _knots->getNumElements(); i++)
   {
      coords_x[i] = (*_knots)[i].x();
      coords_y[i] = (*_knots)[i].y();
      coords_z[i] = (*_knots)[i].z();
   }

   if(_coefficientsX)
      _coefficientsX.release();
   if(_coefficientsY)
      _coefficientsY.release();
   if(_coefficientsZ)
      _coefficientsZ.release();

   _coefficientsX = new osg::Vec4Array;
   _coefficientsY = new osg::Vec4Array;
   _coefficientsZ = new osg::Vec4Array;

   _coefficientsX = calcCoefficients(coords_x, _knots->getNumElements()-1);
   _coefficientsY = calcCoefficients(coords_y, _knots->getNumElements()-1);
   _coefficientsZ = calcCoefficients(coords_z, _knots->getNumElements()-1);

   delete coords_x;
   delete coords_y;
   delete coords_z;
}

osg::Vec4Array* NaturalCubicSpline::calcCoefficients(float *coords, int count)
{
   // basierend auf "An Introduction to Splines for Use in Computer Graphics
   // and Geometric Modeling"
   float *gamma = new float[count+1];
   float *delta = new float[count+1];
   float *derivative_result = new float[count+1];

   gamma[0] = 1/2.0;
   for(int i = 1; i < (count); i++)
   {
      gamma[i] = 1 / (4.0 - gamma[i-1]);
   }
   // das letzte gamma Element wird nur für die Ergebnisse verwendet die mit
   // diesem multipliziert werden
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

   osg::ref_ptr<osg::Vec4Array> coefficients = new osg::Vec4Array;

   for(int i = 0; i < (count); i++)
   {
      // Koeffizienten der Polynome berechnen
      coefficients->push_back( 
         osg::Vec4(
            coords[i],      // d
            derivative_result[i], // c
            3*(coords[i+1]-coords[i]) - 2*derivative_result[i] - derivative_result[i+1], // b
            2*(coords[i]-coords[i+1]) + derivative_result[i] + derivative_result[i+1] // a
            ));
   }

   delete gamma;
   delete delta;
   delete derivative_result;

   return coefficients.release();
}

void NaturalCubicSpline::calcSpline()
{
   if(_vertices)
      _vertices.release();
   _vertices = new osg::Vec3Array;

   for(int i=0; i < _knots->getNumElements()-1; i++)
   {
      for(int j = 0; j <=_curveSteps; j++)
      {
         float u = j / (float) _curveSteps;
         osg::Vec4 u_vec(1, u, pow(u,2.0f), pow(u,3.0f));

         float x = u_vec * (*_coefficientsX)[i];
         float y = u_vec * (*_coefficientsY)[i];
         float z = u_vec * (*_coefficientsZ)[i];

         _vertices->push_back(osg::Vec3(x,y,z));
      }
   }

   if(_geometry)
      _geometry.release();
   _geometry = new osg::Geometry;

   _geometry->setVertexArray( _vertices );
   _geometry->addPrimitiveSet( 
      new osg::DrawArrays(GL_LINE_STRIP, 0, _vertices->getNumElements()) );
}
