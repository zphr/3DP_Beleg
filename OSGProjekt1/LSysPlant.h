#include <iostream>
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include "./fparser/fparser.hh"
#include "BranchNode.h"
#include "BranchVisitor.h"
#include "FlowerGroup.h"
#include "NaturalCubicSpline.h"

#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgUtil/SmoothingVisitor>
#pragma once
using namespace std;

struct PlantStackElement
{
    osg::Vec4 _posVector;         /* vorherige Position */
    osg::Matrix _rotMatrix;       /* vorherige Rotation */
    osg::Vec4 _distanceVector;    /* vorheriger Richtungsvektor */
    BranchNode* _parentBranch;    /* Zeiger auf den Eltern-Ast */
};

class LSysPlant
{
  public:

    LSysPlant(unsigned int repeats,
              float delta,
              osg::Vec4 distanceVector,
              osg::Matrix rotMatrix,
              FlowerGroup* flower,
              vector<osg::ref_ptr<LeafGeode>> leavesGeodes,
              unsigned int leavesLevel,
              unsigned int leavesCount,
              float leavesDistributionAngle,
              NaturalCubicSpline leavesProfile,
              NaturalCubicSpline leavesSpline,
              map<char,string> rules,
              string startWord,
              string variable = "x");
    ~LSysPlant(void);
    void generatePlant();
    void generatePlantWord();
    osg::Group* buildPlant();
  
  protected:
    string getParanthesesContent(int str_pos, string str, int &para_end);
    inline void RotMatX(float angle, osg::Matrix &mat);
    inline void RotMatY(float angle, osg::Matrix &mat);
    inline void RotMatZ(float angle, osg::Matrix &mat);
    
    osg::ref_ptr<BranchNode> _firstBranch;
    
    float _delta;       // _delta -> increment angle
    unsigned int _repeats;
    map<char, string> _rules;
    string _startWord;
    string _variable;
    
    vector<osg::ref_ptr<LeafGeode>> _leavesGeodes;
    unsigned int _leavesLevel;
    unsigned int _leavesCount;
    float _leavesDistributionAngle;
    NaturalCubicSpline _leavesProfile;
    NaturalCubicSpline _leavesSpline;
          
  
    osg::Vec4 _distanceVector;
    osg::Matrix _rotMatrix;
    vector<PlantStackElement> _stack;
    osg::ref_ptr<osg::Vec4Array> _vertices;
    osg::ref_ptr<osg::DrawElementsUInt> _indices;
  
    FunctionParser _fparser;
  
    osg::ref_ptr<FlowerGroup> _flower;
    
};


