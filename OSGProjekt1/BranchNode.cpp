#include "BranchNode.h"


BranchNode::BranchNode(BranchNode* parentBranch,
                       osg::ref_ptr<osg::Vec4Array> knots,
                       FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes)
{
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array( (*knots) );
    
    _flower = flower;
    _leavesGeodes = leavesGeodes;

    _directDescendant = false;

    _geom = new osg::Geometry;
}

BranchNode::BranchNode(FlowerGroup* flower,
                       vector<osg::ref_ptr<LeafGeode>> leavesGeodes)
{
    _parentKnotIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;
    
    _flower = flower;
    _leavesGeodes = leavesGeodes;

    _directDescendant = false;

    _geom = new osg::Geometry;
    
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

    _flower = flower;
    _leavesGeodes = leavesGeodes;

    _directDescendant = false;

    _geom = new osg::Geometry;

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

void BranchNode::checkIfDirectDescendant(float &baseScale)
{
    if(_parentBranch)
        if((_parentBranch->getKnotCount() -1) == _parentKnotIndex)
            baseScale = _parentBranch->getLastScale();
}

// Skalierung des Astes am Ende, wichtig für direkte Nachfolgeräste,
// um nahtlos anzuschließen
float BranchNode::getLastScale()
{
    return _lastScale;
}

void BranchNode::buildBranch(float baseScale,
                             unsigned int splineRes,
                             unsigned int cylinderRes,
                             osg::Texture2D* branchTex,
                             NaturalCubicSpline* branchProfileSpline)
{
    _spline.setKnots( _knots.get() );

    if(_parentBranch)
            _spline.setFirstFrame
                (_parentBranch->getKnotFrameVectors( _parentKnotIndex ));

    if(branchProfileSpline)
        _lastScale = branchProfileSpline->calcProfileAtPercent( 1.0 ) * baseScale;

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable(_spline.buildExtrudedShape(cylinderRes,
                                               baseScale,
                                               branchProfileSpline));

    if(branchTex)
    {
        osg::StateSet* state = gd->getOrCreateStateSet();
        state->setTextureAttributeAndModes(0,branchTex,osg::StateAttribute::ON);
    }
        

    addChild( gd.release() );
}

void BranchNode::addFlower(osg::Matrix &mat, osg::Vec3 transVec, float scale)
{
    if(_flower.get() == 0)
        return;
    
    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    trans->postMult( osg::Matrix().scale( scale, scale, scale ) );
    trans->postMult( mat );
    trans->postMult( osg::Matrix().translate(transVec) );
    trans->addChild( _flower.get() );
    addChild( trans.release() );
}

void BranchNode::buildLeaves(unsigned int leavesCount,
                             float distributionAngle,
                             float leavesScale,
                             NaturalCubicSpline* leavesScaleSpline,
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
        leavesSpline->recalcSpline(leavesCount);
        sp_verts = new osg::Vec3Array( *(leavesSpline->getVertices()) );
    }

    int vert_count = sp_verts->getNumElements();
    float profile_scale = 0;

    for(int i=0; i < vert_count; i++)
    {
        float t = (*sp_verts)[i].y();
        
        osg::ref_ptr<osg::MatrixTransform> transMat = new osg::MatrixTransform();

        if(leavesScaleSpline)
        {
            profile_scale = leavesScaleSpline->calcProfileAtPercent( i/(float) vert_count);
            transMat->postMult(osg::Matrix::scale(profile_scale, profile_scale, profile_scale));
        }
        
        transMat->postMult(osg::Matrix::scale(leavesScale, leavesScale, leavesScale));
        transMat->postMult(osg::Matrix::rotate(distributionAngle*i, 0,0,1));
        transMat->postMult(_spline.calcFrameAt(t));
        transMat->addChild(_leavesGeodes[0]);
        addChild(transMat.release());
    }
}
