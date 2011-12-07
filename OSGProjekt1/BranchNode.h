#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include "NaturalCubicSpline.h"
#include <iostream>
#pragma once
using namespace std;

class BranchNode
{
    public:
        BranchNode();
        BranchNode(int firstKnotParentIndex);
        BranchNode(BranchNode* parentBranch, osg::Vec4 start_knot, int firstKnotParentIndex, int index);
        BranchNode(BranchNode* parentBranch, osg::ref_ptr<osg::Vec4Array> knots);
        ~BranchNode();

        inline BranchNode* getParentBranch();
        void addPoint(osg::Vec4 new_point);
        BranchNode* addChild(int index, int i);
        void traverseChildNodes();
        void traverseChildNodesPerLevel(int level=-1);
        vector<vector<BranchNode>*> getChildrenPerLevel(int level=-1);
        inline int getChildrenCount();
        inline BranchNode* getChild(unsigned int n);
        inline vector<BranchNode>* getChildrenPtr();
        void calcBranches(osg::ref_ptr<osg::Geode> geode);

        inline osg::Matrix getKnotFrame(int n);
        int getKnotCount();
        inline osg::Vec3Array* getKnotFrameVectors(int n);

        inline void setGeometry( osg::ref_ptr<osg::Geometry> geom );
        inline osg::Geometry* getGeometry();
        osg::Geometry* calcBranch();

    private:
        BranchNode* _parentBranch;
        vector<BranchNode> _branchChildren;

        NaturalCubicSpline _spline;
        osg::ref_ptr<osg::Vec4Array> _knots;

        int _firstKnotParentIndex;
        int _index;

        osg::ref_ptr<osg::Geometry> _geom;
};