#include "FencePartController.h"

FencePartController::FencePartController(osg::Group* root,
                                         unsigned int traversalMask,
                                         FlowerBucket* flowerBucket)
    : _traversalMask(traversalMask)
{
    FlowerBucketCtrlBase::_currentBucket = flowerBucket;

    // _root = root
    osg::GraphicsContext::WindowingSystemInterface* wsi =
        osg::GraphicsContext::getWindowingSystemInterface();

    wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0),
                             _windowWidth, _windowHeight);

    osg::Projection* HUDProjectionMatrix = new osg::Projection;

    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,_windowWidth,0,_windowHeight));
      
    // For the HUD model view matrix use an identity matrix:
    HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph:
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    // Add the HUD projection matrix as a child of the root node
    // and the HUD model view matrix as a child of the projection matrix
    // Anything under this node will be viewed using this projection matrix
    // and positioned with this model view matrix.
    root->addChild(HUDProjectionMatrix);
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);


    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( FlowerBucketCtrlBase::_helperColor );

    _vertices = new osg::Vec3Array;
    _geom = new osg::Geometry();
    _geom->setVertexArray( _vertices.get() );    
    _geom->setColorArray( colors.get() );
    _geom->setColorBinding( osg::Geometry::BIND_OVERALL );
    _geom->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( _geom.get() );

    osg::StateSet* HUDStateSet = new osg::StateSet();
    gd->setStateSet(HUDStateSet);

    osg::LineWidth* line_width = new osg::LineWidth();
    line_width->setWidth(4.0f);
    gd->getOrCreateStateSet()->setAttributeAndModes(line_width,osg::StateAttribute::ON);

    HUDModelViewMatrix->addChild( gd.release() );

    // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    HUDStateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( 11, "RenderBin");

}

FencePartController::~FencePartController()
{
    _geom.release();
}

inline void FencePartController::resetGeometry()
{
    _vertices.release();
    _vertices = new osg::Vec3Array;
    _geom->setVertexArray( _vertices.get() );    
    _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( FlowerBucketCtrlBase::_helperColor );
    _geom->setColorArray( colors.get() );
    _geom->setColorBinding( osg::Geometry::BIND_OVERALL );

    _geom->dirtyDisplayList();
    _geom->dirtyBound();

}

bool FencePartController::handle( const osgGA::GUIEventAdapter& ea,
                                  osgGA::GUIActionAdapter& aa )
{
    switch ( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::RELEASE:
        
        if(ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
           ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
        {
            
            _vertices->push_back(osg::Vec3(ea.getX(), ea.getY(), 0));
            _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );
        
            _geom->dirtyDisplayList();
            _geom->dirtyBound();
        }
        
        if(ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON &&
           ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_CTRL )
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
                {
                    return false;
                }

                osgUtil::LineSegmentIntersector::Intersections intersections =
                    intersector->getIntersections();

                osgUtil::LineSegmentIntersector::Intersections::iterator itr =
                    intersections.begin();

                for(; itr != intersections.end(); itr++)
                {
                    osg::NodePath::iterator path_itr = itr->nodePath.begin();

                    for(; path_itr != itr->nodePath.end(); path_itr++)
                    {
                        FlowerBucket* fb = dynamic_cast<FlowerBucket*>(*path_itr);
                        if(fb)
                        {
                            FlowerBucketCtrlBase::_currentBucket = fb;
                            return false;
                        }
                        
                    }
                }

            }

        }
        break;
        
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch ( ea.getKey() )
        {
        case 'a': case 'A':
            {
                if(_geom->getVertexArray()->getNumElements() <= 2 )
                    break;

                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                colors->push_back( osg::Vec4(1,1,1,1) );
                _geom->setColorArray( colors.get() );
                _geom->setColorBinding( osg::Geometry::BIND_OVERALL );

                osg::ref_ptr<FencePart> fence_part = new FencePart(_geom);
                FlowerBucketCtrlBase::_currentBucket->setFencePart( fence_part.release() );

                resetGeometry();
                break;
            }
        case 'd': case 'D':
            resetGeometry();
            break;
        case 's': case 'S':
            FlowerBucketCtrlBase::_currentBucket->swapModels();
            break;
        }
    }
    
    return false;
}
