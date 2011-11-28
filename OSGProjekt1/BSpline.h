class BSpline
{

public:
    
    double tstart;  // start value of the curve parameter
    double tend;    // end value of the curve parameter

    // stores the control points
    osg::ref_ptr<osg::Vec3Array> points;

    // The degree of the blending function.
    int degree;
    // The Drawing resolution. The curve is drawn with Resolution - 1 lines.
    int resolution;

    BSpline();
}
