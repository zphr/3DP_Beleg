#include "FencePart.h"

FencePart::FencePart(osg::ref_ptr<osg::Geometry> &geom,
                     float extrudeWidth)
{
    _extrudeWidth = extrudeWidth;

    osg::GraphicsContext::WindowingSystemInterface* wsi =
        osg::GraphicsContext::getWindowingSystemInterface();

    wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0),
                             _windowWidth, _windowHeight);

    buildFencePart(geom);
}

FencePart::~FencePart()
{
}

void FencePart::buildFencePart(osg::ref_ptr<osg::Geometry> &geom)
{
    // ---------------------------------------- Deckflächen

    osg::ref_ptr<osg::Geometry> new_geom =
        new osg::Geometry(*(geom.get()), osg::CopyOp::DEEP_COPY_ALL);
    osg::Vec3Array* new_verts = static_cast<osg::Vec3Array*>(new_geom->getVertexArray() );
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Z_AXIS * -1 );

    float min_x = 0.0;
    float min_y = 0.0;

    for(int i=0; i < new_verts->getNumElements(); i++)
    {
        osg::Vec3 v = (*new_verts)[i];
         
        v = osg::Vec3(v.x() / (float)_windowWidth,
                      v.y() / (float)_windowHeight,
                      v.z());
        
        if(i == 0)
        {
            min_x = v.x();
            min_y = v.y();
        }
        else
        {
            if(v.x() < min_x)
                min_x = v.x();
            if(v.y() < min_y)
                min_y = v.y();
        }

        (*new_verts)[i] = v;
    }

    // unterste linke Ecke soll im Ursprung liegen
    osg::Vec3 min_trans(min_x, min_y, 0);
    for(int i=0; i < new_verts->getNumElements(); i++)
        (*new_verts)[i] = (*new_verts)[i] - min_trans;
        
    new_geom->setNormalArray( normals.get() );
    new_geom->setNormalBinding( osg::Geometry::BIND_OVERALL );
    new_geom->setPrimitiveSet(0, new osg::DrawArrays(GL_POLYGON, 0, new_verts->getNumElements()) );

    
    osg::BoundingBox bbox = new_geom->getBound();

    osg::ref_ptr<osg::Vec3Array> extrude_verts = new osg::Vec3Array();
    osg::Vec3 extrude_vec(0, 0, _extrudeWidth);

    for(int i=0; i < new_verts->getNumElements(); i++)
    {
        extrude_verts->push_back( osg::Vec3((*new_verts)[i] + extrude_vec) );
    }
    
    osg::ref_ptr<osg::Geometry> new_geom_back = new osg::Geometry();
    new_geom_back->setVertexArray( extrude_verts.get() );
    osg::ref_ptr<osg::Vec3Array> normals_back = new osg::Vec3Array;
    normals_back->push_back( osg::Z_AXIS );
    new_geom_back->setNormalArray( normals_back.get() );
    new_geom_back->setNormalBinding( osg::Geometry::BIND_OVERALL );
    new_geom_back->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON, 0, extrude_verts->getNumElements()) );
    
    osgUtil::Tessellator tessellator;
    tessellator.retessellatePolygons( *new_geom );
    tessellator.retessellatePolygons( *new_geom_back );
    
    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    addDrawable( new_geom.get() );
    addDrawable( new_geom_back.get() );

    // ---------------------------------------- Rand

    osg::ref_ptr<osg::Geometry> border_geom = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> border_verts = new osg::Vec3Array();
    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    osg::Vec3 center = bbox.center();
    float bound_width = bbox.xMax() - bbox.xMin();
    float bound_height = bbox.yMax() - bbox.yMin();
    float ratio = bound_width / bound_height;
    float scale = 0.2;

    int vert_count = new_verts->getNumElements();
    int new_vert_count = vert_count*4;

    for(int i=0; i <= vert_count; i++)
    {
        osg::Vec3 vec = (*new_verts)[i%vert_count];
        osg::Matrix mat;
        mat.postMult( osg::Matrix().translate( vec ) );
        mat.postMult( osg::Matrix().translate( -center ) );
        mat.postMult( osg::Matrix().scale( 1 + scale , 1 + scale * ratio, 1 ) );
        mat.postMult( osg::Matrix().translate( center ) );

        border_verts->push_back( vec );
        border_verts->push_back( mat.getTrans() + (extrude_vec * 2/10.0) );
        border_verts->push_back( mat.getTrans() + (extrude_vec * 8/10.0) );
        border_verts->push_back( vec + extrude_vec );
        
        if(i < vert_count)
            for(int j=0; j < 3; j++)
            {
                face_indices->push_back( (i*4+j)%new_vert_count );
                face_indices->push_back( (i*4+j+1)%new_vert_count );
                face_indices->push_back( (i*4+j+5)%new_vert_count );
                face_indices->push_back( (i*4+j+4)%new_vert_count );
            }
    }

    border_geom->setVertexArray( border_verts.get() );
    border_geom->addPrimitiveSet( face_indices.get() );

    osgUtil::SmoothingVisitor smoother;
    smoother.smooth( *border_geom );

    osg::Vec3 v1((*new_verts)[0] - center);
    osg::Vec3 v2((*new_verts)[1] - center);
    float normal_z = (v1 ^ v2).z();

    // falls die Punkt gegen den Uhrzeigersinn zusammengeklicht wurden
    // -> Normalen umdrehen
    if(normal_z > 0.0)
    {
        osg::Vec3Array* border_normals = static_cast<osg::Vec3Array*>(border_geom->getNormalArray() );
        for(int i=0; i < border_normals->getNumElements(); i++)
            (*border_normals)[i] = (*border_normals)[i] * -1;
    }

    addDrawable( border_geom.get() );
}


