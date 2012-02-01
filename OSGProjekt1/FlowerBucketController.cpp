#include "FlowerBucketController.h"


FlowerBucketController::FlowerBucketController(osg::Group* root,
                                               unsigned int traversalMask,
                                               float pickExpansion
                                              )
    :_root(root),
     _hOffsetVec(osg::Vec3(0,0,0.01)),
     _traversalMask(traversalMask),
     _pickExpansion(pickExpansion),
     _drawRect(false),
     _currentGround(0)
{

}

FlowerBucketController::FlowerBucketController(osg::Group* root, 
                                               vector< osg::ref_ptr<osg::Geometry> > groundGeoms,
                                               unsigned int traversalMask,
                                               float pickExpansion)
    :_root(root),
     _hOffsetVec(osg::Vec3(0,0,0.01)),
     _traversalMask(traversalMask),
     _pickExpansion(pickExpansion),
     _drawRect(false),
     _groundGeoms( groundGeoms ),
     _currentGround(0)
{

}

FlowerBucketController::~FlowerBucketController()
{
    if(_pickGeom.get())
        _pickGeom.release();

    if(_pickVerts.get())
        _pickVerts.release();

    if(_rectGeom.get())
        _rectGeom.release();

    if(_rectVerts.get())
        _rectVerts.release();

}

inline void FlowerBucketController::setupRectangle(osg::Vec3 hitVec)
{
    if(_rectGeom.get())
        _rectGeom.release();
    _rectGeom = new osg::Geometry();

    if(_rectVerts.get())
        _rectVerts.release();
    _rectVerts = new osg::Vec3Array();

    _rectVerts->push_back( hitVec + _hOffsetVec );
    _rectVerts->push_back( hitVec + _hOffsetVec );
    _rectVerts->push_back( hitVec + _hOffsetVec );
    _rectVerts->push_back( hitVec + _hOffsetVec );

    _rectGeom->setVertexArray( _rectVerts.get() );
    _rectGeom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, _rectVerts->getNumElements()) );

    osg::ref_ptr<osg::Geode> rect_gd = new osg::Geode();
    rect_gd->addDrawable( _rectGeom.get() );

    osg::ref_ptr<osg::StateSet> state = rect_gd->getOrCreateStateSet();
    state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    state->setRenderBinDetails( 11, "RenderBin");

    _root->addChild( rect_gd.get() );

}

inline void FlowerBucketController::setupPickPlane(osg::Vec3 hitVec)
{
    if(_pickGeom.get())
        _pickGeom.release();
    _pickGeom = new osg::Geometry();

    if(_pickVerts.get())
        _pickVerts.release();
    _pickVerts = new osg::Vec3Array();

    osg::Vec3 expansion_vec( _pickExpansion, _pickExpansion, 0 );
    osg::Matrix mat90;
    mat90.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));

    for(int i=0; i<4; i++)
    {
        _pickVerts->push_back( hitVec + expansion_vec );
        expansion_vec = expansion_vec * mat90;
    }

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );

    _pickGeom->setVertexArray( _pickVerts.get() );
    _pickGeom->setColorArray( colors.get() );
    _pickGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
    _pickGeom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, _pickVerts->getNumElements()) );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode();
    gd->addDrawable( _pickGeom.get() );

    osg::StateSet* state = gd->getOrCreateStateSet();
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    state->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::BlendFunc* blend = new osg::BlendFunc;
    blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);

    _root->addChild( gd.get() );
}

inline void FlowerBucketController::removeHelper(osg::Geometry* geom)
{
    _root->removeChild( geom->getParent(0) );
}

bool FlowerBucketController::handle( const osgGA::GUIEventAdapter& ea,
                                     osgGA::GUIActionAdapter& aa )
{

    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

    if ( viewer )
    {
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
            new osgUtil::LineSegmentIntersector(
                osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

        osgUtil::IntersectionVisitor iv( intersector.get() );
        iv.setTraversalMask( ~_traversalMask );
        viewer->getCamera()->accept( iv );

        // wenn nichts geschnitten wurde abbrechen!
        if ( !(intersector->containsIntersections()) )
            return false;

        if( ea.getEventType() == osgGA::GUIEventAdapter::PUSH              &&
            ea.getButton()    == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
            (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_ALT))
        {

            // Start-Schnittpunkt suchen
            osgUtil::LineSegmentIntersector::Intersection& result =
                *(intersector->getIntersections().begin());

            for(int i=0; i < _groundGeoms.size(); i++)
                if(result.drawable == _groundGeoms[i])
                    _currentGround = _groundGeoms[i];

            if(_currentGround == 0)
                return false;

            osg::Vec3 hit_vec = result.getWorldIntersectPoint();
            setupRectangle( hit_vec );

            _drawRect = true;

            return false;
        }

        
        if(_currentGround == 0)
            return false;

        if( ea.getEventType() == osgGA::GUIEventAdapter::DRAG && _drawRect )
        {

            osgUtil::LineSegmentIntersector::Intersection& result =
                *(intersector->getIntersections().begin());

            if(result.drawable == _currentGround)
            {
                osg::Vec3 hit_vec = result.getWorldIntersectPoint();

                //           ^y
                //           |
                // 2---------1---------2
                // |         |         |
                // |         |         |
                // 3---------0---------3 -->x
                // |         |         |
                // |         |         |   0:push
                // 2---------1---------2   2:release
                osg::Vec3 origin_vec = (*_rectVerts)[0];
                (*_rectVerts)[2] = osg::Vec3(hit_vec.x(), hit_vec.y(), origin_vec.z()); // andere Ecke
                (*_rectVerts)[1] = osg::Vec3(origin_vec.x(), hit_vec.y(), origin_vec.z());
                (*_rectVerts)[3] = osg::Vec3(hit_vec.x(), origin_vec.y(), origin_vec.z());

                _rectGeom->dirtyDisplayList();
                _rectGeom->dirtyBound();

                return true;
            }
        }

        if( ea.getEventType() == osgGA::GUIEventAdapter::RELEASE           &&
            ea.getButton()    == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
            (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_ALT) )
        {
            osg::Vec3 draw_direction_vec = (*_rectVerts)[2] - (*_rectVerts)[0];
            osg::Vec3 new_origin;

            // die linke untere Ecke ist der Verschiebungsvektor;
            // folgende Fälle treten dabei auf:
            //           ^y
            //           |
            // 2---------1---------2
            // |         |         |
            // |         |         |
            // 3---------0---------3 -->x
            // |         |         |
            // |         |         |   0:push
            // 2---------1---------2   2:release
            if( (draw_direction_vec.x() > 0) && (draw_direction_vec.y() > 0) )
                new_origin = (*_rectVerts)[0];
            else if( (draw_direction_vec.x() > 0) && (draw_direction_vec.y() < 0) )
                new_origin = (*_rectVerts)[1];
            else if( (draw_direction_vec.x() < 0) && (draw_direction_vec.y() < 0) )
                new_origin = (*_rectVerts)[2];
            else if( (draw_direction_vec.x() < 0) && (draw_direction_vec.y() > 0) )
                new_origin = (*_rectVerts)[3];

            osg::BoundingBox bbox =
                (dynamic_cast<osg::Geode*> (_rectGeom->getParent(0)))->getBoundingBox();
            float width = bbox.xMax() - bbox.xMin();
            float depth = bbox.yMax() - bbox.yMin();

            osg::ref_ptr<FlowerBucket> fb = new FlowerBucket(width, depth);

            osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
            trans->setMatrix( osg::Matrix().translate( new_origin.x(),
                                                     new_origin.y(),
                                                     new_origin.z() ));
            
            FlowerBucketCtrlBase::_currentBucket = fb.get();

            trans->addChild( fb.release() );
            _root->addChild( trans.release() );

            _drawRect = false;
            _currentGround = 0;
            removeHelper( _rectGeom.release() );
        }

    }
    return false;
}

