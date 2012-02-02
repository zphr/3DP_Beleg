#pragma once
#include <iostream>

#include <osg/AutoTransform>
#include <osgManipulator/Dragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/ScaleAxisDragger>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/LineSegmentIntersector>

using namespace std;

class TrackballAxisDragger : public osgManipulator::CompositeDragger
{
    
public:
    TrackballAxisDragger(osg::Vec3 center, float boundRadius);
    void setupDefaultGeometry();

protected:
    osg::ref_ptr<osgManipulator::TrackballDragger> _rotDragger;
    osg::ref_ptr<osgManipulator::TranslateAxisDragger> _transDragger;
    osg::ref_ptr<osgManipulator::ScaleAxisDragger> _scaleDragger;
    osg::ref_ptr<osg::AutoTransform> _autoTransform;
    osg::ref_ptr<osg::MatrixTransform> _sizeTransform;

};
