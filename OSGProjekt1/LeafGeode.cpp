#include "LeafGeode.h"

LeafGeode::LeafGeode()
{

}

LeafGeode::LeafGeode(NaturalCubicSpline spline,
                     int resolution,
                     float scale,
                     osg::Image* image)
{
    _spline = spline;

    setTexture(image);
    addDrawable( _spline.buildExtrudedShape(resolution, scale) );
}

LeafGeode::LeafGeode(NaturalCubicSpline spline, NaturalCubicSpline extrudeShape,
                     int resolution, float scale, osg::Image* image)
{
    _spline = spline;
    _extrudeShape = extrudeShape;
    _spline.setExtrudeShape( &_extrudeShape );

    setTexture(image);
    addDrawable( _spline.buildExtrudedShape(resolution, scale) );
}

LeafGeode::~LeafGeode()
{
     
}

inline void LeafGeode::setTexture(osg::Image* image)
{
    if(image)
    {
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
        texture->setImage(image);
 
        osg::StateSet* state = getOrCreateStateSet();
        state->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
        
        state->setMode(GL_BLEND, osg::StateAttribute::ON);

        osg::BlendFunc* blend = new osg::BlendFunc;
        blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        
        state->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
	
        alphaFunc->setFunction(osg::AlphaFunc::GREATER,0.6f);
        state->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );
        
    }
}
