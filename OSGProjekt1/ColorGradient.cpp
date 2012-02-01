#include "ColorGradient.h"

ColorGradient::ColorGradient()
{

}

ColorGradient::~ColorGradient()
{

}

osg::Vec4 ColorGradient::getColorAtPercent( float percent )
{
    ColorMarker *min_color = 0;
     ColorMarker *max_color = 0;

    for(int i=0; i < _colorMarkers.size(); i++)
    {
        if(_colorMarkers[i]._pos >= percent)
        {
            max_color = &(_colorMarkers[i]);
            if(i == 0)
                min_color = &(_colorMarkers[i]);
            else
                min_color = &(_colorMarkers[i-1]);
            
            percent = (percent - min_color->_pos) * (1.0/(max_color->_pos - min_color->_pos));
            osg::Vec4 mix_color = min_color->_color * (1.0 - percent)
                + max_color->_color * percent;
            return mix_color;

        }
    }

    return _colorMarkers[_colorMarkers.size()-1]._color;
}

bool sortMarkerByPos(ColorMarker a, ColorMarker b)
{
    return (a._pos < b._pos);
}

void ColorGradient::addColorMarkerAt(float pos, osg::Vec4 color)
{
    if(pos > 1.0)
        pos = pos - ((int) pos/1);

    _colorMarkers.push_back(ColorMarker(color, pos));
    sort( _colorMarkers.begin(), _colorMarkers.end(), sortMarkerByPos);
}
