#include <iostream>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <fstream>

#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>

using namespace std;


class MonitoringTarget : public osg::Referenced
{
public:
  MonitoringTarget( int id) :
    _id(id)
  {
    cout << "Constructing target " << _id << endl;
  }

protected:
  virtual ~MonitoringTarget() 
  {
    cout << "Destroying target " << _id << endl;
  }

  int _id;
};

MonitoringTarget* createMonitoringTarget( unsigned int id )
{
  osg::ref_ptr<MonitoringTarget> target = new MonitoringTarget(id);
  // release() löscht den SmartPointer und gibt die Speicheradresse zurück
  // auf den gezeigt wurde. wenn die Adresse übernommen wird dann löscht die
  // Garbage Collection noch nicht den Speicher!

  return target.release();

  // Wenn der Speicherbereich auf den ein SmartPointer zeigt am Ende einer
  // Funktion zurückgegeben werden soll dann muss das mit release() geschehen!
  // get() dekrementiert nicht den referenceCount und sollte daher nicht
  // benutzt werden!
  //return target.get();
}

void SmartPointer_Exercise(int argc, char ** argv)
{
  //char* buffer;
  //// Get the current working directory: 
  //if( (buffer = _getcwd( NULL, 0 )) == NULL )
  //perror( "_getcwd error" );
  //else
  //{
  //printf( "%s \n", buffer);
  //free(buffer);
  //}

  // ref_ptr -> smart pointer providing memory management 
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("cessna.osg");

  // referenceCount() -> gibt zurück wieviele Pointer noch auf das jeweilige
  // Objekt verweisen! Methode der Basisklasse osg::Referenced

  // Heap ->  der Speicher zur freien Verfügung mit malloc/new usw.
  // Stack -> Speicher der durch standard Variablen Deklarationen gefüllt
  //          wird 

  // Wenn ein Pointer höher in der Hierarchie final gelöscht wird dann werden
  // auch alle Kinder freigegeben, falls sie mit ref_ptr referenziert werden!

  // Von osg::Referenced abgeleitete Klassen müssen mit new im Heap angelegt
  // werden, da SmartPointer nur dann diese Klassen löschen. Der Grund dafür
  // ist das Klassen Dekonstruktoren protected vererbt werden und damit nicht
  // für die Smart Pointer erreichbar sind!
  // osg::Node node; -> sollte damit nicht verwendet werden sondern:
  // osg::ref_ptr<osg::Node> node = new osg::Node;

  // Keine zyklischen Referenzen aufbauen damit kommt das Memory Management
  // nicht klar!

  //osgViewer::Viewer viewer;
  //viewer.setSceneData( root.get());
  //return viewer.run();

  osg::ref_ptr<MonitoringTarget> target = new MonitoringTarget(0);
  cout << "Referenced count before referring: " << target-> referenceCount() << endl;

  osg::ref_ptr<MonitoringTarget> anothertarget = target;
  cout << "Referenced count after referring: " << target-> referenceCount() << endl;

  for ( unsigned int i =1; i < 5; ++i)
    {
      osg::ref_ptr<MonitoringTarget> subtarget = createMonitoringTarget(i);
      cout << "Referenced count: " << subtarget-> referenceCount() << endl;
    }
}

int ArgumentParser_Exercise(int argc, char ** argv)
{
  osg::ArgumentParser arguments( &argc, argv);
  string filename;
  arguments.read( "--model", filename);

  osg::ref_ptr<osg::Node> root = osgDB::readNodeFile(filename);
  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());

  return viewer.run();
}

// ############################################### Notify Übung

class LogFileHandler : public osg::NotifyHandler
{
protected:
  ofstream _log;

public:
  LogFileHandler( const string& file)
  { 
    _log.open( file.c_str() );
  }
  virtual ~LogFileHandler() 
  {
    _log.close(); 
  }

  virtual void notify(osg::NotifySeverity severity, const char* msg)
  {
    _log << msg; 
  }

};

int NotifyLevel_Excersise( int argc, char** argv)
{
  osg::setNotifyLevel( osg::INFO );
  osg::setNotifyHandler( new LogFileHandler("output.txt") );

  osg::ArgumentParser arguments( &argc, argv);
  osg::ref_ptr<osg::Node> root = osgDB::readNodeFiles(arguments);
  if ( !root )
    {
      OSG_FATAL << arguments.getApplicationName() << ": No data loaded." << endl;
      return -1;
    }

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}

// ############################################### Geometrie Übung

int Shape_Excersise( int argc, char** argv)
{
  osg::ref_ptr<osg::ShapeDrawable> shape1 = new osg::ShapeDrawable;
  shape1->setShape(new osg::Box(osg::Vec3(-3.0f, 0.0f, 0.0f), 2.0f, 2.0f, 1.0f));

  osg::ref_ptr<osg::ShapeDrawable> shape2 = new osg::ShapeDrawable;
  shape2->setShape(new osg::Sphere(osg::Vec3(3.0f, 0.0f, 0.0f), 1.0f));
  shape2->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

  osg::ref_ptr<osg::ShapeDrawable> shape3 = new osg::ShapeDrawable;
  shape3->setShape(new osg::Cone(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f));
  shape3->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));

  osg::ref_ptr<osg::Geode> root = new osg::Geode;
  root->addDrawable( shape1.get() );
  root->addDrawable( shape2.get() );
  root->addDrawable( shape3.get() );

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  return viewer.run();
}

int PrimitiveSet__Exercise( int argc, char** argv)
{
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  // mit push_back Scheitelpunkte in das Vertex-Array einfügen
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );

  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  // Ist nur eine Ebene daher ist auch nur eine Normale notwendig! Die
  // Verbindung der Scheitelpunkte mit der Normalen wird später mit
  // setNormalBinding() und BIND_OVERALL aufgebaut.
  normals->push_back( osg::Vec3(0.0f,-1.0f,0.0f) );

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
  colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
  colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

  osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
  quad->setVertexArray( vertices.get() );
  quad->setNormalArray( normals.get() );
  // siehe BIND_OVERALL weiter oben
  quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
  quad->setColorArray( colors.get() );
  quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  // die eingefügten Arrays sollen als Rechteckfläche dargestellt werden (im
  // mathematisch positiven Sinn)
  quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );

  osg::ref_ptr<osg::Geode> root = new osg::Geode;
  root->addDrawable( quad.get() );

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  return viewer.run();

}


int main( int argc, char** argv)
{

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
  // mit push_back Scheitelpunkte in das Vertex-Array einfügen
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
  vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
  vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );

  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3(0.0f,-1.0f,0.0f) );

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
  colors->push_back( osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
  colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
  colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

  osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
  quad->setVertexArray( vertices.get() );
  quad->setNormalArray( normals.get() );
  // siehe BIND_OVERALL weiter oben
  quad->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
  quad->setColorArray( colors.get() );
  quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  quad->addPrimitiveSet( new osg::DrawArrays(GL_POINTS, 0, vertices->getNumElements()) );

  osg::ref_ptr<osg::Geode> root = new osg::Geode;
  root->addDrawable( quad.get() );
   
  osg::StateSet* set = root->getOrCreateStateSet();
  set->setMode(GL_BLEND, osg::StateAttribute::ON);
  osg::BlendFunc *fn = new osg::BlendFunc();
  fn->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::DST_ALPHA);
  set->setAttributeAndModes(fn, osg::StateAttribute::ON);

  /// Setup the point sprites
  osg::PointSprite *sprite = new osg::PointSprite();
  set->setTextureAttributeAndModes(0, sprite, osg::StateAttribute::ON);

  /// Give some size to the points to be able to see the sprite
  osg::Point *point = new osg::Point();
  point->setSize(5);
  set->setAttribute(point);

  /// Disable depth test to avoid sort problems and Lighting
  set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  return viewer.run();

}
