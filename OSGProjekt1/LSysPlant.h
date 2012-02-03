#include <iostream>
#include <string>
#include <stdlib.h>
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
#include <osg/Texture2D>
#pragma once
using namespace std;


class LSysPlant
{
  public:

    LSysPlant(unsigned int       repeats, // Iterationen des L-Systems
              float              delta,   // Grundwinkel
              osg::Vec4          distanceVector, // Grundvektor für
                                                 // F/f Befehle
              osg::Matrix        rotMatrix,      // Grundrotation
              float              baseScale,      // Ausgangsdicke der Äste
              float              relativeLevelScale, // Prozentualer
                                                     // Abzug pro Level
              unsigned int       splineRes,          // Auflösung der Astsplines
              unsigned int       cylinderRes,        // Auflösung der
                                                     // extrudierten Zylinder
              unsigned int       translationJitter,  // zufällige
                                                     // Translation in Prozent
              unsigned int       rotationJitter,     // zufällige
                                                     // Rotation in Prozent
              osg::Image*        branchImg,          // Ast-Textur
              NaturalCubicSpline branchProfile,      // Ast Profil
              float              baseFlowerScale,    // Größe der Blüten
              float              relativeFlowerScale, // Abnahme pro Level
              FlowerGroup*       flower,              // Blütenobjekt
              vector<osg::ref_ptr<LeafGeode>> leavesGeodes, // Blatt-Modelle
              unsigned int       leavesLevel,               // Startlevel
                                                            // für Blätter
              unsigned int       leavesCount,               // Anzahl
                                                            // Blätter
                                                            // pro Ast
              float              leavesDistributionAngle,   // Verteilungswinkel
              float              leavesBaseScale,           // Ausgangsgröße
              float              leavesRelativeScale,       // Skalierung
                                                            // pro Level
              NaturalCubicSpline leavesProfile,             // Skalierung
                                                            // über Spline
              NaturalCubicSpline leavesSpline,              // Skalierungsspline
              map<char,string>   rules,                     // L-System Regeln
              string             startWord,                
              string             variable = "x"); // Variablenname für
                                                  // ffparser

    ~LSysPlant(void);
    void generatePlant();
    void generatePlantWord();
    osg::Group* buildPlant();
  
  protected:

    LSysPlant(){;}

    struct PlantStackElement
    {
      osg::Vec4 _posVector;         /* vorherige Position */
      osg::Matrix _rotMatrix;       /* vorherige Rotation */
      osg::Vec4 _distanceVector;    /* vorheriger Richtungsvektor */
      BranchNode* _parentBranch;    /* Zeiger auf den Eltern-Ast */
    };

    string getParanthesesContent(int str_pos, string str, int &para_end);
    inline void RotMatX(float angle, osg::Matrix &mat);
    inline void RotMatY(float angle, osg::Matrix &mat);
    inline void RotMatZ(float angle, osg::Matrix &mat);
    
    inline void randomizeRotation(float angle, char direction);
    inline void randomizeRotAllDirections(float angle);
    inline void randomizeTranslation(double &length);

    osg::ref_ptr<BranchNode> _firstBranch;
    
    unsigned int      _repeats;
    map<char, string> _rules;
    string            _startWord;
    string            _variable;

    unsigned int _translationJitter;
    unsigned int _rotationJitter;

    vector<osg::ref_ptr<LeafGeode>> _leavesGeodes;
    unsigned int       _leavesLevel;
    unsigned int       _leavesCount;
    float              _leavesDistributionAngle;
    float              _leavesBaseScale;
    float              _leavesRelativeScale;
    NaturalCubicSpline _leavesProfile;
    NaturalCubicSpline _leavesSpline;

    unsigned int _splineRes;
    unsigned int _cylinderRes;

    osg::ref_ptr<osg::Texture2D> _branchTex;
    NaturalCubicSpline _branchProfile;

    float       _delta;         // _delta -> increment angle
    osg::Vec4   _distanceVector;
    osg::Matrix _rotMatrix;
    float       _baseScale;          // Ausgangsdicke der Äste
    float       _relativeLevelScale; // Ab-/Zunahme der Astdicke je Level
    
    vector<PlantStackElement>           _stack;

    FunctionParser _fparser;
  
    osg::ref_ptr<FlowerGroup> _flower;
    float                     _baseFlowerScale;
    float                     _relativeFlowerScale;
    
};


