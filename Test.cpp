#include "WebsocketClient.h"
#include "HttpClient.h"
#include <functional>
using namespace std;
json stringToJson(string str)
{
	json jsonObj = str;
	auto j = json::parse(jsonObj.get<std::string>());
	return j;
}
struct WebsocketMessage
{
	HttpClient m_httpClientToServer;
	HttpClient m_httpClientToCollection;
	WebsocketClient m_websocketClientObject;
	string m_websocketClientUrl;
	bool isGettoken_;
	string token_;
	bool isOnConnection_;					// websocket连接是否存在
	bool isReciveMessage_;					// websocket服务器是否发送信息
	string messageFromServer_;				// websocket服务器发送的信息
	WebsocketMessage(bool m_isConnection, bool m_isSendMes, bool m_isGettoken, string m_message, string m_message2) : isOnConnection_(m_isConnection), isReciveMessage_(m_isSendMes), isGettoken_(m_isGettoken), messageFromServer_(m_message), token_(m_message2) {}
	void websocketCloseFunc();				// websocket连接关闭时调用
	void setMessage(const string &message); // websocket服务器设置收到的数据
	void WssConnectOpenFun();				// Wss连接成功
	void WssConnectFailFun();				// Wss连接失败
	void WebsocketInitialization(string ipToServer,string ipTocollection);	//初始化操作
	void connectionAndLogin();				// http获取token并且创建wss连接
};
void WebsocketMessage::connectionAndLogin()
{
	json m_jsonDataToServer;
	m_jsonDataToServer["deviceUuid"] = "device_uid111";
	m_jsonDataToServer["deviceSecret"] = "device_secret111";
	string m_strnDataToServer = m_jsonDataToServer.dump();
	cout << "HTTP向客户端发送数据 : " << m_strnDataToServer << endl;
	if (m_httpClientToServer.SendMessage(m_strnDataToServer) == false) {
		cout << "HTTP数据发送失败!正在尝试重新发送........" << endl;
		int m_index = 3;
		bool m_isSuccess = false;
		while (m_index--) {
			if (m_httpClientToServer.SendMessage(m_strnDataToServer)) {
				m_isSuccess = true;
				break;
			}
			else
				std::cout << "HTTP数据发送失败!正在尝试重新发送........" << endl;
		}
		if (m_isSuccess == false) {
			std::cout << "HTTP数据发送失败!退出程序" << endl;
			exit(0);
		}
	}
	{
		string m_strDataFromServer = m_httpClientToServer.GetReciveMessage();
		cout << "HTTP数据发送成功! 接收到的数据为：" << m_strDataFromServer << endl;
		json m_jsonDataFromServer = stringToJson(m_strDataFromServer);
		m_jsonDataToServer.clear();
		

		// m_jsonDataToServer["token"] = m_jsonDataFromServer["token"];
		m_jsonDataToServer["token"] = "123";

		
		this->m_websocketClientObject.Connect(m_websocketClientUrl);
		if (this->isOnConnection_ == false) {
			cout << "Wss连接失败!正在尝试重新连接........" << endl;
			int m_index = 3;
			bool m_isSuccess = false;
			while (m_index--) {
				this->m_websocketClientObject.Connect(m_websocketClientUrl);
				if (this->isOnConnection_) {
					cout << "Wss重新连接成功!" << endl;
					m_isSuccess = true;
					break;
				}
			}
			if (m_isSuccess == false) {
				cout << "----------------------------------------" << endl;
				cout << "Wss连接失败!退出程序" << endl;
				exit(0);
			}
		}
		this->m_websocketClientObject.Send(m_jsonDataToServer.dump()); // WSS 发送token
	}
}
void WebsocketMessage::WebsocketInitialization(string ipToServer,string ipTocollection)
{//159.138.146.46
//192.168.4.162
	this->m_httpClientToServer.PutUrl("http://" + ipToServer + ":8088/cameraLogin");
	this->m_httpClientToCollection.PutUrl("http://" + ipTocollection + ":8080/sendCommand");
	this->m_websocketClientUrl = "ws://" + ipToServer + ":8089/ws";

	//注册回调函数
	//注册wss连接成功回调函数
	std::function<void()> m_OnConnectionOpenFunc = bind(&WebsocketMessage::WssConnectOpenFun, &(*this));
	this->m_websocketClientObject.SetOnOpenFunc(m_OnConnectionOpenFunc);
	//注册wss连接失败回调函数
	std::function<void()> m_OnConnectionFailFunc = bind(&WebsocketMessage::WssConnectFailFun, &(*this));
	this->m_websocketClientObject.SetOnFailFunc(m_OnConnectionFailFunc);
	//注册wss关闭回调函数
	std::function<void()> m_OnCloseFunc = bind(&WebsocketMessage::websocketCloseFunc, &(*this));
	this->m_websocketClientObject.SetOnCloseFunc(m_OnCloseFunc);
	//注册wss接收数据回调函数
	std::function<void(const std::string &message)> m_onSetMessage = bind(&WebsocketMessage::setMessage, &(*this), placeholders::_1);
	this->m_websocketClientObject.SetMessageFunc(m_onSetMessage);
}
void WebsocketMessage::websocketCloseFunc() // websocket连接关闭时调用
{
	this->isOnConnection_ = false;
	this->connectionAndLogin(); //重新申请连接
}
void WebsocketMessage::setMessage(const string &message)
{
	this->isReciveMessage_ = true;
	this->messageFromServer_ = message;
	int pos = message.find("LOGIN");
	if (pos != -1) {
		json m_jsonDataFromServer = stringToJson(message);
		int m_resultCode = m_jsonDataFromServer["resultCode"];
		cout << "message :" << message << endl;
		cout << "m_resultCode: " << m_resultCode << endl;
		if (m_resultCode == 1)
		{ // wss创建失败
			cout << "----------------------------------------" << endl;
			cout << "与服务器创建wss失败! 正在向服务器请求token重新连接......." << endl;

			this->connectionAndLogin();
		}
		else if (m_resultCode == 0) // wss创建成功
		{
			return;
		}
		return;
	}
	{
		// 1 http发送采集端m_jsonDataToCollection
		cout << "发送到采集端的数据为:    " << message << endl;
		if (m_httpClientToCollection.SendMessage(message) == false)
		{
			cout << "HTTP向采集端发送数据失败!正在尝试重新发送........" << endl;
			int m_index = 3;
			bool m_isSuccess = false;
			while (m_index--) {
				if (m_httpClientToCollection.SendMessage(message)) {
					m_isSuccess = true;
					break;
				}
				else
					cout << "HTTP向采集端发送数据失败!正在尝试重新发送........" << endl;
			}
			if (m_isSuccess == false) {
				cout << "HTTP向采集端发送数据失败!" << endl;
				return;
			}
		}
		else {
			// 2 http接收采集端函数调用是否成功
			string m_strDataFromCollection = m_httpClientToCollection.GetReciveMessage();
			cout << "HTTP向采集端发送数据成功! 接收到的数据为：" << m_strDataFromCollection << endl;
			// 3 wss向服务器发送从采集端接收到的数据
			this->m_websocketClientObject.Send(m_strDataFromCollection);
		}
	}
}
void WebsocketMessage::WssConnectOpenFun()
{
	this->isOnConnection_ = true;
}
void WebsocketMessage::WssConnectFailFun()
{
	this->isOnConnection_ = false;
}
int main()
{
	WebsocketMessage m_ServerMessage(false, false, false, "", "");
	string ipToserver="159.138.146.46";
	string ipToCollection="192.168.4.162";
	cout<<"请输入服务器ip地址:"; cin >> ipToserver;
	cout<<"请输入采集端ip地址:"; cin >> ipToCollection;

	m_ServerMessage.WebsocketInitialization(ipToserver,ipToCollection);
	m_ServerMessage.connectionAndLogin();
	while (true) ;

	m_ServerMessage.m_websocketClientObject.Close();

	return 0;
}
