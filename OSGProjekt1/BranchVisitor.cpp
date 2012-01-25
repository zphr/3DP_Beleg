#include "BranchVisitor.h"

BranchVisitor::BranchVisitor(unsigned int leavesLevel,
                             unsigned int leavesCount,
                             float distributionAngle,
                             NaturalCubicSpline* profileSpline,
                             NaturalCubicSpline* leavesSpline)
  : _level(0),
    _leavesLevel(leavesLevel),
    _leavesCount(leavesCount),
    _distributionAngle(distributionAngle),
    _profileSpline(profileSpline),
    _leavesSpline(leavesSpline)
{
    setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}

void BranchVisitor::apply( osg::Group& group )
{
    BranchNode* branch = dynamic_cast<BranchNode*>(&group);
    
    if(branch)
    {
        // cout << "level: " << _level << endl;
        branch->buildBranch();
        
        if(_level >= _leavesLevel)
            branch->buildLeaves(_leavesCount,
                                _distributionAngle,
                                _profileSpline,
                                _leavesSpline);
    }
    
    _level++;
    traverse( group );
    _level--;
}
