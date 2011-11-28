
public BSpline::BSpline()
{
    points = new osg::Vec3Array;
    degree=3;
    resolution=40;
}

public BSpline::~BSpline() {}

/// It returns the curve point at position t.
/// <param name="t">curve position</param>
osg::Vec3 BSpline::CalculateCurvePointAt(double t)
{
    osg::Vec3 vec;
    double n;

    for (int i=0; i<points->getNumElements(); i++)  
    {
        n=BlendingFunction(i,degree,t);     
        vec += points[i] * n;
    } 

    return vec;

}

void BSpline::DrawCurve()
{
    Point[] drawingpoints = new Point[resolution+1];

    /// "tend" = end value of the curve parameter
    double tstep = (tend-tstart)/(resolution); 

    /// tstart = start value of the curve parameter
    double t=tstart;

    for(int i=0; i < resolution+1; i++)
    {
        drawingpoints[i]=CalculateCurvePointAt(t);
        t=Math.Min(t+tstep,tend);
    }

    g.DrawLines(new Pen(color,width),drawingpoints);
}

