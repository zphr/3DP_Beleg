#include "SunController.h"

SunController::SunController( Sun* sun, unsigned int traversalMask)
    : _sun(sun),
      _traversalMask(traversalMask)
{
}

SunController::~SunController()
{
    
}

bool SunController::handle( const osgGA::GUIEventAdapter& ea,
                            osgGA::GUIActionAdapter& aa )
{

    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        osg::Matrix rot_mat;

         switch ( ea.getKey() )
        {
        case osgGA::GUIEventAdapter::KEY_Left:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(Sun::_rotationAngle),
                                           osg::Z_AXIS);

            _sun->setManualRotation(true);
            _sun->setLastRotationMatrix(rot_mat);
            _sun->preMult(rot_mat);
            break;
        case osgGA::GUIEventAdapter::KEY_Right:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(-Sun::_rotationAngle),
                                           osg::Z_AXIS);

            _sun->setManualRotation(true);
            _sun->setLastRotationMatrix(rot_mat);
            _sun->preMult(rot_mat);
            break;
        case osgGA::GUIEventAdapter::KEY_Up:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(Sun::_rotationAngle),
                                           osg::X_AXIS);

            _sun->setManualRotation(true);
            _sun->setLastRotationMatrix(rot_mat);
            _sun->preMult(rot_mat);
            break;
        case osgGA::GUIEventAdapter::KEY_Down:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(-Sun::_rotationAngle),
                                           osg::X_AXIS);

            _sun->setManualRotation(true);
            _sun->setLastRotationMatrix(rot_mat);
            _sun->preMult(rot_mat);
            break;
        case 'R':
            _sun->setMatrix( osg::Matrix() );
            return false;
        case 'M':
            _sun->toggleManualRotation();
            return false;
        }


    }
    return false;
}



// if((ea.getButton()    != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) ||
//    (ea.getEventType() != osgGA::GUIEventAdapter::PUSH)              ||
//    (ea.getModKeyMask()&osgGA::GUIEventAdapter::MODKEY_SHIFT) )
// {
//     _sun->setDraggerHandleEvents(false);
//     return false;
// }

// osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

// if ( viewer )
// {
//     osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
//         new osgUtil::LineSegmentIntersector(
//             osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

//     osgUtil::IntersectionVisitor iv( intersector.get() );
//     iv.setTraversalMask( ~_traversalMask );
//     viewer->getCamera()->accept( iv );
        
//     // wenn nichts geschnitten wurde abbrechen!
//     if ( !(intersector->containsIntersections()) )
//         return false;

//     osgUtil::LineSegmentIntersector::Intersections intersections =
//         intersector->getIntersections();

//     osgUtil::LineSegmentIntersector::Intersections::iterator itr =
//         intersections.begin();

//     for(; itr != intersections.end(); itr++)
//     {
//         osg::NodePath::iterator path_itr = itr->nodePath.begin();

//         for(; path_itr != itr->nodePath.end(); path_itr++)
//         {
//             osgManipulator::TrackballDragger* tb_dragger =
//                 dynamic_cast<osgManipulator::TrackballDragger*>(*path_itr);
//             if(tb_dragger)
//             {
//                 _sun->setDraggerHandleEvents(true);
//                 return true;
//             }
//         }
//     }
// }

