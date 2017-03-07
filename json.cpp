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
    virtual int weight()
    {
        return 1;
    }

    virtual Value* parse(std::string, int&, int&) =0;

    virtual ~Value()=default;
};

struct Null: public Value
{
    std::string null;
    Value* parse(std::string jsonData, int &k, int &test)
    {
        null="";
        while(jsonData[k]!=','&&jsonData[k]!='}'&&jsonData[k]!=']')
        {
            null+=jsonData[k];
            k++;
        }
        test++;
        return this;
    }
};

struct Bool: public Value
{
    std:: string boolean;
    Value* parse(std::string jsonData, int &k, int &test)
    {
        boolean = "";
        while(jsonData[k]=','&& jsonData[k]!='}' && jsonData[k]!=']')
        {
            boolean += jsonData[k];
            k++;
        }
        test++;
        return this;
    }
};

struct String: public Value
{
    std::string str;
    Value* parse(std::string jsonData, int &k, int &test)
    {
        str="";
        k++;
        while(jsonData[k]!='"')
        {
            if(jsonData[k]=='\\')
                k++;
            str+=jsonData[k];
            k++;
        }
        test++;
        return this;
    }
};

struct Number: public Value
{
    std::string number;

    Value* parse(std::string jsonData, int &k, int &test)
    {
        number = "";
        while(jsonData[k]=='-' || isdigit(jsonData[k]) || jsonData[k]=='e' || jsonData[k]=='.')
        {
            number+=jsonData[k];
            k++;
        }
        test++;
        return this;
    }
};

struct Array: public Value, std::vector<Value*>
{
    std::vector<Value*> jsonArray;
    Value* parse(std::string, int&,int&);

    int weight();
};

struct Object: public Value
{
    std::vector<std::pair<std::string, Value*>> jsonObject;
    Value* parse(std::string jsonData, int& k, int &test)
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
        int counter=0;

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
                    k++;
                }
            }
            if(jsonData[k]=='"')
            {
                orderedPair= std::make_pair(valueName,contString.parse(jsonData,k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }

            else if(jsonData[k]=='-' || isdigit(jsonData[k]))
            {
                orderedPair=std::make_pair(valueName,contNumber.parse(jsonData,k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(jsonData[k]=='t'||jsonData[k]=='f')
            {
                orderedPair = std::make_pair(valueName,contBool.parse(jsonData,k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(jsonData[k]=='n')
            {
                orderedPair = std::make_pair(valueName,contNull.parse(jsonData,k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(jsonData[k]=='[')
            {
                orderedPair = std::make_pair(valueName,contArray.parse(jsonData,k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(jsonData[k]=='{')
            {
                orderedPair = std::make_pair(valueName,contObject.parse(jsonData, k,test));
                jsonObject.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            if(jsonData[k]=='}')
            {
                test++;
                return this;
            }
            k++;
        }
        test++;
        test+=counter;
        return this;
}

    int weight()
    {
        std::cout<<"Calculating Object Weight..."<<std::endl;
        int total = 1;

        for(int i=0; i<jsonObject.size();i++)
        {
            total+=jsonObject[i].second->weight();
        }
        return total;
    }
};

Value* Array::parse(std::string jsonData, int &k,int& test)
{
    //Declare values that may be contained in Array
        Object contObject;
        Array contArray;
        Number contNumber;
        Bool contBool;
        Null contNull;
        String contString;

        std::string valueName;
        int counter = 0;

        while(jsonData[k]!=']')
        {
            k++;
            if(jsonData[k]=='"')
            {
                jsonArray.push_back(contString.parse(jsonData,k,test));
                counter++;
            }
            else if(jsonData[k]=='-'||isdigit(jsonData[k]))
            {
                jsonArray.push_back(contNumber.parse(jsonData,k,test));
                counter++;
            }
            else if(jsonData[k]=='t'||jsonData[k]=='f')
            {
                jsonArray.push_back(contBool.parse(jsonData,k,test));
                counter++;
            }
            else if(jsonData[k]=='n')
            {
                jsonArray.push_back(contNull.parse(jsonData,k,test));
                counter++;
            }
            else if(jsonData[k]=='[')
            {
                jsonArray.push_back(contArray.parse(jsonData,k,test));
                counter++;
            }
            else if(jsonData[k]=='{')
            {
                jsonArray.push_back(contObject.parse(jsonData, k,test));
                counter++;
            }
        }
        test++;
        test+=counter;
        return this;
}

int Array::weight()
{
    int total=1;
    int vSize=this->size();

    for(int i=0; i<vSize; i++)
    {
        total += this[i].weight();
    }

    return total;
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

    int total=0;

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

    int k=0;
    std::cout<<"Begin Processing of JSON File"<<std::endl;
    do
    {
        switch(jsonData[k])
        {
            case '{':
                jstructure.push_back(jobject.parse(jsonData, k, total));
                break;

            case '[':
                jstructure.push_back(jarray.parse(jsonData,k, total));
                break;

            case '"':
                jstructure.push_back(jstring.parse(jsonData,k, total));
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
            case '9':jstructure.push_back(jnumber.parse(jsonData,k,total));
                break;

            case 't':
            case 'f':jstructure.push_back(jbool.parse(jsonData,k,total));
                break;

            case 'n':jstructure.push_back(jnull.parse(jsonData,k,total));
                break;

            default:
                break;
        }
        k++;
    }
    while(k<jsonSize);

    std::cout<<"Finished Processing"<<std::endl;


    int vSize=jstructure.size();
/*
    std::cout<<"Calculating Weight..."<<std::endl;

    for(int i=0; i<vSize;i++)
    {
        total+=jstructure[i]->weight();
    }

*/

    std::cout<<"Weight of file " << fileName <<" is: "<<total<<std::endl;

    return 0;
}
