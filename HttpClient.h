#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <string>
#include <sstream>
// extern "C"{
// #include "json-c/json.h"
// }
#include <fstream>
#include <nlohmann/json.hpp>
#include<sstream>
using json = nlohmann::json;
using namespace std;

class HttpClient{

public:
    void PutUrl(string url00);
    // void PutDeviceUuid(string deviceUuid00);
    // void PutDeviceSecret(string deviceSecret00);
    // int GetResultCode();
    // string GetResultMessage();
    // string GetToken();
    
    bool SendMessage(string m_message);
    string GetReciveMessage();
private:

    //Initialization parameters
    string deviceUuid;
    string deviceSecret;
    string url;

    //Data returned by the server
    string m_reciveMessage_;
    int resultCode;
    string resultMessage;
    string token;

    //Curlpp variable
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    std::list<std::string> header;

    void AnalysisString(string message);
};