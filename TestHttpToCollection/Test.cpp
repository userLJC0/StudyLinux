#include "HttpClient.h"
#include <functional>
json stringToJson(string str)
{
	json jsonObj = str;
	auto j = json::parse(jsonObj.get<std::string>());
	return j;
}
int main()
{
	HttpClient m_httpClientToCollection;

	string m_strDataFromCollection;
	json m_jsonDataFromCollection;
	string m_strnDataToCollection;
	json m_jsonDataToCollection;

	m_httpClientToCollection.PutUrl("http://192.168.4.244:8080/sendCommand");
    m_jsonDataToCollection["command"] = "GETINFO";
   m_strnDataToCollection= m_jsonDataToCollection.dump();   
	if(m_httpClientToCollection.SendMessage(m_strnDataToCollection))
  {
	m_strDataFromCollection =m_httpClientToCollection.GetReciveMessage();
    cout<<"HTTP数据发送成功!接收到的数据为："<<m_strDataFromCollection<<endl;

	//解析JSON数据
   m_jsonDataFromCollection=stringToJson(m_strDataFromCollection);
   cout<<"解析json数据为:"<<endl;
	//m_jsonDataToCollection.clear();
	for (auto& el : m_jsonDataFromCollection.items()) {
      std::cout << el.key() << " : " << el.value() << "\n";
    }
	cout<<"------------------"<<endl;

  }else cout<<"HTTP数据发送失败!"<<endl;


	return 0;
}
