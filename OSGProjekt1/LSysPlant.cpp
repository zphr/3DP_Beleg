#include "LSysPlant.h"
using namespace std;

LSysPlant::LSysPlant(
    unsigned int repeats,
    float delta,
    osg::Vec4 distanceVector,
    osg::Matrix rotMatrix,
    FlowerGroup* flower,
    vector<osg::ref_ptr<LeafGeode>> leavesGeodes,
    unsigned int leavesLevel,
    unsigned int leavesCount,
    float leavesDistributionAngle,
    NaturalCubicSpline leavesProfile,
    NaturalCubicSpline leavesSpline,
    map<char,string> rules,
    string startWord,
    string variable)
    : _repeats(repeats),
   _delta(delta),
   _rules(rules),
   _startWord(startWord),
   _distanceVector(distanceVector),
   _rotMatrix(rotMatrix),
   _variable(variable),
   _leavesCount(leavesCount),
   _leavesDistributionAngle(leavesDistributionAngle),
   _leavesProfile(leavesProfile),
   _leavesSpline(leavesSpline)
{
    
    _leavesGeodes = leavesGeodes;
    _leavesLevel = leavesLevel;
    
    _flower = flower;
    _firstBranch = new BranchNode(_flower.get(), _leavesGeodes);
    
    osg::ref_ptr<osg::Vec4Array> new_v4array = new osg::Vec4Array;
    _vertices = new osg::Vec4Array;
    _vertices->push_back( osg::Vec4(0.0, 0.0, 0.0, 1.0) );
    _indices = new osg::DrawElementsUInt( GL_LINES );
}

LSysPlant::~LSysPlant(void)
{ 
    _firstBranch.release();
    _vertices.release();

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
    int branch = 0, level = 0,  new_i = 0;
    string function_str = "";
    double f_result = 1.0;
    float angle = 0.0;

    _firstBranch->addKnot( vec );
    BranchNode* currentBranch = _firstBranch;

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
                    else if((_startWord[i] == 'F') || (_startWord[i] == 'f'))
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

            currentBranch->addKnot( vec );

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
        case '@':
        {
            currentBranch->addFlower(osg::Matrix(_rotMatrix).translate(vec.x(), vec.y(), vec.z()));
            break;
        }
        // Stack Push 
        case '[':
        {
            struct PlantStackElement new_item;
            new_item._posVector      = vec;
            new_item._rotMatrix      = _rotMatrix;
            new_item._distanceVector = _distanceVector;
                
            // ---------------------------------------- Tree Version

            new_item._parentBranch = currentBranch;
            // Index des Vektors übergeben den Eltern- und
            // Kind-Ast gemeinsam haben
            currentBranch = currentBranch->addChildBranch();

            _stack.push_back(new_item);
            break;
        }
        // Stack Pop
        case ']':
        {
            int size        = _stack.size()-1;
            vec             = _stack[size]._posVector; // vorherige Position
            _rotMatrix      = _stack[size]._rotMatrix; // vorherige Rotation
            _distanceVector = _stack[size]._distanceVector; // vorheriger Richtungsvektor

            // wenn der aktuelle Ast nur 1 oder 0 Knoten hat
            // dann muss er gelöscht werden
            if( currentBranch->getKnotCount() <= 1)
            {
                BranchNode* parent_branch = currentBranch->getParentBranch();
                parent_branch->deleteLastChild();
            }

            // Zeiger auf den Eltern-Ast
            currentBranch = _stack[size]._parentBranch;
                
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

    mat = mat * mRotationMatrix;
}

inline void LSysPlant::RotMatY(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate( osg::DegreesToRadians(angle),
                                osg::Vec3f( 0.0f, 1.0f, 0.0f ) );  

    mat = mat * mRotationMatrix;
}

inline void LSysPlant::RotMatZ(float angle, osg::Matrix &mat)
{
    osg::Matrix mRotationMatrix;  
    mRotationMatrix.makeRotate(
        osg::DegreesToRadians(angle), 
        osg::Vec3f( 0.0f, 0.0f, 1.0f ) );  

    mat = mat * mRotationMatrix;
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

osg::Group* LSysPlant::buildPlant()
{
    generatePlantWord();
    generatePlant();

    osg::ref_ptr<BranchVisitor> branch_visitor =
        new BranchVisitor(_leavesLevel,
                          _leavesCount,
                          _leavesDistributionAngle,
                          &_leavesProfile,
                          &_leavesSpline);

    branch_visitor->apply(*(_firstBranch.get()));
    
    return _firstBranch.get();
}

