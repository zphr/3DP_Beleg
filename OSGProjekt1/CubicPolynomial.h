#include <osg/Geode>
#pragma once
using namespace std;

class CubicPolynomial
{
  public:
   /* vom niedrigen zum Koeffizienten des höchsten Grades */
   CubicPolynomial(osg::Vec4 coeffcients);
   ~CubicPolynomial();

   inline float calcAt(float t);
   inline float firstDerivCalcAt(float t);
   inline float secondDerivCalcAt(float t);

  protected:
   osg::Vec4 _coefficients;
   
};
