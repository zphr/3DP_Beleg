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
