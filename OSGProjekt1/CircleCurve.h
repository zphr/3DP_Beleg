#include "BaseCurve.h"

class CircleCurve: public BaseCurve
{
    public:
         CircleCurve(int resolution=3);
         ~CircleCurve();

         osg::Vec4Array* calcPoints(unsigned int resolution);
         osg::Geometry* buildMeshAlongPath(unsigned int resolution,
                                        float scale,
                                        const vector<osg::Matrix> &matrices,
                                        const osg::ref_ptr<osg::Vec3Array> &vertices,
                                        const vector<float> &profile_scale = vector<float>());
};
