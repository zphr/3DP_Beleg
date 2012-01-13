#include "BranchNode.h"


BranchNode::BranchNode(BranchNode* parentBranch,
                       osg::ref_ptr<osg::Vec4Array> knots,
                       FlowerGroup* flower,
                       bool hasLeaves,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes, 
                       int leavesCount,
                       int leavesLevelCount)
{
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array( (*knots) );

    _hasLeaves = hasLeaves;
    _leavesGeodes = leavesGeodes;
    _leavesCount = leavesCount;
    _leavesLevelCount = leavesLevelCount;

    _geom = new osg::Geometry;
    
    _flower = flower;
}

BranchNode::BranchNode(FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes,
                       int leavesCount,
                       int leavesLevelCount)
{
    _parentKnotIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;

    _hasLeaves = false;
    _leavesGeodes = leavesGeodes;
    _leavesCount = leavesCount;
    _leavesLevelCount = leavesLevelCount;

    _geom = new osg::Geometry;
    
    _flower = flower;
}

BranchNode::BranchNode(BranchNode* parentBranch,
                       osg::Vec4 startKnot,
                       int parentKnotIndex,
                       FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes,
                       int leavesCount,
                       int leavesLevelCount)
{
    _parentKnotIndex = parentKnotIndex;
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array;
    _knots->push_back(startKnot);

    _hasLeaves = false;
    _leavesGeodes = leavesGeodes;
    _leavesCount = leavesCount;
    _leavesLevelCount = leavesLevelCount;

    _geom = new osg::Geometry;
    
    _flower = flower;
}

BranchNode::~BranchNode()
{
    if(_knots)
        _knots.release();

    if(_geom)
        _geom.release();
    
}

int BranchNode::getLevel()
{
    return _level;
}

void BranchNode::deleteLastChild()
{
    removeChild(getNumChildren()-1);
}

BranchNode* BranchNode::getParentBranch()
{
    return _parentBranch;
}

void BranchNode::addKnot(osg::Vec4 new_point)
{
    _knots->push_back(new_point);
} 

BranchNode* BranchNode::addChildBranch()
{
    osg::Vec4 branching_knot = (*_knots)[_knots->getNumElements()-1];
    int parentKnotIndex = _knots->getNumElements()-1;
    
    osg::ref_ptr<BranchNode> new_branch = new BranchNode(this,
                                                         branching_knot,
                                                         parentKnotIndex,
                                                         _flower.get());
    addChild( new_branch );
    
    return ( new_branch.release() );
}

int BranchNode::getKnotCount()
{
    return _knots->getNumElements();
}

inline osg::Vec3Array* BranchNode::getKnotFrameVectors(int n)
{
    return _spline.getKnotFrameVectors(n);
}

inline osg::Matrix BranchNode::getKnotFrame(int n)
{
    return _spline.getKnotFrame(n);
}

void BranchNode::buildBranch()
{
    calcBranch();
    buildLeaves();
}

void BranchNode::calcBranch()
{
    _spline.setKnots( _knots.get() );

    // cout << _parentKnotIndex << endl;
    if(_parentBranch)
        {
            _spline.setFirstFrame(
                _parentBranch->getKnotFrameVectors( _parentKnotIndex )
            );
        }

    _spline.calcSpline();
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable(_spline.drawExtrudedCylinder(3, 0.15f));
    addChild( gd.release() );
}

void BranchNode::addFlower(osg::Matrix &mat)
{
    if(_flower.get() == 0)
        return;
    
    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform(mat);
    trans->setMatrix( mat );
    trans->addChild( _flower.get() );
    addChild( trans.release() );
}

bool BranchNode::hasLeaves()
{
    return _hasLeaves;
}

void BranchNode::buildLeaves()
{
    if(_leavesGeodes.size() == 0)
        return;

    float distributionAngle = osg::DegreesToRadians(144.0);
    
    osg::ref_ptr<osg::Vec4Array> spline_points = new osg::Vec4Array;
    spline_points->push_back(osg::Vec4(0.2,0.2,0,1));
    // spline_points->push_back(osg::Vec4(0.5,0.25,0,1));
    spline_points->push_back(osg::Vec4(1,0.9,0,1));

    NaturalCubicSpline spline(spline_points, _leavesCount);
    osg::ref_ptr<osg::Vec3Array> sp_verts(spline.getVertices());

    for(int i=0; i < sp_verts->getNumElements(); i++)
        {
            float t = (*sp_verts)[i].y();
            
            osg::ref_ptr<osg::MatrixTransform> transMat = new osg::MatrixTransform();

            transMat->postMult(osg::Matrix::rotate(distributionAngle*i, 0,0,1));
            transMat->postMult(_spline.calcFrameAt(t));
            transMat->addChild(_leavesGeodes[0]);
            addChild(transMat.release());
        }
}
