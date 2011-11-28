#include <iostream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include "./fparser/fparser.hh"

#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#pragma once
using namespace std;

struct PlantStackElement
{
  int old_branch, old_index;
  osg::Vec4 old_vec;
  osg::Matrix _rotMatrix;
  osg::Vec4 _distanceVector;
};

class LSysPlant
{
 public:
  float _delta;       // _delta -> increment angle
  unsigned int _repeats;
  map<char, string> _rules;
  string _startWord;
  string _variable;

  osg::Vec4 _distanceVector;
  osg::Matrix _rotMatrix;
  vector<PlantStackElement> _stack;
  osg::ref_ptr<osg::Vec4Array> _vertices;
  osg::ref_ptr<osg::DrawElementsUInt> _indices;
  vector<osg::ref_ptr<osg::Vec4Array>> _branches;
  FunctionParser _fparser;


  LSysPlant(unsigned int repeats,
            float delta,
            map<char,string> rules,
            string startWord,
            osg::Vec4 distanceVector,
            osg::Matrix rotMatrix,
            string variable = "x");
  ~LSysPlant(void);
  void generatePlant();
  void generatePlantWord();
  void drawPlant(osg::ref_ptr<osg::Geode> &node);
  inline void RotMatX(float angle, osg::Matrix &mat);
  inline void RotMatY(float angle, osg::Matrix &mat);
  inline void RotMatZ(float angle, osg::Matrix &mat);
  inline float DegToRad(float angle);
  int getParanthesesContent(int str_pos, string str);
};

