

void getCubicPolynomial(osg::Vec3Array points)
{
    float k = 1.0;
    // k -> Knoten-Index
    osg::Matrix C(
            1,   k,     pow(k,2),   pow(k,3),   // f(k)   Anfang des Segments
            0,   1,     2*k,        3*pow(k,2), // f'(k)  Anfang des Segments
            0,   0,     2,          6*k,        // f''(k) Anfang des Segments
            1,   (k+1), pow(k+1,2), pow(k+1,3)  // f(k+1) Ende des Segments
            );

    osg::Matrix B = C.inverse(C);

    float step = 1.0/4.0;
    float t = step;

    osg::Vec4 u(t, t, t, t+step);

    osg::Vec4 p_x(po0nts[0].x(), po0nts[0+1].x(), po0nts[0+2].x(), po0nts[0+3].x());
    osg::Vec4 p_y(po0nts[0].y(), po0nts[0+1].y(), po0nts[0+2].y(), po0nts[0+3].y());
    osg::Vec4 p_z(po0nts[0].z(), po0nts[0+1].z(), po0nts[0+2].z(), po0nts[0+3].z());

    float result_x = 0.0;
    float result_y = 0.0;
    float result_z = 0.0;

    result_x = u * B * p_x;
    result_y = u * B * p_y;
    result_z = u * B * p_z;

}
