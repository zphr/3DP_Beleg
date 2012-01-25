#include "FencePartController.h"

FencePartController::FencePartController(FlowerBucket* root)
{
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
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
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

    _vertices = new osg::Vec3Array;
    _geom = new osg::Geometry();
    _geom->setVertexArray( _vertices.get() );    
    _geom->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( _geom.get() );

    HUDModelViewMatrix->addChild( gd.get() );

    osg::StateSet* HUDStateSet = new osg::StateSet();
    gd->setStateSet(HUDStateSet);

    // Disable depth testing so geometry is draw regardless of depth values
    // of geometry already draw.
    HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // Need to make sure this geometry is draw last. RenderBins are handled
    // in numerical order so set bin number to 11
    HUDStateSet->setRenderBinDetails( 11, "RenderBin");

    _root = root;
}

FencePartController::~FencePartController()
{
    _geom.release();
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
            cout << ea.getX() << " " << ea.getY() << endl;
            
            _vertices->push_back(osg::Vec3(ea.getX(), ea.getY(), 0));
            _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );
        
            _geom->dirtyDisplayList();
            _geom->dirtyBound();
        }
        break;
    case osgGA::GUIEventAdapter::KEYDOWN:
        switch ( ea.getKey() )
        {
        case 'a': case 'A':
            {
                osg::ref_ptr<FencePart> fence_part = new FencePart(_geom);
                _root->setFencePart( fence_part.release() );
                break;
            }
        case 'd': case 'D':
            {
                _vertices.release();
                _vertices = new osg::Vec3Array;
                _geom->setVertexArray( _vertices.get() );    
                _geom->setPrimitiveSet(0, new osg::DrawArrays(GL_LINE_LOOP, 0, _vertices->getNumElements()) );
                
                _geom->dirtyDisplayList();
                _geom->dirtyBound();

            }
        }
    }
    return false;
}
