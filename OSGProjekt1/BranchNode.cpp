#include "BranchNode.h"


BranchNode::BranchNode()
{
    _firstKnotParentIndex = 0;
    _parentBranch = 0;
    _knots = new osg::Vec4Array;
    _hasLeafes = false;

    _geom = new osg::Geometry;

    _index = 0;
}

BranchNode::BranchNode(int firstKnotParentIndex)
{
    _firstKnotParentIndex = 0; 
    _parentBranch = 0;
    _knots = new osg::Vec4Array;
    _hasLeafes = false;

    _geom = new osg::Geometry;

}

BranchNode::BranchNode(BranchNode* parentBranch, osg::Vec4 start_knot, int firstKnotParentIndex, int index)
{
    _firstKnotParentIndex = firstKnotParentIndex;
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array;
    _knots->push_back(start_knot);
    _hasLeafes = false;

    _geom = new osg::Geometry;

    _index = index;
}

BranchNode::BranchNode(BranchNode* parentBranch, osg::ref_ptr<osg::Vec4Array> knots)
{
    _parentBranch = parentBranch;
    _knots = new osg::Vec4Array( (*knots) );
    _hasLeafes = false;

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
    delete _branchChildren.back();
    _branchChildren.pop_back();
}

BranchNode* BranchNode::getParentBranch()
{
    return _parentBranch;
}

void BranchNode::addKnot(osg::Vec4 new_point)
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

inline vector<BranchNode*>* BranchNode::getChildrenPtr()
{
    return &_branchChildren;
}

void BranchNode::traverseChildNodesPerLevel(int level)
{
    vector<vector<BranchNode*>*> curr_children;
    curr_children.push_back(&_branchChildren);
    int l = 0;
    level > 0 ? l = 0 : l = -2;
    
    while(curr_children.size() && (l < level))
    {
        vector<vector<BranchNode*>*> new_curr_children;
        
        // Ausführen
        for(int i=0; i < curr_children.size(); i++)
        {
            for(int j=0; j < curr_children[i]->size(); j++)
            {
                // Kinder der nächsten Ebene sammeln
                if( (*(curr_children[i]))[j]->getChildrenCount() )
                     curr_children.push_back( (*(curr_children[i]))[j]->getChildrenPtr() );
            }
        }

        level > 0 ? l++ : l = -2;
        curr_children = new_curr_children;
    }
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
            
            for(int j=0; j < curr_children[i]->size(); j++)
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

bool BranchNode::hasLeafes()
{
    return _hasLeafes;
}

void BranchNode::buildLeafes()
{
    
}
