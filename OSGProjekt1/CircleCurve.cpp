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

osg::Geometry* CircleCurve::buildMeshAlongPath(unsigned int extrudeShapeRes,
                                               float scale,
                                               const vector<osg::Matrix> &matrices,
                                               const osg::ref_ptr<osg::Vec3Array> &vertices,
                                               BaseCurve* profileCurve)
{

    unsigned int spline_vert_count = vertices->getNumElements();
    int k = 0;

    //   ^  ^  ^     ^
    //   +--+--+ ... +
    //   |  |  |     |
    //   0--0--0--0--0->
    //   |  |  |     |   0: vertices
    //   +--+--+ ... +   +: extrude_verts
    osg::ref_ptr<osg::Vec4Array> extrude_verts = calcPoints(extrudeShapeRes);

    osg::Vec4 vert;
    osg::ref_ptr<osg::Vec3Array> new_verts = new osg::Vec3Array;

    osg::Vec3 normal;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    float tex_width  = (1.0 / (extrudeShapeRes));
    float tex_height = (1.0 / (spline_vert_count-1));

    float tile_u = 1.0;
    float tile_v = 3.0;

    extrudeShapeRes++;          // den Endpunkt noch mit beachten

    float profile_scale = 1.0; 

    osg::ref_ptr<osg::Vec2Array> texc = new osg::Vec2Array;
    
    for(int i=0; i < spline_vert_count; i++)
    {

         if(profileCurve)
            profile_scale = profileCurve->calcProfileAtPercent(
                i / (float)(spline_vert_count-1));

        for(int j=0; j < extrude_verts->getNumElements(); j++)
        {

            vert = osg::Vec4((*extrude_verts)[j]);
            vert *= scale;
            vert *= profile_scale;

            vert[3] = 1;
            vert =  vert * matrices[i];
            new_verts->push_back( osg::Vec3(vert.x(), vert.y(), vert.z()) );

            texc->push_back(osg::Vec2((tex_width * j)  * tile_u,
                                      (tex_height * i) * tile_v));

            normal = osg::Vec3(vert.x(), vert.y(), vert.z());
            // Normalen vom Ursprung aus angeben
            normal -= (*vertices)[i];
            normal.normalize();
            normals->push_back(normal);

            if((i < (spline_vert_count-1)) && (j < (extrudeShapeRes-1)))
            {
                face_indices->push_back( i * extrudeShapeRes + j );
                face_indices->push_back( i * extrudeShapeRes + (j + 1));
                face_indices->push_back( (i+1) * extrudeShapeRes + (j + 1));
                face_indices->push_back( (i+1) * extrudeShapeRes + j);
            }
        }
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    geom->setVertexArray( new_verts.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->setTexCoordArray(0,texc.get());
    geom->addPrimitiveSet( face_indices.get() );

    return geom.release();
}

float CircleCurve::calcProfileAtPercent(float percent)
{
    return 1.0;
}
