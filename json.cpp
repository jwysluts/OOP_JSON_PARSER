/*
Project: C++ JSON Parser
Description: This program will read through a JSON file, parse the data, and output the weight of the values in the file.
Programmer: Joachim Wyslutsky
Due Date: 3/7/2016
*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


struct Value
{
    virtual int weight()=0;

    virtual Value* parse(std::string, int&) =0;

    virtual ~Value()=default;
};

struct Null: public Value
{
    std::string null;
    Value* parse(std::string jsonData, int &k)
    {
        null="";
        while(jsonData[k]!=','&&jsonData[k]!='}'&&jsonData[k]!=']')
        {
            null+=jsonData[k];
            k++;
        }
        return this;
    }
    int weight()
    {
        return 1;
    }
};

struct Bool: public Value
{
    std:: string boolean;
    Value* parse(std::string jsonData, int &k)
    {
        boolean = "";
        while(jsonData[k]!=','&& jsonData[k]!='}' && jsonData[k]!=']')
        {
            boolean += jsonData[k];
            k++;
        }
        return this;
    }

    int weight()
    {
        return 1;
    }
};

struct String: public Value, std::string
{
    std::string str;
    Value* parse(std::string jsonData, int &k)
    {
        str="";
        k++;
        while(jsonData[k]!='"')
        {
            if(jsonData[k]=='\\')
                k++;
        }
    }

    int weight()
    {
        return 1;
    }
};

struct Number: public Value
{
    std::string number;

    Value* parse(std::string jsonData, int &k)
    {
        number = "";
        while(jsonData[k]=='-' || isdigit(jsonData[k]) || jsonData[k]=='e' || jsonData[k]=='.')
        {
            number+=jsonData[k];
            k++;
        }
        return this;
    }

    int weight()
    {
        return 1;
    }
};

struct Array: public Value, std::vector<Value*>
{
    Value* parse(std::string, int&);

    int weight()
    {
        int total=1;
        int vSize=this->size();

        for(int i=0; i<vSize; i++)
        {
            total += this[i].weight();
        }

        return total;
    }
};

struct Object: public Value, std::vector<std::pair<std::string,Value*>>
{
    Value* parse(std::string jsonData, int& k)
    {
        //Declare values that may be contained in object
        Object contObject;
        Array contArray;
        Number contNumber;
        Bool contBool;
        Null contNull;
        String contString;

        std::string valueName="";
        std::pair<std::string, Value*> orderedPair;
        while(jsonData[k]!='}')
        {
            while (valueName=="")
            {
                k++;
                if(jsonData[k]=='"')
                {
                    k++;
                    while(jsonData[k]!='"')
                        {
                            if(jsonData[k]=='\\')
                            {
                                k++;
                            }
                            valueName+=jsonData[k];
                            k++;
                        }
                }
            }
            if(jsonData[k]=='"')
            {
                orderedPair= std::make_pair(valueName,contString.parse(jsonData,k));
                this->push_back(orderedPair);
                valueName = "";
            }

            else if(jsonData[k]=='-' || isdigit(jsonData[k]))
            {
                orderedPair=std::make_pair(valueName,contNumber.parse(jsonData,k));
                this->push_back(orderedPair);
                valueName = "";
            }
            else if(jsonData[k]=='t'||jsonData[k]=='f')
            {
                orderedPair = std::make_pair(valueName,contBool.parse(jsonData,k));
                this->push_back(orderedPair);
                valueName = "";
            }
            else if(jsonData[k]=='n')
            {
                orderedPair = std::make_pair(valueName,contNull.parse(jsonData,k));
                this->push_back(orderedPair);
                valueName = "";
            }
            else if(jsonData[k]=='[')
            {
                orderedPair = std::make_pair(valueName,contArray.parse(jsonData,k));
                this->push_back(orderedPair);
                valueName = "";
            }
            else if(jsonData[k]=='{')
            {
                orderedPair = std::make_pair(valueName,contObject.parse(jsonData, k));
                this->push_back(orderedPair);
                valueName = "";
            }

            k++;
        }
}

    int weight()
    {
        int total = 1;
        std::vector<std::pair<std::string, Value*>>::iterator it;
        Value* temp;
        for(it=this->begin(); it<this->end();it++)
        {
            total+=it->second->weight();
        }
        return total;
    }
};

Value* Array::parse(std::string jsonData, int &k)
{
    //Declare values that may be contained in Array
        Object contObject;
        Array contArray;
        Number contNumber;
        Bool contBool;
        Null contNull;
        String contString;

        std::string valueName;

        while(jsonData[k]!=']')
        {
            if(jsonData[k]=='"')
            {
                this->push_back(contString.parse(jsonData,k));
            }
            else if(jsonData[k]=='-'||isdigit(jsonData[k]))
            {
                this->push_back(contNumber.parse(jsonData,k));
            }
            else if(jsonData[k]=='t'||jsonData[k]=='f')
            {
                this->push_back(contBool.parse(jsonData,k));
            }
            else if(jsonData[k]=='n')
            {
                this->push_back(contNull.parse(jsonData,k));
            }
            else if(jsonData[k]=='[')
            {
                this->push_back(contArray.parse(jsonData,k));
            }
            else if(jsonData[k]=='{')
            {
                this->push_back(contObject.parse(jsonData, k));
            }

            k++;
        }
        return this;
}

int main(int argc, char*argv[])
{
    //Declare values
    Bool jbool;
    String jstring;
    Array jarray;
    Null jnull;
    Object jobject;
    Number jnumber;

    //Vector to hold the values
    std::vector<Value*> jstructure;

    int weight = 0;

    std::string fileName = argv[1];
    std::ifstream json;

    json.open(fileName.c_str(),std::ios::binary);

    std::streampos begin, end;

    begin=json.tellg();
    json.seekg(0,std::ios::end);
    end=json.tellg();

    std::streampos size = end-begin;
    json.seekg(0,std::ios::beg);

    char * fileData = new char[size];
    json.read(fileData, size);
    fileData[size]='\0';
    json.close();

    std::string jsonData(fileData);

    int jsonSize =jsonData.size();

    for(int k=0; k<jsonSize;k++)
    {
        switch(jsonData[k])
        {
            case '{':
                jstructure.push_back(jobject.parse(jsonData, k));
                break;

            case '[':
                jstructure.push_back(jarray.parse(jsonData,k));
                break;

            case '"':
                jstructure.push_back(jstring.parse(jsonData,k));
                break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':jstructure.push_back(jnumber.parse(jsonData,k));
                break;

            case 't':
            case 'f':jstructure.push_back(jbool.parse(jsonData,k));
                break;

            case 'n':jstructure.push_back(jnull.parse(jsonData,k));
                break;

            default:
                break;
        }
    }

    int total=0;
    int vSize=jstructure.size();
    std::vector<Value*>::iterator it;

    for(int i=0; i<vSize;i++)
    {
        total+=jstructure[i]->weight();
    }

    std::cout<<"Weight of file " << fileName<<" is: "<<total<<std::endl;

    return 0;
}
