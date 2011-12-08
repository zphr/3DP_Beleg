#include "LSysPlant.h"
using namespace std;

LSysPlant::LSysPlant(
        unsigned int repeats,
        float delta,
        map<char,string> rules,
        string startWord,
        osg::Vec4 distanceVector = osg::Vec4(),
        osg::Matrix rotMatrix = osg::Matrix(),
        string variable)
: _repeats(repeats),
    _delta(delta),
    _rules(rules),
    _startWord(startWord),
    _distanceVector(distanceVector),
    _rotMatrix(rotMatrix),
    _variable(variable)
{
    _currentBranch = &_firstBranch;
    
    osg::ref_ptr<osg::Vec4Array> new_v4array = new osg::Vec4Array;
    _branches.push_back( new_v4array ); 
    _vertices = new osg::Vec4Array;
    _vertices->push_back( osg::Vec4(0.0, 0.0, 0.0, 1.0) );
    _indices = new osg::DrawElementsUInt( GL_LINES );
}

LSysPlant::~LSysPlant(void)
{ 
    for(int i = 0; i < _branches.size(); i++)
        _branches[i].release();

    _vertices.release();

    vector<vector<BranchNode*>*> curr_children = _firstBranch.getChildrenPerLevel();

    for(int i=0; i < curr_children.size(); i++)
    {
        for(int j=0; j < (*(curr_children[i])).size(); j++)
        {
            delete (*(curr_children[i]))[j];
        }
    }

}

void LSysPlant::generatePlantWord()
{
    string word = _startWord, str, function_str = "";
    int para_end = 0;

    for(int i = 0; i < _repeats; i++)
    {
        for(int j = 0; j < word.length(); j++)
        {

            if (_rules.count(word[j]) != 0)
            {
                if(j+1 < word.length())
                {
                    // Funktion parsen 
                    if(word[j+1] == '(')
                    {
                        function_str = getParanthesesContent(j+1, word, para_end);
                    }
                }

                string tmp_str = "";
                tmp_str += _rules.find(word[j])->second;
                for(int k = 0; k < tmp_str.length(); k++)
                    if(tmp_str[k] == 'x')
                        str += function_str;
                    else
                        str += tmp_str[k];
            }
            else
                str += word[j];

            if(para_end)
            {
                j = para_end;
                para_end = 0;
            }

        }
        word = str;
        str = "";
    }

    _startWord = word;
}

void LSysPlant::generatePlant()
{
    osg::Vec4 vec(0,0,0,1);
    osg::ref_ptr<osg::Vec4Array> branch_array = new osg::Vec4Array;
    int branch = 0, index = 0, old_index = -1, new_i = 0;
    string function_str = "";
    double f_result = 1.0;
    float angle = 0.0;

    _firstBranch.addPoint( vec );
    _currentBranch = &_firstBranch;

    _branches[0]->push_back( vec );

    for(int i = 0; i < _startWord.length(); i++)
    {
        // Funktionsergebnisse zurücksetzen
        f_result = 1.0;
        angle = _delta;

        // ---------------------------------------- Funktions Auswertung
        // -------------------------------------------------------------
        if(i+1 < _startWord.length())
        {
            if(_startWord[i+1] == '(')
            {
                function_str = getParanthesesContent(i+1, _startWord, new_i);

                if(function_str != "")
                {
                    _fparser.Parse(function_str, _variable);
                    double variables[1] = { 1.0 };

                    if( // Funktion für die Rotations-Wörter auswerten
                            (_startWord[i] == '+') || (_startWord[i] == '-') ||
                            (_startWord[i] == '&') || (_startWord[i] == '^') ||
                            (_startWord[i] == '{') || (_startWord[i] == '}')
                      )
                    {
                        variables[0] = _delta;
                        angle = _fparser.Eval(variables);
                    }
                    // Funktion für die Translation auswerten
                    else if((_startWord[i] == 'F') || (_startWord[i] == 'F'))
                    {
                        variables[0] = _distanceVector.z();
                        f_result = _fparser.Eval(variables);
                    }
                }
            }
        }

        switch(_startWord[i])
        { 
            case 'F':
                {
                    vec += (_rotMatrix * (_distanceVector * (float)f_result));
                    vec[3] = 1.0;

                    _currentBranch->addPoint( vec );
                    _branches[branch]->push_back( vec );
                    _vertices->push_back( vec );

                    // Indizes 
                    if(old_index != -1)
                    {
                        _indices->push_back( old_index );
                        old_index = -1;
                    }
                    else
                        _indices->push_back( index );

                    _indices->push_back( ++index );

                    break;
                }
            case 'f':
                {
                    vec += (_rotMatrix * (_distanceVector * (float)f_result));
                    vec[3] = 1.0;
                    break;
                }
            case '+':
                {
                    RotMatZ(angle, _rotMatrix);
                    break;
                }
            case '-':
                {
                    RotMatZ(-angle, _rotMatrix);
                    break;
                }
            case '&':
                {
                    RotMatY(angle, _rotMatrix);
                    break;
                }
            case '^':
                {
                    RotMatY(-angle, _rotMatrix);
                    break;
                }
            case '{':
                {
                    RotMatX(angle, _rotMatrix);
                    break;
                }
            case '}':
                {
                    RotMatX(-angle, _rotMatrix);
                    break;
                }
            case '|':
                {
                    // TODO: Rotation der Matrix muss invertiert werden!
                    RotMatY(180.0, _rotMatrix);
                    break;
                }
            case '[':
                {
                    struct PlantStackElement new_item;
                    new_item._rotMatrix = _rotMatrix;
                    new_item._distanceVector = _distanceVector;
                    new_item.old_branch = branch;

                    if(old_index != -1)
                    {
                        new_item.old_index = old_index;
                    }
                    else
                        new_item.old_index = index;

                    new_item.old_vec = vec;

                    osg::ref_ptr<osg::Vec4Array> new_v4array = new osg::Vec4Array;
                    _branches.push_back( new_v4array );

                    branch = _branches.size()-1;
                    _branches[branch]->push_back( vec );

                    // ---------------------------------------- Tree Version

                    new_item.old_branch_node = _currentBranch;
                    // Index des Vektors übergeben den Eltern- und
                    // Kind-Ast gemeinsam haben
                    _currentBranch = _currentBranch->addChild(
                        _currentBranch->getKnotCount()-1, index);

                    // ---------------------------------------- alter Stackkram

                    // push_back erstellt sowieso eine Kopie -> kein Grund
                    // neuen Speicher zu allokieren!
                    _stack.push_back(new_item);

                    break;
                }
            case ']':
                {
                    int size = _stack.size()-1;
                    _distanceVector = _stack[size]._distanceVector;
                    _rotMatrix = _stack[size]._rotMatrix;
                    branch = _stack[size].old_branch;
                    old_index = _stack[size].old_index;
                    vec = _stack[size].old_vec;

                    // wenn der aktuelle Ast nur 1 oder 0 Knoten hat
                    // dann muss er gelöscht werden
                    if( _currentBranch->getKnotCount() <= 1)
                    {
                        BranchNode* parent_branch = _currentBranch->getParentBranch();
                        parent_branch->deleteLastChild();
                    }

                    _currentBranch = _stack[size].old_branch_node;
                    
                    _stack.pop_back();

                    break;
                }
        }

        if (new_i != 0)
        {
            i = new_i;
            new_i = 0;
        }

    }
}

inline void LSysPlant::RotMatX(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( osg::DegreesToRadians(angle),
            osg::Vec3f( 1.0f, 0.0f, 0.0f ) );  // X axis

    mat = mRotationMatrix * mat;
}

inline void LSysPlant::RotMatY(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( osg::DegreesToRadians(angle),
            osg::Vec3f( 0.0f, 1.0f, 0.0f ) );  

    mat = mRotationMatrix * mat;
}

inline void LSysPlant::RotMatZ(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate(
            osg::DegreesToRadians(angle), 
            osg::Vec3f( 0.0f, 0.0f, 1.0f ) );  

    mat = mRotationMatrix * mat;
}

string LSysPlant::getParanthesesContent(int str_pos, string str, int &para_end)
{
    int para_count = 0;
    for(int i = str_pos; i < str.length(); i++)
    {
        if(str[i] == '(')
            para_count++;
        else if(str[i] == ')')
            para_count--;

        if(para_count == 0)
        {
            para_end = i;
            return str.substr(str_pos+1, (i - str_pos)-1);
        }

    }
    // TODO: echte Fehlermeldung/Exception ausgeben/auslösen!
    return "";
}

void LSysPlant::drawPlant(osg::ref_ptr<osg::Geode> &node)
{
    generatePlantWord();
    generatePlant();

    osg::ref_ptr<osg::Geometry> line_loop = new osg::Geometry;
    line_loop->setVertexArray( _vertices.get() );
    line_loop->addPrimitiveSet( _indices.get() );
    osgUtil::SmoothingVisitor::smooth( *line_loop );

    vector<NaturalCubicSpline> branch_splines;
    int element_count = 0;

    _firstBranch.calcBranches( node );
    
    // for(int i = 0; i < _branches.size(); i++)
    // {
    //     element_count = (*_branches[i]).getNumElements();
        
    //     if(element_count == 1)
    //     {
    //         //printf("Branch %d had just 1 vertex!\n", i);
    //         continue;
    //     }

    //     //for(int j = 0; j < element_count; j++)
    //     //{
    //         //printf("%d %f %f %f\n", i, 
    //                 //(*_branches[i])[j].x(),
    //                 //(*_branches[i])[j].y(),
    //                 //(*_branches[i])[j].z());
    //     //}

    //     node->addDrawable(  NaturalCubicSpline(_branches[i],3).drawExtrudedCylinder(3, 0.05f) );
    //     //node->addDrawable(  NaturalCubicSpline(_branches[i]).drawSpline() );
    // }

    /*node->addDrawable( line_loop.get() );*/
}

