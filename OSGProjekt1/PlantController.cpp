#include "PlantController.h"

PlantController::PlantController(osg::Group* root,
                                 unsigned int traversalMask)
    : _traversalMask(traversalMask),
      _root(root)
{
    
}

PlantController::~PlantController()
{
    
}

bool PlantController::handle( const osgGA::GUIEventAdapter& ea,
                              osgGA::GUIActionAdapter& aa )
{

    if( (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) &&
        (ea.getKey()       == 'p'))
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

            osgUtil::LineSegmentIntersector::Intersection& result =
                *(intersector->getIntersections().begin());
            
            osg::Vec3 hit_vec = result.getWorldIntersectPoint();

            osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
            trans->setMatrix( osg::Matrix().translate(hit_vec));
            RosePlant rose_plant( _traversalMask );
            trans->addChild( rose_plant.releasePlant() );

            _root->addChild( trans.release() );
        }
    }
    return false;
}
