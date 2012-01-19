#include "CircleCurve.h"

CircleCurve::CircleCurve(int resolution)
{
    _resolution = resolution;
}

CircleCurve::~CircleCurve()
{
}

osg::Vec4Array* CircleCurve::calcPoints(unsigned int resolution)
{
    float x, y;
    float PI_step = (2*M_PI)/(float)resolution;
    float PI_2 =  2.0f*M_PI;

    osg::Vec4 circle_vert;
    osg::ref_ptr<osg::Vec4Array> cylinder_verts = new osg::Vec4Array;

    for(float j=0; j <= (PI_2); j+=PI_step)
    {
        x = cos(j);
        y = sin(j);

        circle_vert = osg::Vec4(x, y, 0, 1);
        circle_vert[3] = 1;

        cylinder_verts->push_back(circle_vert);
    }

    return cylinder_verts.release();
}

osg::Geometry* CircleCurve::buildMeshAlongPath(unsigned int resolution,
                                               float scale,
                                               const vector<osg::Matrix> &matrices,
                                               const osg::ref_ptr<osg::Vec3Array> &vertices,
                                               const vector<float> &profileScale)
{
    
    unsigned int elements = vertices->getNumElements();
    int k = 0;

    osg::ref_ptr<osg::Vec4Array> shape_verts = calcPoints(resolution);

    osg::Vec4 vert;
    osg::ref_ptr<osg::Vec4Array> verts = new osg::Vec4Array;

    osg::Vec3 normal;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    float tex_width  = (1.0 / (resolution));
    float tex_height = (1.0 / (elements-1));
    
    resolution++;
    
    osg::ref_ptr<osg::Vec2Array> texc = new osg::Vec2Array;
    for(int i=0; i < elements; i++)
    {
        for(int j=0; j < shape_verts->getNumElements(); j++)
        {

            vert = osg::Vec4((*shape_verts)[j]);
            vert *= scale;

            if(profileScale.size() == elements)
                vert *= profileScale[i];

            vert[3] = 1;
            vert =  vert * matrices[i];
            verts->push_back(vert);

            texc->push_back(osg::Vec2((tex_width * j), (tex_height * i)));
            
            normal = osg::Vec3(vert.x(), vert.y(), vert.z());
            // Normalen müssen vom Ursprung aus angegeben werden
            normal -= (*vertices)[i];
            normal.normalize();
            normals->push_back(normal);

            if((i < (elements-1)) && (j < (resolution-1)))
            {
                face_indices->push_back( i * resolution + j );
                face_indices->push_back( i * resolution + (j + 1));
                face_indices->push_back( (i+1) * resolution + (j + 1));
                face_indices->push_back( (i+1) * resolution + j);
            }
        }
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray( verts.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->setTexCoordArray(0,texc.get());
    geom->addPrimitiveSet( face_indices.get() );

    return geom.release();
}
