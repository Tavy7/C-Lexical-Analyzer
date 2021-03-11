#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Analyzer
{
    vector<string> Keywords{ "auto", "and", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "class",
        "const", "continue", "cin", "cout" , "default", "delete", "do",
        "double", "else", "enum", "endl", "false", "float", "for",
        "file", "ifstream", "if", "int","include", "long", "namespace",
        "new", "not", "or", "private", "protected", "push_back",
        "pop_back", "public", "return", "short", 
        "sizeof", "static", "static_cast", "struct", "string", "switch",
        "this", "throw", "true", "try", "typdef", "typename", 
        "union", "unisgned", "vector", "virtual", "void", "while", "xor" };
    vector<string> Operators{ "<<", ">>", "<", ">", "<=", ">=", "*", "+", "-",
        "/", "=", "-=", "*=", "+=", "/=", "++", "--", "==", ":", "&", "#", "::",
        "&&", "||", "!", "!="};
    vector<string> Separators{ "{", "}", ",", "(", ")", ";", ".", "[", "]"};

    // Var constanta inseamna read only
    // pentru a fi constanta cu adevarat ca sa fie folosita in switch
    // folosim static constexpr
    static constexpr short int Comment = 10;
    static constexpr short int Operator = 11;
    static constexpr short int Separator = 12;
    static constexpr short int Keyword = 13;
    static constexpr short int String = 14;
    static constexpr short int Boolean = 15;
    static constexpr short int Number = 16;
    static constexpr short int Identifier = 17;
    static constexpr short int Characther = 18;
    static constexpr short int Unknown = -10;
    static constexpr short int IncompleteString = -11;

    bool isComment(string& str)
    {
        return str == "/*" || str == "//";
    }
    
    bool isIdentifier(const std::string& str)
    {
        if (isdigit(str[0]))
            return false;
        unsigned int counter = 0;
        if (str[0] == '_')
            counter++;

        for (; counter < str.size(); counter++)
            if (!(isalnum(str[counter])))
                return false;

        return true;
    }
    // Verifica daca str e numar
    bool isNumber(string& str)
    {
        for (auto el : str)
        {
            if (!isdigit(el))
            {
                return false;
            }
        }
        return true;
    }
    
    bool isString(string& str)
    {
        return str[0] == '"' && str[str.size() - 1] == '"';
    }

    bool isChar(string& str)
    {
        return str[0] == '\'' && str[str.size() - 1] == '\'';
    }

    bool isBool(string& str)
    {
        return str == "true" || str == "false";
    }

    bool isKeyword(string& str)
    {
        for (string keyword : Keywords)
        {
            if (str == keyword)
                return true;
        }
        return false;
    }

    bool isOperator(string& str)
    {
        for (string op : Operators)
        {
            if (str == op)
                return true;
        }
        return false;
    }

    bool isSeparator(string& str)
    {
        for (string separator : Separators)
        {
            if (str == separator)
                return true;
        }
        return false;
    }

    int AnalyzeToken(string Token)
    {
        if (isComment(Token))
            return Comment;
        if (isString(Token))
            return String;
        if (isSeparator(Token))
            return Separator;
        if (isOperator(Token))
            return Operator;
        if (isChar(Token))
            return Characther;
        if (isBool(Token))
            return Boolean;
        if (isNumber(Token))
            return Number;
        if (isKeyword(Token))
            return Keyword;
        if (isIdentifier(Token))
            return Identifier;
        if (Token[0] == '\"' && Token[Token.size() - 1] != '\"')
            return IncompleteString;
            
        return Unknown;
    }

};

// Transforma un string in tokens in functie de separatorul dat
vector<string> TokenizeWordsFromString(string Sentence, char Separator = ' ')
{
    // Vector pentru cuvinte
    vector<string> Tokens;
    // String pe care il folosesc pentru a extrage cuvintele
    string word = "";
    Analyzer analyzer = Analyzer();

    for (char chr : Sentence)
    {
        string chrButString = "";
        chrButString += chr;

        // Daca caracterul curent e separator sau operator
        bool specialCharacther = analyzer.isSeparator(chrButString) || analyzer.isOperator(chrButString);
        if (specialCharacther == true)
        {
            // Daca cuvantul nu este gol considera <vector> ca ca op <, cuv vector, op >
            // daca scoatem a doua conditie, il considera ca op <> si cuvantul vector
            if (Tokens.size() > 0 && word == "")
            {
                // Daca ultimul token a fost operator si cel curent este operator, ii combinam
                bool chrIsOperator = analyzer.isOperator(chrButString);
                string lastToken = Tokens[Tokens.size() - 1];
                bool lastTokenIsOperator = analyzer.isOperator(lastToken);
                if (chrIsOperator && lastTokenIsOperator)
                {
                    Tokens.pop_back();
                    Tokens.push_back(lastToken + chrButString);
                    continue;
                }
            }

            if (word != "")
            {
                Tokens.push_back(word);
                word = "";
            }

            Tokens.push_back(chrButString);
            continue;
        }

        // Daca caracterul curent este separaor
        if (chr == Separator)
        {
            if (word == "")
                continue;
            // Adaugam cuvantul in lista, si eliberam variabila pt urmatorul cuvant
            Tokens.push_back(word);
            word = "";
        }
        else
        {
            word += chr;
        }
    }
    
    // Adaugam si ultimul cuvant in lista daca este nenul
    if (word != "")
    {
        Tokens.push_back(word);
    }
    return Tokens;

}

vector<pair<int, string>> ScanData(vector<string> Data)
{
    vector<pair<int, string>> UnknownTokens;
    Analyzer analyzer = Analyzer();

    for (unsigned int lineNumber = 0; lineNumber < Data.size(); lineNumber++)
    {
        cout << "\nLine " << lineNumber << " " << Data[lineNumber]<<endl;
        
        vector<string> Tokens = TokenizeWordsFromString(Data[lineNumber]);
        pair<int, string> UnknownTokenPair;

        for (string token : Tokens)
        {
            cout << token << " ";
            int result = analyzer.AnalyzeToken(token);

            if (result == analyzer.Comment)
            {
                cout << "Comentariu\n";
                break;
            }

            switch (result)
            {
            case analyzer.Operator:
                cout << "Operator\n";
                break;
            case analyzer.Separator:
                cout << "Separator\n";
                break;
            case analyzer.Keyword:
                cout << "Keyword\n";
                break;
            case analyzer.String:
                cout << "String\n";
                break;
            case analyzer.Boolean:
                cout << "Boolean\n";
                break;
            case analyzer.Number:
                cout << "Number\n";
                break;
            case analyzer.Identifier:
                cout << "Identifier\n";
                break;
            case analyzer.Characther:
                cout << "Caracter\n";
                break;
            case analyzer.Unknown:
                cout << "Necunoscut\n";
                UnknownTokenPair = make_pair(lineNumber, token);
                UnknownTokens.push_back(UnknownTokenPair);
                break;
            case analyzer.IncompleteString:
                cout << "String gresit\n";
                UnknownTokenPair = make_pair(lineNumber, token);
                UnknownTokens.push_back(UnknownTokenPair);
                break;
            };
        }
    }

    return UnknownTokens;
}

// Citim datele din fisier si le memoram intr-un vector de stringuri
vector<string> ReadDataFromFile(string FileName = "Date.txt")
{
    vector<string> Data;
    ifstream File(FileName);

    if (File.is_open())
    {
        string Line;
        while (getline(File, Line))
        {
            Data.push_back(Line);
        }
        File.close();
    }
    return Data;
}

int main()
{
    vector<string> Data = ReadDataFromFile();
    vector<pair<int, string>> UnknownTokens = ScanData(Data);

    if (UnknownTokens.size() > 0)
        cout << "\n\nErrors:\n";

    for (pair<int, string> it : UnknownTokens)
    {
        cout << it.first << " " << it.second << " unknown word\n";
    }
    return 0;
}
