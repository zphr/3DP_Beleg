#include "LSysPlant.h"
using namespace std;

LSysPlant::LSysPlant(
    unsigned int       repeats,
    float              delta,
    osg::Vec4          distanceVector,
    osg::Matrix        rotMatrix,
    float              baseScale,
    float              relativeLevelScale,
    unsigned int       translationJitter,
    unsigned int       rotationJitter,
    string             branchImgPath,
    NaturalCubicSpline branchProfile,
    float              baseFlowerScale,
    float              relativeFlowerScale,
    FlowerGroup*       flower,
    vector<osg::ref_ptr<LeafGeode>> leavesGeodes,
    unsigned int       leavesLevel,
    unsigned int       leavesCount,
    float              leavesDistributionAngle,
    float              leavesBaseScale,
    float              leavesRelativeScale,
    NaturalCubicSpline leavesProfile,
    NaturalCubicSpline leavesSpline,
    map<char,string>   rules,
    string             startWord,
    string             variable)
    : _repeats(repeats),
   _delta(delta),
   _rules(rules),
   _startWord(startWord),
   _distanceVector(distanceVector),
   _rotMatrix(rotMatrix),
   _baseScale(baseScale),
   _translationJitter(translationJitter),
   _rotationJitter(rotationJitter),
   _relativeLevelScale(relativeLevelScale),
   _branchProfile(branchProfile),
   _baseFlowerScale(baseFlowerScale),
   _relativeFlowerScale(relativeFlowerScale),
   _variable(variable),
   _leavesCount(leavesCount),
   _leavesDistributionAngle(leavesDistributionAngle),
   _leavesBaseScale(leavesBaseScale),
   _leavesRelativeScale(leavesRelativeScale),
   _leavesProfile(leavesProfile),
   _leavesSpline(leavesSpline)
{
    osg::ref_ptr<osg::Image> branch_img = osgDB::readImageFile(branchImgPath);

    _branchTex = new osg::Texture2D();
    _branchTex->setWrap(osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT);
    _branchTex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
    _branchTex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    _branchTex->setImage( branch_img.release() );

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
inline void LSysPlant::randomizeTranslation(double &length)
{
    float random_trans;
    float random_pos_neg;

    random_pos_neg = (1.0-((rand()%2)*2));
    random_trans = (rand() % _translationJitter)/100.0 * random_pos_neg;
    length += length * random_trans;
}

inline void LSysPlant::randomizeRotation(float angle, char direction)
{
    float random_rot;
    float random_pos_neg;

    float rand_angle = angle * _rotationJitter/100.0;
    random_pos_neg = (1.0-((rand()%2)*2));

    random_rot = (rand() % _rotationJitter)/100.0 * random_pos_neg;
    rand_angle += rand_angle * random_rot;

    if((direction == 'x') || (direction == 'X'))
        RotMatX(rand_angle, _rotMatrix);

    if((direction == 'y') || (direction == 'Y'))
        RotMatY(rand_angle, _rotMatrix);

    if((direction == 'z') || (direction == 'Z'))
        RotMatZ(rand_angle, _rotMatrix);

}

inline void LSysPlant::randomizeRotAllDirections(float angle)
{
    randomizeRotation(angle, 'X');
    randomizeRotation(angle, 'Y');
    randomizeRotation(angle, 'Z');
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

    srand ( time(NULL) );

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
            randomizeTranslation(f_result);
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
            randomizeRotAllDirections(angle);
            RotMatZ(angle, _rotMatrix);
            break;
        }
        case '-':
        {
            randomizeRotAllDirections(angle);
            RotMatZ(-angle, _rotMatrix);
            break;
        }
        case '&':
        {
            randomizeRotAllDirections(angle);
            RotMatY(angle, _rotMatrix);
            break;
        }
        case '^':
        {
            randomizeRotAllDirections(angle);
            RotMatY(-angle, _rotMatrix);
            break;
        }
        case '{':
        {
            randomizeRotAllDirections(angle);
            RotMatX(angle, _rotMatrix);
            break;
        }
        case '}':
        {
            randomizeRotAllDirections(angle);
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
            osg::Matrix mat(osg::Matrix().inverse(_rotMatrix));
            // mat.postMult(osg::Matrix().translate(vec.x(), vec.y(), vec.z()));
            currentBranch->addFlower(mat,
                                     osg::Vec3(vec.x(), vec.y(), vec.z()),
                                     _baseFlowerScale * pow(_relativeFlowerScale, (float) level)
                                    );
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
            level++;
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
            level--;
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
        new BranchVisitor(_baseScale,
                          _relativeLevelScale,
                          3,
                          6,
                          _branchTex.get(),
                          &_branchProfile,
                          _leavesLevel,
                          _leavesCount,
                          _leavesDistributionAngle,
                          _leavesBaseScale,
                          _leavesRelativeScale,
                          &_leavesProfile,
                          &_leavesSpline);

    branch_visitor->apply(*(_firstBranch.get()));
    
    return _firstBranch.get();
}

