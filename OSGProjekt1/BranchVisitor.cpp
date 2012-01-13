#include "BranchVisitor.h"

void BranchVisitor::apply( osg::Group& group )
{
    BranchNode* branch = dynamic_cast<BranchNode*>(&group);
    
    if(branch)
    {
        cout << "level: " << _level << endl;
        branch->buildBranch();

    }
    
    _level++;
    traverse( group );
    _level--;
}
