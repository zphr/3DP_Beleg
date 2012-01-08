#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "NaturalCubicSpline.h"
#include "LeafGeode.h"
#include <iostream>
#include <deque>
#pragma once
using namespace std;

class BranchNode
{
    public:
        BranchNode(BranchNode* parentBranch,
                   osg::ref_ptr<osg::Vec4Array> knots,
                   bool hasLeaves = false,
                   vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                   int leavesCount = 0
                    );
        BranchNode(
                vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                int leavesCount = 0
                );
        BranchNode(BranchNode* parentBranch,
                   osg::Vec4 start_knot,
                   int firstKnotParentIndex,
                   int index,
                   vector<osg::ref_ptr<LeafGeode>> leavesGeodes = vector<osg::ref_ptr<LeafGeode>>(),
                   int leavesCount = 0
                   );
        ~BranchNode();

        BranchNode* getParentBranch();

        /* Traversierung der Baumstruktur */
        void traverseChildNodesPerLevel(int level=-1);
        vector<vector<BranchNode*>*> getChildrenPerLevel(int level=-1);

        /* Kind-Methoden */
        void deleteLastChild();
        inline int getChildrenCount();
        BranchNode* addChild(int index, int i);
        inline BranchNode* getChild(unsigned int n);
        inline vector<BranchNode*>* getChildrenPtr();

        /* Knoten-Methoden */
        int getKnotCount();
        void addKnot(osg::Vec4 new_point);
        inline osg::Matrix getKnotFrame(int n);
        inline osg::Vec3Array* getKnotFrameVectors(int n);

        /* Ast-Geometrie Berechnung */
        osg::Geometry* calcBranch();
        void calcBranches(osg::ref_ptr<osg::Geode> geode);

        int _index;

        bool hasLeaves();
        osg::Group* buildLeaves();

    private:
        BranchNode* _parentBranch;
        vector<BranchNode*> _branchChildren;

        NaturalCubicSpline _spline;
        osg::ref_ptr<osg::Vec4Array> _knots;

        int _firstKnotParentIndex;

        osg::ref_ptr<osg::Geometry> _geom;

        bool _hasLeaves;
        vector<osg::ref_ptr<LeafGeode>> _leavesGeodes;
        int _leavesCount;
};
