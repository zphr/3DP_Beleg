#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <fstream>
#include "LSysPlant.h"
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

//#include <osg/MatrixTransform>
//#include <osg/Point>
//#include <osg/PointSprite>
//#include <osg/Texture2D>
//#include <osg/BlendFunc>
//#include <osgDB/ReadFile>
//#include <osgGA/StateSetManipulator>
//#include <osgParticle/ParticleSystem>
//#include <osgParticle/ParticleSystemUpdater>
//#include <osgParticle/ModularEmitter>
//#include <osgParticle/ModularProgram>
//#include <osgParticle/AccelOperator>

int VonKochKurve( int argc, char** argv)
{
    string word ("F--F--F");
    string result;

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < word.length(); j++)
        {

            if(word[j] == 'F')
                result += "F+F--F+F";
            else
                result += word[j];

        }
        word = result;
        result = "";
    }

    result = word;
    cout << result << endl;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    float dist = 1.0, alpha = 0.0, delta = 60.0;
    float x = 0, y = 0;

    for(int i = 0; i < result.length(); i++)
    {

        if(result[i] == 'F')
        {
            x -= dist * cos(alpha * (M_PI/180.0));
            y -= dist * sin(alpha * (M_PI/180.0));
            //cout << "x: " << x << " y: " << y << endl;
            vertices->push_back( osg::Vec3(x, 0.0f, y) );
        }
        else if(result[i] == 'f')
        {
            x -= dist * cos(alpha * (M_PI/180.0));
            y -= dist * sin(alpha * (M_PI/180.0));
        }
        else if(result[i] == '+')
            alpha += delta;
        else if(result[i] == '-')
            alpha -= delta;

    }   

    osg::ref_ptr<osg::Geometry> line_loop = new osg::Geometry;
    line_loop->setVertexArray( vertices.get() );

    line_loop->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, vertices->getNumElements()) );
    line_loop->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> root = new osg::Geode;
    root->addDrawable( line_loop.get() );

    //osg::StateSet* set = root->getOrCreateStateSet();
    //set->setMode(GL_BLEND, osg::StateAttribute::ON);
    //osg::BlendFunc *fn = new osg::BlendFunc();
    //fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
    //set->setAttributeAndModes(fn, osg::StateAttribute::ON);

    ///// Setup the point sprites
    //osg::PointSprite *sprite = new osg::PointSprite();
    //set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);

    ///// Give some size to the points to be able to see the sprite
    //osg::Point *point = new osg::Point();
    //point->setSize(5);
    //set->setAttribute(point);

    ///// Disable depth test to avoid sort problems and Lighting
    //set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    //set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}


int StackBasedLSystem( int argc, char** argv)
{
    string word ("F");
    string result;
    //string p_1 ("F[+F]F[-F]F");
    string p_1 ("FF-[-F+F+F]+[+F-F-F]");

    float dist = 1.0, alpha = 0.0, delta = 22.5;
    float x = 0, y = 0;
    unsigned int n = 4;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> stack = new osg::Vec4Array;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < word.length(); j++)
        {

            if(word[j] == 'F')
                result += p_1;
            else
                result += word[j];

        }
        word = result;
        //cout << result << endl;
        result = "";
    }

    result = word;

    for(int i = 0; i < result.length(); i++)
    {

        if(result[i] == 'F')
        {
            x -= dist * cos(alpha * (M_PI/180.0));
            y -= dist * sin(alpha * (M_PI/180.0));
            //cout << "x: " << x << " y: " << y << endl;
            vertices->push_back( osg::Vec3(x, 0.0f, y) );
        }
        else if(result[i] == 'f')
        {
            x -= dist * cos(alpha * (M_PI/180.0));
            y -= dist * sin(alpha * (M_PI/180.0));
        }
        else if(result[i] == '+')
            alpha += delta;
        else if(result[i] == '-')
            alpha -= delta;
        else if(result[i] == '[')
        {
            stack->push_back( osg::Vec4(x, 0.0f, y, alpha) );
        }
        else if(result[i] == ']')
        {
            x = (*stack)[stack->getNumElements()-1].x();
            y = (*stack)[stack->getNumElements()-1].z();
            alpha = (*stack)[stack->getNumElements()-1].w();
            stack->pop_back();
        }

    }   

    osg::ref_ptr<osg::Geometry> line_loop = new osg::Geometry;
    line_loop->setVertexArray( vertices.get() );

    line_loop->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, vertices->getNumElements()) );
    //line_loop->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> root = new osg::Geode;
    root->addDrawable( line_loop.get() );

    //osg::StateSet* set = root->getOrCreateStateSet();
    //set->setMode(GL_BLEND, osg::StateAttribute::ON);
    //osg::BlendFunc *fn = new osg::BlendFunc();
    //fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
    //set->setAttributeAndModes(fn, osg::StateAttribute::ON);

    ///// Setup the point sprites
    //osg::PointSprite *sprite = new osg::PointSprite();
    //set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);

    ///// Give some size to the points to be able to see the sprite
    //osg::Point *point = new osg::Point();
    //point->setSize(5);
    //set->setAttribute(point);

    ///// Disable depth test to avoid sort problems and Lighting
    //set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    //set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}

inline float DegToRad(float angle)
{
    return angle * (M_PI/180.0);
}

inline void RotMatX(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( DegToRad(angle), osg::Vec3f( 1.0f, 0.0f, 0.0f ) );  // X axis

    mat = mRotationMatrix * mat;
}

inline void RotMatY(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( DegToRad(angle), osg::Vec3f( 0.0f, 1.0f, 0.0f ) );  

    mat = mRotationMatrix * mat;
}

inline void RotMatZ(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( DegToRad(angle), osg::Vec3f( 0.0f, 0.0f, 1.0f ) );  

    mat = mRotationMatrix * mat;
}

inline void RotY(float angle, osg::Vec3 &vec)
{
    osg::Quat rot_q = osg::Quat(DegToRad(angle), osg::Vec3(0.0, 1.0, 0.0));
    vec =  rot_q * vec;
}
inline void RotZ(float angle, osg::Vec3 &vec)
{
    osg::Quat rot_q = osg::Quat(DegToRad(angle), osg::Vec3(0.0, 0.0, 1.0));
    vec =  rot_q * vec;
}


struct PlantStack
{
  osg::Matrix rot_mat;
  osg::Vec4 dist;
};

int main( int argc, char** argv)
{

    vector<PlantStack> plant_stack;
    osg::Vec4 dist (0.0, 0.0, 1.0, 1.0);
    osg::Matrix dist_mat = dist_mat.translate(osg::Vec3(0.0, 0.0, 1.0));

    osg::Matrix rot_mat;

    //osg::ref_ptr<osg::Vec3Array> dist = new osg::Vec3Array;
    float alpha = 0.0, delta = 90.0;
    float x = 0, y = 0, z = 0;
    unsigned int n = 2;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> stack = new osg::Vec4Array;

    string word ("A");
    string result;
    string p_1 ("[FA](((([&FA]((((((([&FA]");

    map<char, string> rules;
    rules['A'] = "[FA](((([&FA]((((((([&FA]";
    rules['F'] = "S(((((F";
    rules['S'] = "F";

    LSysPlant plant(n, delta, rules, p_1, dist, rot_mat);
    string test;
    plant.generatePlantWord();

    test = plant.getParanthesesContent(5, "test((blabla asdf asdf) asldjfasdjf (alsdfjasdf)) test 1 2 3");

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < word.length(); j++)
        {

            if(word[j] == 'A')
                result += p_1;
            if(word[j] == 'F')
                result += "S(((((F";
            if(word[j] == 'S')
                result += "F";
            else
                result += word[j];

        }
        word = result;
        //cout << result << endl;
        result = "";
    }

    result = word;
    //result = "FF[&F]FFFFFFFFFF";

    for(int i = 0; i < result.length(); i++)
    {

        if(result[i] == 'F')
        {
            x += (rot_mat * dist)[0];
            y += (rot_mat * dist)[1];
            z += (rot_mat * dist)[2];
            vertices->push_back( osg::Vec3(x, y, z) );
        }
        else if(result[i] == 'f')
        {
            x += (rot_mat * dist)[0];
            y += (rot_mat * dist)[1];
            z += (rot_mat * dist)[2];
        }
        else if(result[i] == '+')
            RotMatZ(delta, rot_mat);
        else if(result[i] == '-')
            RotMatZ(-delta, rot_mat);
        else if(result[i] == '&')
            RotMatY(delta, rot_mat);
        else if(result[i] == '^')
            RotMatY(-delta, rot_mat);
        else if(result[i] == '(')
            RotMatX(delta, rot_mat);
        else if(result[i] == ')')
            RotMatX(-delta, rot_mat);
        else if(result[i] == '|') // Umdrehen
            RotMatY(180.0, rot_mat);
        else if(result[i] == '[')
        {
            struct PlantStack new_item;
            new_item.rot_mat = rot_mat;
            new_item.dist = dist;

            // push_back erstellt sowieso eine Kopie -> kein Grund neuen
            // Speicher zu allokieren!
            plant_stack.push_back(new_item);
        }
        else if(result[i] == ']')
        {
            int size = plant_stack.size()-1;
            dist = plant_stack[size].dist;
            rot_mat = plant_stack[size].rot_mat;
            plant_stack.pop_back();
        }

    }   

    osg::ref_ptr<osg::Geometry> line_loop = new osg::Geometry;
    line_loop->setVertexArray( vertices.get() );

    line_loop->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, vertices->getNumElements()) );
    //line_loop->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, vertices->getNumElements()) );

    osg::ref_ptr<osg::Geode> root = new osg::Geode;
    root->addDrawable( line_loop.get() );

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}


