#include "BranchNode.h"


BranchNode::BranchNode(BranchNode* parentBranch,
                       osg::ref_ptr<osg::Vec4Array> knots,
                       FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes)
{
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array( (*knots) );

    _leavesGeodes = leavesGeodes;

    _geom = new osg::Geometry;
    
    _flower = flower;
}

BranchNode::BranchNode(FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes)
{
    _parentKnotIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;

    _leavesGeodes = leavesGeodes;

    _geom = new osg::Geometry;
    
    _flower = flower;
}

BranchNode::BranchNode(BranchNode* parentBranch,
                       osg::Vec4 startKnot,
                       int parentKnotIndex,
                       FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes)
{
    _parentKnotIndex = parentKnotIndex;
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array;
    _knots->push_back(startKnot);

    _leavesGeodes = leavesGeodes;

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
                                                         _flower.get(),
                                                         _leavesGeodes);
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

void BranchNode::buildLeaves(unsigned int leavesCount,
                             float distributionAngle,
                             NaturalCubicSpline* profileSpline,
                             NaturalCubicSpline* leavesSpline)
{
    if(_leavesGeodes.size() == 0)
        return;

    distributionAngle = osg::DegreesToRadians(distributionAngle);
    osg::ref_ptr<osg::Vec3Array> sp_verts;
    
    if(leavesSpline == 0)
    {
        osg::ref_ptr<osg::Vec4Array> spline_points = new osg::Vec4Array;
        spline_points->push_back(osg::Vec4(0.2,0.2,0,1));
        spline_points->push_back(osg::Vec4(1,0.9,0,1));
        NaturalCubicSpline spline(spline_points, leavesCount);
        
        sp_verts = new osg::Vec3Array( *(spline.getVertices()) );
    }
    else
    {
        leavesSpline->calcSpline(leavesCount);
        sp_verts = new osg::Vec3Array( *(leavesSpline->getVertices()) );
    }

    int vert_count = sp_verts->getNumElements();
    float profile_scale = 0;

    for(int i=0; i < vert_count; i++)
    {
        float t = (*sp_verts)[i].y();
        
        osg::ref_ptr<osg::MatrixTransform> transMat = new osg::MatrixTransform();

        if(profileSpline)
        {
            profile_scale = profileSpline->calcProfileAtPercent( i/(float) vert_count);
            transMat->postMult(osg::Matrix::scale(profile_scale, profile_scale, profile_scale));
        }
         
        transMat->postMult(osg::Matrix::rotate(distributionAngle*i, 0,0,1));
        transMat->postMult(_spline.calcFrameAt(t));
        transMat->addChild(_leavesGeodes[0]);
        addChild(transMat.release());
    }
}
