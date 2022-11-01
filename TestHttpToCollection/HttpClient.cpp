#include"HttpClient.h"

// int HttpClient::GetResultCode()
// {
//     return this->resultCode;
// }

// string HttpClient::GetResultMessage()
// {
//     return this->resultMessage;
// }

// string HttpClient::GetToken()
// {
//     return this->token;
// }

// void HttpClient::PutDeviceUuid(string deviceUuid00)
// {
//     this->deviceUuid=deviceUuid00;
// }

// void HttpClient::PutDeviceSecret(string deviceSecret00)
// {
//     this->deviceSecret=deviceSecret00;
// }

void HttpClient::PutUrl(string url00)
{
    this->url=url00;
}
string HttpClient::GetReciveMessage()
{
  return this->m_reciveMessage_;
}
// bool HttpClient::SendMessage()
// {   
//   try {  
//     request.setOpt(new curlpp::options::Url(url)); 
//     request.setOpt(new curlpp::options::Verbose(true));  
//     header.push_back("Content-Type: text/plain"); 
//     request.setOpt(new curlpp::options::HttpHeader(header)); 
//     struct json_object *obj = json_object_new_object();// 创建一个 json 对象
//     json_object_object_add(obj, "deviceUuid", json_object_new_string(deviceUuid.c_str()));
// 	  json_object_object_add(obj, "deviceSecret", json_object_new_string(deviceSecret.c_str()));
//     const char* buf = json_object_to_json_string(obj); 
//     request.setOpt(new curlpp::options::PostFields(string(buf)));
//     request.setOpt(new curlpp::options::PostFieldSize(strlen(buf)+1));
//     request.perform(); 
//     //cout << "-----message:" << request << std::endl;
//     std::stringstream sstream;
//     sstream<<request;
//     string  Receivedata;
//     sstream >> Receivedata;
//     cout <<endl<< "Recive Message From Server:"<< Receivedata << std::endl;
//     AnalysisString(Receivedata);
//   }
//   catch ( curlpp::LogicError & e ) {
//     //std::cout << e.what() << std::endl;
//     std::cout << "LogicError!" << std::endl;
//     return false;
//   }
//   catch ( curlpp::RuntimeError & e ) {
//     //std::cout << e.what() << std::endl;
//     std::cout << "RuntimeError!" << std::endl;
//     return false;
//   }
//   return true;
// }

bool HttpClient::SendMessage(string m_message)
{
    
  try {
    
    request.setOpt(new curlpp::options::Url(url)); 
    request.setOpt(new curlpp::options::Verbose(true)); 
    
    header.push_back("Content-Type: text/plain"); 
    request.setOpt(new curlpp::options::HttpHeader(header)); 

    // json JsonObj;
    // JsonObj["deviceUuid"] = deviceUuid;
    // JsonObj["deviceSecret"] = deviceSecret;
    // string JsonStr= JsonObj.dump();   
    string JsonStr= m_message;
    request.setOpt(new curlpp::options::PostFields(JsonStr));
    request.setOpt(new curlpp::options::PostFieldSize(JsonStr.length()+1));
    // request.perform();  //20221031 //Solve the problem of sending data twice

    //cout << "-----message:" << request << std::endl;
    std::stringstream sstream;
    sstream<<request;
    string  Receivedata;
    sstream >> Receivedata;
    this->m_reciveMessage_=Receivedata;
    cout <<endl<< "Recive Message From HTTP:"<< Receivedata << std::endl;

   // AnalysisString(Receivedata);
  }
  catch ( curlpp::LogicError & e ) {
    std::cout << e.what() << std::endl;
    std::cout << "LogicError!" << std::endl;
    return false;
  }
  catch ( curlpp::RuntimeError & e ) {
    std::cout << e.what() << std::endl;
    std::cout << "RuntimeError!" << std::endl;
    return false;

  }

  return true;
}


// void HttpClient::AnalysisString(string message)
// {
//     //解析JSON数据
// 	struct json_object *obj = json_tokener_parse(message.c_str());// 将符合 json 格式的字符串 转为 json 对象 
// 	struct json_object *json;
// 	json_type type;
//     json_object_object_get_ex(obj, "resultCode", &json);// 根据键名 解析出 json 对象
// 	type = json_object_get_type(json);// 判断解析出的 json 对象 是什么类型
// 	if(type == json_type_int){
//         this->resultCode = json_object_get_int(json);
//         printf("resultCode: %d\n", this->resultCode);
//     }
//     json_object_object_get_ex(obj, "resultMessage", &json);// 根据键名 解析出 json 对象
// 	type = json_object_get_type(json);// 判断解析出的 json 对象 是什么类型
// 	if(type == json_type_string){
//         this->resultMessage= json_object_get_string(json);
// 	printf("resultMessage: %s\n",this->resultMessage.c_str());
//     }
//     json_object_object_get_ex(obj, "token", &json);// 根据键名 解析出 json 对象
// 	type = json_object_get_type(json);// 判断解析出的 json 对象 是什么类型
// 	if(type == json_type_string){
//         this->token= json_object_get_string(json);
// 	printf("token: %s\n",this->token.c_str());
//     }   
// }

void HttpClient::AnalysisString(string message)
{
    //解析JSON数据
    json JsonObj;
    stringstream sstream;
    sstream<<message;
    sstream >> JsonObj;

this->resultCode = JsonObj["resultCode"];
this->resultMessage= JsonObj["resultMessage"];
this->token= JsonObj["token"];

printf("resultCode: %d\n", this->resultCode);
printf("resultMessage: %s\n",this->resultMessage.c_str());
printf("token: %s\n",this->token.c_str());
	    
}