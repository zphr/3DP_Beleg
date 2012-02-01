#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <osg/MatrixTransform>
using namespace std;

class ColorMarker
{
    public:
    ColorMarker(osg::Vec4 color, float pos)
        : _color(color), _pos(pos)
        {};
    ~ColorMarker(){};
    
    float _pos;
    osg::Vec4 _color;
    
};

class ColorGradient
{
  public:
    ColorGradient();
    ~ColorGradient();

    osg::Vec4 getColorAtPercent(float percent);
    void addColorMarkerAt(float pos, osg::Vec4 color);
    // bool sortMarkerByPos(ColorMarker a, ColorMarker b);
  protected:

    vector<ColorMarker> _colorMarkers;
};
