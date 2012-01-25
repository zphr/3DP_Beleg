#include "FlowerBucket.h"

FlowerBucket::FlowerBucket(osgViewer::Viewer* viewer)
{
    _fencePartModel = osgDB::readNodeFile("3d/latte.obj");
    _fencePart = _fencePart.get();

    _fenceOffset = 0.08;
    _fenceArrayOffset = _fenceOffset;
    _fenceWidth = 0.267 + _fenceArrayOffset;
    _fenceHeight = 1.569;
    _offset = 0.25;

    _fenceCountX = 16;
    _fenceCountY = 4;

    _verts = new osg::Vec3Array();
    _verts->push_back(osg::Vec3(0,0,0));
    _verts->push_back(osg::Vec3(_fenceCountX * _fenceWidth, 0, 0));
    _verts->push_back(osg::Vec3(_fenceCountX * _fenceWidth, _fenceCountY * _fenceWidth, 0));
    _verts->push_back(osg::Vec3(0, _fenceCountY * _fenceWidth, 0));

    // osg::ref_ptr<FencePartController> ctrler = new FencePartController(this);
    // viewer->addEventHandler( ctrler.get() );

    buildBucket();
    buildFence();
    buildEarth();
}


FlowerBucket::~FlowerBucket()
{
    _verts.release();
}

inline void FlowerBucket::placeFence(bool alongY)
{
    int count = 0;
    float angle = 0.0;
    osg::Vec3 trans_init_v;
    osg::Vec3 trans_v;
    osg::Vec3 trans_rot_v;

    if(alongY)
    {
        count = _fenceCountY;
        angle = osg::DegreesToRadians(90.0);
        trans_init_v = osg::Vec3(_fenceArrayOffset/2.0,+_fenceOffset,0);
        trans_v = osg::Vec3(0,_fenceWidth,0);
        trans_rot_v = osg::Vec3(_fenceWidth*_fenceCountX+_fenceOffset*2,0,0);
    }
    else
    {
        count = _fenceCountX;
        trans_init_v = osg::Vec3(_fenceArrayOffset/2.0,-_fenceOffset,0);
        trans_v = osg::Vec3(_fenceWidth,0,0);
        trans_rot_v = osg::Vec3(0,_fenceWidth*_fenceCountY+_fenceOffset*2,0);
    }

    for(int i=0; i < count; i++)
    {
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        osg::Matrix mat =
            osg::Matrix().translate(trans_init_v);
        mat.postMult(osg::Matrix().rotate(angle, osg::Z_AXIS));
        mat.postMult(osg::Matrix().translate(trans_v * i));
        trans->setMatrix( mat );
        trans->addChild( _fencePartModel.get() );
        addChild( trans.release() );

        trans = new osg::MatrixTransform();
        mat.postMult(osg::Matrix().translate(trans_rot_v));
        trans->setMatrix( mat );
        trans->addChild( _fencePartModel.get() );
        addChild( trans.release() );
    }
}

void FlowerBucket::buildFence()
{
    placeFence();
    placeFence(true);
}

void FlowerBucket::buildBucket()
{
    osg::ref_ptr<osg::Geode> kasten_gd = new osg::Geode;

    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight-0.2) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight-0.2, 0.1) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight/10.0, -_fenceOffset/2.0, 7*_fenceHeight/10.0) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight/10.0, -_fenceOffset/2.0, _fenceHeight/10.0) );
    addChild( kasten_gd.release() );
}

osg::Geometry* FlowerBucket::buildBoxWalls(osg::ref_ptr<osg::Vec3Array> verts,
                                           float height,
                                           float inset,
                                           float h_offset)

{
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
    osg::Vec3 normal(0,-1,0);
    if(inset > 0)
        normal = normal * -1.0;

    osg::ref_ptr<osg::Vec3Array> box_verts = new osg::Vec3Array();

    osg::Matrix inset_mat;
    inset_mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));
    osg::Vec3 inset_v(inset, inset, 0);

    osg::Vec3 height_v(0,0,height);
    osg::Vec3 h_offset_v(0,0,h_offset);

    for(int i=0; i < 4; i++)
    {
        box_verts->push_back((*verts)[i] + h_offset_v + inset_v);
        box_verts->push_back((*verts)[(i+1)%4] + h_offset_v + inset_v * inset_mat);
        box_verts->push_back((*verts)[(i+1)%4] + h_offset_v + height_v + inset_v * inset_mat);
        box_verts->push_back((*verts)[i] + h_offset_v + height_v + inset_v );

        osg::Matrix mat;
        mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0)*i, osg::Z_AXIS));

        for(int j=0; j<4; j++)
            normals->push_back( normal * mat);

        if(inset)
        {
            for(int j=0; j<2; j++)
            {
                box_verts->push_back( (*verts)[i] + h_offset_v + height_v*j );
                box_verts->push_back( (*verts)[(i+1)%4] + h_offset_v + height_v*j );
                box_verts->push_back( (*verts)[(i+1)%4] + h_offset_v + inset_v * inset_mat + height_v*j );
                box_verts->push_back( (*verts)[i] + h_offset_v + inset_v + height_v*j);
                for(int k=0; k<4; k++)
                    // j=2 -> normal(0,0,1)
                    normals->push_back( osg::Vec3(0,0,-1 + (2*j)) );

            }
        }
        inset_v = inset_v * inset_mat;
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray( box_verts.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, box_verts->getNumElements()) );

    return geom.release();
}

void FlowerBucket::buildEarth()
{
    float r_z = 0.0;
    unsigned int res_x = _fenceCountX+1;
    unsigned int res_y = _fenceCountY+1;
    
    srand ( time(NULL) );

    osg::ref_ptr<osg::Vec3Array> earth_verts = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec2Array> texc = new osg::Vec2Array;
    float tex_x  = 1.0 / (_fenceCountX-1);
    float tex_y = 1.0 / (_fenceCountY-1);
    
    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    for(int i=0; i < res_x; i++)
    {
        for(int j=0; j < res_y; j++)
        {
            earth_verts->push_back(
                    osg::Vec3(_fenceWidth * i,
                              _fenceWidth * j,
                              // _fenceHeight/1.25
                              ((float)(rand() % 100))/(1000.0*(1.0-((rand()%2)*2))) + _fenceHeight/1.25
                             ));
            texc->push_back(osg::Vec2(tex_x * j, tex_y * i));

            if((i < (res_x-1)) && (j < (res_y-1)))
            {
                face_indices->push_back( i * res_y + j );
                face_indices->push_back( i * res_y + (j + 1));
                face_indices->push_back( (i+1) * res_y + (j + 1));
                face_indices->push_back( (i+1) * res_y + j);
            }
        }
    }

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    int vert_count = earth_verts->getNumElements();
    osg::Vec3 n;
    osg::Vec3 e1, e2, e3, e4;

    for(int i=0; i < vert_count; i++)
    {
        int imodr = (i % res_y);
        if(imodr == 0)
        {
            // Unten ganz links
            if(i == 0)
            {
                e2 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));
                n = e3 ^ e2;
            }
            // Oben ganz links
            else if(i == (vert_count-res_y))
            {
                e1 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));
                n = e2 ^ e1;
            }
            // am linken Rand 
            else
            {
                e1 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n /= 2;
            }

        }
        else if(imodr == (res_y-1))
        {

            // Unten ganz rechts
            if(i == (res_y-1))
            {
                e1 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));

                n = e2 ^ e1;
            }
            // Oben ganz rechts
            else if(i == (vert_count-1))
            {
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));

                n = e3 ^ e2;
            }
            // rechter Rand
            else
            {
                e1 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n /= 2;
            }

        }
        else
        {
            // Unterer Rand
            if( (i / res_y) == 0)
            {
                e1 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));
                e4 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));

                n = e2 ^ e1;
                n += e1 ^ e4;
                n /= 2;
            }
            // Oberer Rand
            else if((i/(vert_count - res_y) <= 1))
            {
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));
                e4 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));

                n = e3 ^ e2;
                n += e4 ^ e3;
                n /= 2;
            }
            // in der Mitte
            else
            {
                e1 = (((*earth_verts)[i+res_y]) - ((*earth_verts)[i]));
                e2 = (((*earth_verts)[i-1]) - ((*earth_verts)[i]));
                e3 = (((*earth_verts)[i-res_y]) - ((*earth_verts)[i]));
                e4 = (((*earth_verts)[i+1]) - ((*earth_verts)[i]));

                n = e2 ^ e1;
                n += e3 ^ e2;
                n += e4 ^ e3;
                n += e1 ^ e4;
                n /= 4;
            }
        }
        n.normalize();
        normals->push_back( n );
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray( earth_verts.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->setTexCoordArray(0,texc.get());
    geom->addPrimitiveSet( face_indices.get() );

    osg::ref_ptr<osg::Geode> gd = new osg::Geode;
    gd->addDrawable( geom.release() );
    addChild( gd.release() );
}


void FlowerBucket::setFencePart(osg::Geode* gd)
{
    _fencePart = gd;
}
