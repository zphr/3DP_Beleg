#include "BranchNode.h"


BranchNode::BranchNode()
{
    _firstKnotParentIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;

    _geom = new osg::Geometry;

    _index = 0;
    _this = this;
}

BranchNode::BranchNode(int firstKnotParentIndex)
{
    _firstKnotParentIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;

    _geom = new osg::Geometry;

    _this = this;
}

BranchNode::BranchNode(BranchNode* parentBranch, osg::Vec4 start_knot, int firstKnotParentIndex, int index)
{
    _firstKnotParentIndex = firstKnotParentIndex;
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array;
    _knots->push_back(start_knot);

    _geom = new osg::Geometry;

    _index = index;
    _this = this;
}

BranchNode::BranchNode(BranchNode* parentBranch, osg::ref_ptr<osg::Vec4Array> knots)
{
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array( (*knots) );

    _geom = new osg::Geometry;
    _this = this;
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
    delete _branchChildren.back();
    _branchChildren.pop_back();
}

BranchNode* BranchNode::getParentBranch()
{
    return _parentBranch;
}

void BranchNode::addPoint(osg::Vec4 new_point)
{
    _knots->push_back(new_point);
} 

BranchNode* BranchNode::addChild(int index, int i)
{
    osg::Vec4 branching_knot = (*_knots)[_knots->getNumElements()-1];

    _branchChildren.push_back( new BranchNode(this, branching_knot, index, i) );
    return (_branchChildren[_branchChildren.size()-1]);
}

inline int BranchNode::getChildrenCount()
{
    return _branchChildren.size();
}

inline BranchNode* BranchNode::getChild(unsigned int n)
{
    if( n < (_branchChildren.size()) )
         return _branchChildren[n];
    else 
         return 0;
}

struct BranchStElement
{
    BranchNode* branch_ptr;
    int position;
};

void BranchNode::traverseChildNodes()
{
    // if(_branchChildren.size() == 0)
    //      return;

    // vector<BranchStElement> node_stack;
    // BranchNode* curr_branch_child = this;
    // BranchStElement stack_element;
    
    // int children_count = 0, i = 0;

    // do
    // {
    //     if(( node_stack.size() != 0 ) && (curr_branch_child == 0))
    //     {
    //         BranchStElement last_stack_element;
    //         last_stack_element = node_stack[node_stack.size()-1];
    //         i = last_stack_element.position;
    //         curr_branch_child = last_stack_element.branch_ptr;

    //         if(curr_branch_child->getChildrenCount() < (i+1))
    //              node_stack.pop_back();
    //         else
    //              node_stack[node_stack.size()-1].position += 1;
            
    //         curr_branch_child = last_stack_element.branch_ptr->getChild(i);
    //         i = 0;
    //         continue;
    //     }

    //     if(curr_branch_child->getChildrenCount())
    //     {
    //         if(curr_branch_child->getChildrenCount() > (i+1))
    //         {
    //             BranchStElement new_stack_element;
    //             new_stack_element.branch_ptr = curr_branch_child;
    //             new_stack_element.position = i+1;
    //             node_stack.push_back( new_stack_element );
    //         }
    //         curr_branch_child = curr_branch_child->getChild(i);
    //     }
    //     else
    //          curr_branch_child = 0;
        
    // }while(( node_stack.size() != 0) || (curr_branch_child != 0));

}

inline vector<BranchNode*>* BranchNode::getChildrenPtr()
{
    return &_branchChildren;
}

void BranchNode::traverseChildNodesPerLevel(int level)
{
    // vector<vector<BranchNode*>*> curr_children;
    // curr_children.push_back(&_branchChildren);
    // int l = 0;
    // level > 0 ? l = 0 : l = -2;
    
    // while(curr_children.size() && (l < level))
    // {
    //     vector<vector<BranchNode*>*> new_curr_children;
        
    //     // Ausführen
    //     for(int i=0; i < curr_children.size(); i++)
    //     {
    //         for(int j=0; j < (*curr_children[i])->size(); j++)
    //         {
    //             // Kinder der nächsten Ebene sammeln
    //             if((*(*(curr_children[i])))[j].getChildrenPtr())
    //                  new_curr_children.push_back( (*(*(curr_children[i])))[j].getChildrenPtr() );
    //         }
    //     }

    //     level > 0 ? l++ : l = -2;
    //     curr_children = new_curr_children;
    // }

}

vector<vector<BranchNode*>*> BranchNode::getChildrenPerLevel(int level)
{
    vector<vector<BranchNode*>*> curr_children;
    curr_children.push_back(&_branchChildren);
    int l = 0, old_size = 0;

    // level hat Standardwert von -1 -> alle Kinder holen; alles
    // >0 nur bis zum jeweiligen Level Kinder abfragen
    level > 0 ? l = 0 : l = -2;

    old_size = curr_children.size();
    do
    {
        for(int i=0; i < curr_children.size(); i++)
        {
            
            for(int j=0; j < (curr_children[i])->size(); j++)
            {
                // Kinder der nächsten Ebene sammeln
                if( (*(curr_children[i]))[j]->getChildrenCount() )
                     curr_children.push_back( (*(curr_children[i]))[j]->getChildrenPtr() );
            }
        }
        level > 0 ? l++ : l = -2;
        // sind noch neue Kinder hinzugekommen?
        old_size = curr_children.size();
    }
    while((curr_children.size() != old_size) && (l < level));

    return curr_children;
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

osg::Geometry* BranchNode::calcBranch()
{
    _spline.setKnots( _knots.get() );

    // cout << _firstKnotParentIndex << endl;
    if(_parentBranch)
    {
        _spline.setFirstFrame(
            _parentBranch->getKnotFrameVectors( _firstKnotParentIndex )
        );
    }

    _spline.calcSpline();
    _geom = _spline.drawExtrudedCylinder(3, 0.15f);

    return _geom.get();
}

void BranchNode::calcBranches(osg::ref_ptr<osg::Geode> geode)
{
    vector<vector<BranchNode*>*> curr_children = getChildrenPerLevel();

    geode->addDrawable(calcBranch());

    for(int i=0; i < curr_children.size(); i++)
    {
        for(int j=0; j < (*(curr_children[i])).size(); j++)
        {
            geode->addDrawable(
                (*(curr_children[i]))[j]->calcBranch()
            );
        }
    }
}

