#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include "FlowerGroup.h"
#include "RoseFlower.h"
#include <iostream>
#include <deque>
#pragma once
using namespace std;

class BranchNode: public osg::Group
{
    public:
        BranchNode(BranchNode* parentBranch,
                   osg::ref_ptr<osg::Vec4Array> knots,
                   FlowerGroup* flower,
                   bool hasLeaves = false,
                   vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                   int leavesCount = 0,
                   int leavesLevelCount = -1
                    );
        
        BranchNode(FlowerGroup* flower,
                   vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                   int leavesCount = 0,
                   int leavesLevelCount = -1
                   );
        
        BranchNode(BranchNode* parentBranch,
                   osg::Vec4 startKnot,
                   int parentKnotIndex,
                   FlowerGroup* flower,
                   vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                   int leavesCount = 0,
                   int leavesLevelCount = -1
                   );
        
        ~BranchNode();
                   

        BranchNode* getParentBranch();

        /* Kind-Methoden */
        void deleteLastChild();
        inline int getChildrenCount();
        BranchNode* addChildBranch();
        void addFlower(osg::Matrix &mat);

        /* Knoten-Methoden */
        int getKnotCount();
        void addKnot(osg::Vec4 new_point);
        inline osg::Matrix getKnotFrame(int n);
        inline osg::Vec3Array* getKnotFrameVectors(int n);

        /* Ast-Geometrie Berechnung */
        void buildBranch();
        /* void buildBranches(); */
        
        void calcBranch();
        void buildLeaves();
        bool hasLeaves();

        int _index;
        int getLevel();

 protected:
        int _level;             /* Ebene des Astes im Baum */
        
        BranchNode* _parentBranch;

        NaturalCubicSpline _spline;
        osg::ref_ptr<osg::Vec4Array> _knots;

        int _parentKnotIndex;

        osg::ref_ptr<osg::Geometry> _geom;

        bool _hasLeaves;
        vector<osg::ref_ptr<LeafGeode>> _leavesGeodes;
        int _leavesCount;
        int _leavesLevelCount;  /* welche Kind-Ast-Level haben
                                   Blätter */
        
        osg::ref_ptr<FlowerGroup> _flower;
        
};
