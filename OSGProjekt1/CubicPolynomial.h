#include <osg/Geode>
#pragma once
using namespace std;

class CubicPolynomial
{
  public:
   /* vom niedrigen zum Koeffizienten des höchsten Grades */
   CubicPolynomial(osg::Vec4 coeffcients);
   ~CubicPolynomial();

   float calcAt(float t);
   float firstDerivCalcAt(float t);
   float secondDerivCalcAt(float t);

  protected:
   osg::Vec4 _coefficients;
   
};
