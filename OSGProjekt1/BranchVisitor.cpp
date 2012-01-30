#include "BranchVisitor.h"

BranchVisitor::BranchVisitor(float baseScale,
                             float relativeLevelScale,
                             unsigned int splineRes,
                             unsigned int cylinderRes,
                             osg::Texture2D* branchTex,
                             NaturalCubicSpline* branchProfileSpline,
                             unsigned int leavesLevel,
                             unsigned int leavesCount,
                             float distributionAngle,
                             float leavesBaseScale,
                             float leavesRelativeScale,
                             NaturalCubicSpline* leavesRelativeScaleSpline,
                             NaturalCubicSpline* leavesSpline)
    : _baseScale(baseScale),
      _relativeLevelScale(relativeLevelScale),
      _splineRes(splineRes),
      _cylinderRes(cylinderRes),
      _branchTex(branchTex),
      _branchProfileSpline(branchProfileSpline),
      _level(0),
      _leavesLevel(leavesLevel),
      _leavesCount(leavesCount),
      _distributionAngle(distributionAngle),
      _leavesBaseScale(leavesBaseScale),
      _leavesRelativeScale(leavesRelativeScale),
      _leavesRelativeScaleSpline(leavesRelativeScaleSpline),
      _leavesSpline(leavesSpline)
{
    setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}

void BranchVisitor::apply( osg::Group& group )
{
    BranchNode* branch = dynamic_cast<BranchNode*>(&group);
    
    if(branch)
    {
        float base_scale = _baseScale * pow(_relativeLevelScale, (int)_level);

        branch->checkIfDirectDescendant( base_scale );

        // if(branch->isDirectDescendant())
        //     base_scale = branch->getParentBranch()->getLastScale();

        branch->buildBranch(base_scale,
                            _splineRes,
                            _cylinderRes,
                            _branchTex,
                            _branchProfileSpline
                           );
        
        if(_level >= _leavesLevel)
            branch->buildLeaves(_leavesCount,
                                _distributionAngle,
                                _leavesBaseScale * pow(_leavesRelativeScale, (int)_level),
                                _leavesRelativeScaleSpline,
                                _leavesSpline);
    }
    
    _level++;
    traverse( group );
    _level--;
}
