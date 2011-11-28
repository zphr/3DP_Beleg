#include <osg/Geode>
#pragma once
using namespace std;

class CubicPolynomial
{
  public:
   /* "low" to "high" coeffcient */
   CubicPolynomial(osg::Vec4 coeffcients);
   ~CubicPolynomial();

   inline float calcAt(float t);
   inline float firstDerivCalcAt(float t);

  protected:
   osg::Vec4 _coefficients;
   
};
