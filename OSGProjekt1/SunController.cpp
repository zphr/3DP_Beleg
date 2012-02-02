#include "SunController.h"

SunController::SunController( Sun* sun)
    : _sun(sun)
{
}

SunController::~SunController()
{
    
}

bool SunController::handle( const osgGA::GUIEventAdapter& ea,
                            osgGA::GUIActionAdapter& aa )
{

    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN &&
        ea.getKey())
    {
        osg::Matrix rot_mat;

         switch ( ea.getKey() )
        {
        case osgGA::GUIEventAdapter::KEY_Left:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(Sun::_rotationAngle),
                                           osg::Z_AXIS);
            break;
        case osgGA::GUIEventAdapter::KEY_Right:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(-Sun::_rotationAngle),
                                           osg::Z_AXIS);
            // rot_mat.preMult( _sun->getLastRotationMatrix() );
            break;
        case osgGA::GUIEventAdapter::KEY_Up:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(Sun::_rotationAngle),
                                           osg::X_AXIS);
            _sun->setLastRotationMatrix(rot_mat);

            break;
        case osgGA::GUIEventAdapter::KEY_Down:
            rot_mat = osg::Matrix().rotate(osg::DegreesToRadians(-Sun::_rotationAngle),
                                           osg::X_AXIS);
            _sun->setLastRotationMatrix(rot_mat);
            break;
        case 'R':
            _sun->setMatrix( osg::Matrix() );
            return false;
        case 'M':
            _sun->toggleManualRotation();
            return false;
        }

         _sun->preMult(rot_mat);

    }
    return false;
}
