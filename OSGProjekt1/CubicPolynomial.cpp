#include "CubicPolynomial.h"

CubicPolynomial::CubicPolynomial(osg::Vec4 coefficients)
   : _coefficients(coefficients)
{
}

CubicPolynomial::~CubicPolynomial()
{
}

inline float CubicPolynomial::calcAt(float t)
{
   osg::Vec4 t_vec(1, t, pow(t,2.0f), pow(t,3.0f));
   return t_vec * _coefficients;
}

inline float CubicPolynomial::firstDerivCalcAt(float t)
{
   osg::Vec4 t_vec(0, 1.0, 2.0*t, 3.0*pow(t,2.0f));
   return t_vec * _coefficients;
}

inline float CubicPolynomial::secondDerivCalcAt(float t)
{
   osg::Vec4 t_vec(0, 0.0, 2.0, 6.0*t);
   return t_vec * _coefficients;
}

