#include "FlowerGroup.h"

FlowerGroup::FlowerGroup(
    osg::ref_ptr<osg::Geode> body,
    osg::ref_ptr<osg::Geode> stamen,      /* stamen -> Staubblätter */
    int stamenCount,
    float stamenStartRadius,
    float stamenEndRadius,
    vector<osg::ref_ptr<LeafGeode>> petalGeodes,
    float petalStartRadius,
    float petalEndRadius,
    int petalCount)
{
    _body = body;
    
    _stamen            = stamen;
    _stamenCount       = stamenCount;
    _stamenStartRadius = stamenStartRadius;
    _stamenEndRadius   = stamenEndRadius;

    _petalGeodes      = petalGeodes;
    _petalStartRadius = petalStartRadius;
    _petalEndRadius   = petalEndRadius;
    _petalCount       = petalCount;

    buildFlower();
}

FlowerGroup::~FlowerGroup()
{

}

void FlowerGroup::buildFlower()
{
    float alpha;
    float distributionAngle = 360.0 / _petalCount;
    distributionAngle = osg::DegreesToRadians(137.2);
    float r=_petalStartRadius;
    
    for(int i=1;
        r <= _petalEndRadius;
        i++, r = _petalStartRadius * sqrt((float)i))
    {

        float beta = 15.0 * sqrt((float)i);
        alpha = i * distributionAngle;

        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
        trans->preMult(osg::Matrix::rotate(osg::DegreesToRadians(beta), 0,1,0));
        trans->postMult(osg::Matrix::translate(r/sqrt((float)i),0,1));
        trans->postMult(osg::Matrix::rotate(alpha, 0,0,1));
        trans->addChild(_petalGeodes[0]);
        getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        insertChild(0, trans);
        // addChild(trans);
        trans.release();                            

    }

}


