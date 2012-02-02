#include "FlowerBucket.h"

const osg::ref_ptr<osg::Node> FlowerBucket::_fencePartModel =
    osgDB::readNodeFile("3d/latte.obj");

const osg::ref_ptr<osg::Image> FlowerBucket::_fenceImg = 
    osgDB::readImageFile( "3d/latte.png" );

const osg::ref_ptr<osg::Image> FlowerBucket::_bucketImg = 
    osgDB::readImageFile( "3d/kuebel_basis.png" );

const osg::ref_ptr<osg::Image> FlowerBucket::_earthImg = 
    osgDB::readImageFile( "3d/erde.png" );

const float FlowerBucket::_earthTileU = 2.0;
const float FlowerBucket::_earthTileV = 2.0;

const float FlowerBucket::_bucketTileU = 1.0;
const float FlowerBucket::_bucketTileV = 1.4176;

FlowerBucket::FlowerBucket(float width, float depth)
{
    _fpIndex = 0;
    _useModel = true;

    _texture = new osg::Texture2D;
    _texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    _texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    _texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    _texture->setImage( FlowerBucket::_fenceImg.get() );
    
    _bucketTex = new osg::Texture2D;
    _bucketTex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    _bucketTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    _bucketTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    _bucketTex->setImage( FlowerBucket::_bucketImg.get() );

    _fenceModelOffset = 0.08;
    _fenceModelWidth  = 0.267;
    _fenceModelHeight = 1.4176;
    _fenceModelDepth  = 0.047124;

    if(width < (_fenceModelWidth + _fenceModelOffset))
        _width = (_fenceModelWidth + _fenceModelOffset);
    else
        _width  = width;

    if(depth < (_fenceModelWidth + _fenceModelOffset))
        _depth = (_fenceModelWidth + _fenceModelOffset);
    else
        _depth  = depth;

    _fenceHOffset = 0.1;
    _fenceOffset = _fenceModelOffset;
    _fenceWidth  = _fenceModelWidth;
    _fenceHeight = _fenceModelHeight; 
    _fenceDepth  = _fenceModelDepth;

    float val = 0.75;
    float r[4] = {0,val,0,0};
    float s[4] = {val,0,0,0};
    float t[4] = {0,0,val,0};

    calcParameters(FlowerBucket::_fencePartModel.get(), _fenceWidth, 1.0, r, s, t);
    
    _verts = new osg::Vec3Array();
    _verts->push_back(osg::Vec3(0,0,0));
    _verts->push_back(osg::Vec3(_width, 0, 0));
    _verts->push_back(osg::Vec3(_width, _depth, 0));
    _verts->push_back(osg::Vec3(0, _depth, 0));

    buildEarth();
    buildBucket();
    buildFence();
}

    
inline void FlowerBucket::calcParameters(osg::Node *fencePart,
                                         float fp_width, float scale_ratio,
                                         float r[4], float s[4], float t[4])
{
    _fencePart = fencePart;
    
    osg::StateSet* state = fencePart->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0,_texture,osg::StateAttribute::ON);

    // Texturkoordinatengenerierung initialisieren
    state->setTextureMode(0, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
    state->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
    state->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);

    // Texturkoordinatengenerierung Koeffizienten-Vektoren r,s,t
    // definieren
    osg::TexGen *tg = new osg::TexGen;
    tg->setMode(osg::TexGen::OBJECT_LINEAR);
    tg->setPlane(osg::TexGen::R, osg::Plane(r[0], r[1], r[2], r[3]));
    tg->setPlane(osg::TexGen::S, osg::Plane(s[0], s[1], s[2], s[3]));
    tg->setPlane(osg::TexGen::T, osg::Plane(t[0], t[1], t[2], t[3]));
    
    state->setTextureAttributeAndModes(0, tg, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    
    _fenceWidth  = scale_ratio*fp_width + _fenceOffset;
    _fenceCountX = (int)(_width / _fenceWidth);
    _fenceCountY = (int)(_depth / _fenceWidth);

    _fenceArrayOffsetX = _fenceWidth * ((_width / _fenceWidth) - _fenceCountX)+_fenceOffset; 
    _fenceArrayOffsetY = _fenceWidth * ((_depth / _fenceWidth) - _fenceCountY)+_fenceOffset; 
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
    osg::Vec3 trans2_v;
    
    if(alongY)
    {
        count = _fenceCountY;
        angle = osg::DegreesToRadians(90.0);
        trans_init_v = osg::Vec3(_fenceArrayOffsetY/2.0,_fenceDepth,_fenceHOffset);
        trans_v = osg::Vec3(0,_fenceWidth,0);
        trans2_v = osg::Vec3(_width+_fenceDepth*2,0,0);
    }
    else
    {
        count = _fenceCountX;
        trans_init_v = osg::Vec3(_fenceArrayOffsetX/2.0,-_fenceDepth,_fenceHOffset);
        trans_v = osg::Vec3(_fenceWidth,0,0);
        trans2_v = osg::Vec3(0,_depth+_fenceDepth*2,0);
    }
    
    for(int i=0; i < count; i++)
    {
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        osg::Matrix mat =
            osg::Matrix().translate(trans_init_v);
        mat.postMult(osg::Matrix().rotate(angle, osg::Z_AXIS));
        mat.postMult(osg::Matrix().translate(trans_v * i));
        trans->setMatrix( mat );
        trans->addChild( _fencePart.get() );
        addChild( trans.release() );

        trans = new osg::MatrixTransform();
        mat.postMult(osg::Matrix().translate(trans2_v));
        trans->setMatrix( mat );
        trans->addChild( _fencePart.get() );
        addChild( trans.release() );
    }
}

void FlowerBucket::buildFence()
{
    _fpIndex = getNumChildren()-1;
    placeFence();
    placeFence(true);
}

void FlowerBucket::buildBucket()
{
    osg::ref_ptr<osg::Geode> kasten_gd = new osg::Geode;

    float tile_u = _fenceHeight / FlowerBucket::_bucketTileU;
    float tile_v = _width / FlowerBucket::_bucketTileV;

    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight-0.2, 0, 0,
                                          tile_u, tile_v ) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight-0.2, 0.1, 0,
                                          tile_u, tile_v) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight/10.0, -_fenceOffset/2.0, 7*_fenceHeight/10.0, 0.1, 12.0) );
    kasten_gd->addDrawable( buildBoxWalls(_verts, _fenceHeight/10.0, -_fenceOffset/2.0, _fenceHeight/10.0, 0.1, 12.0) );

    osg::StateSet* state = kasten_gd->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0, FlowerBucket::_bucketTex, osg::StateAttribute::ON);

    // osg::PolygonMode * polygonMode = new osg::PolygonMode;
    // polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
    // state->setAttributeAndModes( polygonMode,
    //                              osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

	addChild( kasten_gd.release() ); 
}

osg::Geometry* FlowerBucket::buildBoxWalls(osg::ref_ptr<osg::Vec3Array> verts,
                                           float height,
                                           float inset,
                                           float h_offset,
                                           float tile_u,
                                           float tile_v)
{
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
    osg::Vec3 normal(0,-1,0);
    if(inset > 0)
        normal = normal * -1.0;

    osg::ref_ptr<osg::Vec3Array> box_verts = new osg::Vec3Array();

    osg::Vec3 inset_vec(inset, inset, 0);
    // Matrix um inset_vec in 4er Schritten um 90° zu drehen
    osg::Matrix inset_mat;      
    inset_mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS));

    osg::Vec3 height_vec(0,0,height);
    osg::Vec3 h_offset_vec(0,0,h_offset);

    // ---------------------------------------- Texturkoordinatengenerierung
    osg::ref_ptr<osg::Vec2Array> texc = new osg::Vec2Array;
    
    // Länge der Gegenkathete
    float opposite = sin(osg::DegreesToRadians(45.0)) * inset;

    // Gesamtlänge U
    float length_u = height + 2*opposite;
    // normalisierte Länge der Verschiebung in U
    float inset_u  = opposite / length_u;
    // normalisierte Länge der Höhe 
    float height_u = height / length_u;

    // Gesamtlänge V
    float length_v = 2*(_width + _depth);
    // normalisierte Länge der Verschiebung in V
    float inset_v  = opposite / length_v;
    float v        = 0;

    for(int i=0; i < 4; i++)
    {
        float l_v = ((*verts)[(i+1)%4] - (*verts)[i]).length() / length_v;

        box_verts->push_back((*verts)[i] + h_offset_vec + inset_vec);
        box_verts->push_back((*verts)[(i+1)%4] + h_offset_vec + inset_vec * inset_mat);
        box_verts->push_back((*verts)[(i+1)%4] + h_offset_vec + height_vec + inset_vec * inset_mat);
        box_verts->push_back((*verts)[i] + h_offset_vec + height_vec + inset_vec );

        texc->push_back(osg::Vec2(tile_u * (inset_u),            tile_v *  v ));
        texc->push_back(osg::Vec2(tile_u * (inset_u),            tile_v * (v + l_v) ));
        texc->push_back(osg::Vec2(tile_u * (height_u + inset_u), tile_v * (v + l_v) ));
        texc->push_back(osg::Vec2(tile_u * (height_u + inset_u), tile_v *  v ));
        
        osg::Matrix mat;
        mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0)*i, osg::Z_AXIS));

        for(int j=0; j<4; j++)
            normals->push_back( normal * mat);

        if(inset)
        {
            // Inset-Deckflächen erstellen, j wird verwendet um den
            // Höhenversatz ein-/auszuschalten
            for(int j=0; j<2; j++)
            {

                box_verts->push_back( (*verts)[i]       + h_offset_vec + height_vec*j );
                box_verts->push_back( (*verts)[(i+1)%4] + h_offset_vec + height_vec*j );
                box_verts->push_back( (*verts)[(i+1)%4] + h_offset_vec + height_vec*j + inset_vec * inset_mat );
                box_verts->push_back( (*verts)[i]       + h_offset_vec + height_vec*j + inset_vec );
                
                texc->push_back(osg::Vec2(tile_u * ((height_u + 2*inset_u)*j), tile_v * (v-inset_v)));
                texc->push_back(osg::Vec2(tile_u * ((height_u + 2*inset_u)*j), tile_v * (v+l_v+inset_v) ));
                texc->push_back(osg::Vec2(tile_u * ((height_u)*j + inset_u), tile_v * (v+l_v) ));
                texc->push_back(osg::Vec2(tile_u * ((height_u)*j + inset_u), tile_v * (v)));
                
                // Normalendefinition
                for(int k=0; k<4; k++)
                    // Normale umgekehren; je nachdem ob es sich um
                    // einen Punkt der unteren/oberen Fläche handelt.
                    normals->push_back( osg::Vec3(0,0,-1 + (2*j)) );
            }
        }
        inset_vec = inset_vec * inset_mat;
        v += l_v;
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray( box_verts.get() );
    // geom->setVertexArray( texc.release() );
    geom->setNormalArray( normals.release() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->setTexCoordArray(0,texc.release());
    geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, box_verts->getNumElements()) );
    box_verts.release();

    return geom.release();
}

void FlowerBucket::buildEarth()
{
    float r_z = 0.0;
    unsigned int res_x = _fenceCountX+1;
    unsigned int res_y = _fenceCountY+1;

    float length_x = _width / _fenceCountX;
    float length_y = _depth / _fenceCountY;
    
    srand ( time(NULL) );

    osg::ref_ptr<osg::Vec3Array> earth_verts = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec2Array> texc = new osg::Vec2Array;
    float tex_x  = 1.0 / (_fenceCountX-1);
    float tex_y = 1.0 / (_fenceCountY-1);

    float tile_u = _width / FlowerBucket::_earthTileU;
    float tile_v = _depth / FlowerBucket::_earthTileV;

    osg::ref_ptr<osg::DrawElementsUInt> face_indices = new osg::DrawElementsUInt( GL_QUADS );

    for(int i=0; i < res_x; i++)
    {
        for(int j=0; j < res_y; j++)
        {
            earth_verts->push_back(
                osg::Vec3(length_x * i,
                          length_y * j,
                          // _fenceHeight/1.25
                          ((float)(rand() % 100))/(1000.0*(1.0-((rand()%2)*2))) + _fenceHeight/1.25
                         ));
            texc->push_back(osg::Vec2(tile_u * (tex_x * j),
                                      tile_v *( tex_y * i)));

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
    
    osg::ref_ptr<osg::Texture2D> earth_tex = new osg::Texture2D;
    earth_tex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    earth_tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    earth_tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    earth_tex->setImage( FlowerBucket::_earthImg.get() );

    osg::StateSet* state = gd->getOrCreateStateSet();
    state->setTextureAttributeAndModes(0, earth_tex.release(), osg::StateAttribute::ON);
    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setDiffuse( osg::Material::FRONT, osg::Vec4(1,1,1,1) );
    material->setSpecular( osg::Material::FRONT, osg::Vec4(0.8, 0.8, 0.8, 1.0) );
    material->setShininess( osg::Material::FRONT, 15.0 );
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    state->setAttribute( material.release() );

    addChild( gd.release() );
}

void FlowerBucket::setFencePart(osg::Geode* gd)
{

    // die alten Latten löschen
    removeChildren(2, getNumChildren()-1);

    _fpGeode = gd;

    osg::BoundingBox bbox = _fpGeode->getBoundingBox();
    float fp_width = bbox.xMax() - bbox.xMin();
    float fp_height = bbox.yMax() - bbox.yMin();
    float scale_ratio = _fenceHeight/fp_height;

    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    osg::Matrix mat;
    mat.postMult(osg::Matrix().scale(scale_ratio,
                                     scale_ratio,
                                     scale_ratio));
    mat.postMult(osg::Matrix().rotate(osg::DegreesToRadians(90.0), osg::X_AXIS));
    trans->setMatrix( mat );
    trans->addChild( _fpGeode );

    float val = 2.0;
    float r[4] = {0,0,val,0};
    float s[4] = {0,val,0,0};
    float t[4] = {val,0,0,0};

    calcParameters(trans.release(), fp_width, scale_ratio, r, s, t);

    buildFence();
}

void FlowerBucket::swapModels()
{
    if(!_fpGeode.get())
        return;
    
    if(_useModel)
    {
        removeChildren(2, getNumChildren()-1);
        
        _fenceOffset = _fenceModelOffset;
        _fenceWidth  = _fenceModelWidth;
        _fenceHeight = _fenceModelHeight; 
        _fenceDepth  = _fenceModelDepth;

        float val = 0.75;
        float r[4] = {0,val,0,0};
        float s[4] = {val,0,0,0};
        float t[4] = {0,0,val,0};

        calcParameters(FlowerBucket::_fencePartModel.get(),
                       _fenceWidth, 1.0, r, s, t);
        
        buildFence();
    }
    else
    {
        setFencePart( _fpGeode.release() );
    }
    
    _useModel = !_useModel;
}
