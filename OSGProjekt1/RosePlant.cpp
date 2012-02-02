#include "RosePlant.hpp"

const osg::ref_ptr<osg::Image> RosePlant::_stengelImg =
    osgDB::readImageFile("3d/Stengel.png");

RosePlant::RosePlant(unsigned int traversalMask)
    : _traversalMask(traversalMask)
{
    float delta = 22.5;
    osg::Vec4 dist (0.0, 0.0, 1.0, 1.0);
    osg::Matrix rot_mat;

    map<char, string> rules;
    // rules['S'] = "[A(x)]+(66)F(0.25)[A(1.0)]+(66)F(0.25)[A(1.0)]+(66)F(0.25)[A(1.0)]F(0.5)&(10.0)F(0.5)@";
    rules['S'] = "[A(x)]+(66)F(0.25)[A(1.0)]+(66)F(0.25)[A(1.0)]+(66)F(0.25)[A(1.0)]F(0.5)&(10.0)S";
    rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)@[S]";

    // rules['S'] = "[A(x)]+(66)[A(1.0)]+(66)[A(1.0)]+(66)[A(1.0)]F(0.5)&(10.0)F(0.5)@";
    // rules['A'] = "&(45)F(x/1.456)^F(x/1.456)^F(x/1.456)[S]";

    osg::ref_ptr<RoseLeaf> rose_leaf = new RoseLeaf();
    vector<osg::ref_ptr<LeafGeode>> leaf_list;
    leaf_list.push_back( rose_leaf.release() );

    osg::ref_ptr<osg::Vec4Array> spline_points = new osg::Vec4Array;
    spline_points->push_back(osg::Vec4(0.0,0.0,0,1));
    spline_points->push_back(osg::Vec4(0.8,1.3,0,1));
    spline_points->push_back(osg::Vec4(1,0.0,0,1));
    NaturalCubicSpline spline(spline_points, 3);

    osg::ref_ptr<osg::Vec4Array> profile_points = new osg::Vec4Array;
    profile_points->push_back(osg::Vec4(0, 1.0, 0, 1));
    profile_points->push_back(osg::Vec4(1, 0.5, 0, 1));
    NaturalCubicSpline profile(profile_points, 3);

    srand ( time(NULL) );

    _repeats                 = (rand() % 2)+1; 
    _delta                   = delta;
    _distanceVector          = dist;
    _rotMatrix               = rot_mat;
    _baseScale               = 0.06;
    _relativeLevelScale      = 0.70;
    _splineRes               = 2;
    _cylinderRes             = 3;
    _translationJitter       = 30;
    _rotationJitter          = 7;
    _branchProfile           = profile;
    _baseFlowerScale         = 0.75;
    _relativeFlowerScale     = 0.90;
    _flower                  = new RoseFlower();
    _leavesGeodes            = leaf_list;
    _leavesLevel             = 0;
    _leavesCount             = 5;
    _leavesDistributionAngle = 144.0;
    _leavesBaseScale         = 1.0;
    _leavesRelativeScale     = 0.85;
    _leavesProfile           = spline;
    _leavesSpline            = NaturalCubicSpline();
    _rules                   = rules;
    _startWord               = rules['S'];
    _variable                = 'x';

    _branchTex = new osg::Texture2D();
    _branchTex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    _branchTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    _branchTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    _branchTex->setImage( RosePlant::_stengelImg.get() );

    _firstBranch = new BranchNode(_flower.get(), _leavesGeodes);

    _plant = buildPlant();
    _plant->setNodeMask( traversalMask );
}

RosePlant::~RosePlant()
{

}

osg::Group* RosePlant::getPlant()
{
    return _plant.get();
}

osg::Group* RosePlant::releasePlant()
{
    return _plant.release();
}
