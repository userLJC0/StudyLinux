#include "WebsocketClient.h"
// extern "C"{
// #include "json-c/json.h"
// }
#include "HttpClient.h"
#include <functional>
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
	WebsocketMessage(bool m_isConnection,bool m_isSendMes,bool m_isGettoken,string m_message,string m_message2):
			isOnConnection_(m_isConnection),isReciveMessage_(m_isSendMes),isGettoken_(m_isGettoken),messageFromServer_(m_message),token_(m_message2){}
	bool isOnConnection_;				//websocket连接是否存在
	bool isReciveMessage_;				//websocket服务器是否发送信息
	string messageFromServer_;			//websocket服务器发送的信息
	void websocketCloseFunc();			//websocket连接关闭时调用
	void setMessage(const string& message );	//websocket服务器设置收到的数据
	void WssConnectOpenFun();			//Wss连接成功
	void WssConnectFailFun();			//Wss连接失败
	void WebsocketInitialization();
};
void  WebsocketMessage::WebsocketInitialization()
{

}
void WebsocketMessage::websocketCloseFunc()//websocket连接关闭时调用
{
	this->isOnConnection_=false;
}
void WebsocketMessage::setMessage(const string& message )
{
	this->isReciveMessage_ = true ;
	this->messageFromServer_ = message;
	int pos =message.find("LOGIN");
	if(pos!= -1 )  
	{
		cout<<"接收到的WSS数据stringToJson----------"<< message <<endl;
		json m_jsonDataFromServer = stringToJson(message);
		int m_resultCode = m_jsonDataFromServer["resultCode"];
		cout<<"message :"<<message<<endl;
		cout<<"m_resultCode: "<<m_resultCode<<endl;
		if( m_resultCode == 1 ){//wss创建失败
			cout<<"----------------------------------------"<<endl;
			cout<<"与服务器创建wss失败! 正在向服务器请求token重新连接......."<<endl;
			
			json m_jsonDataToServer;
			m_jsonDataToServer["deviceUuid"] = "device_uid";
    		m_jsonDataToServer["deviceSecret"] = "device_secret";
   			string	m_strnDataToServer = m_jsonDataToServer.dump();   
   	 		cout<<"HTTP向客户端发送数据 : "<<m_strnDataToServer<<endl;
			if(m_httpClientToServer.SendMessage(m_strnDataToServer) == false) {
				cout<<"HTTP数据发送失败!正在尝试重新发送........"<<endl;
				int m_index = 3 ; bool m_isSuccess=false;
				while(m_index--) {
				if(m_httpClientToServer.SendMessage(m_strnDataToServer)) {
					//cout<<"HTTP数据发送成功!"<<endl;
					m_isSuccess=true;
					break;
				} else cout<<"HTTP数据发送失败!正在尝试重新发送........"<<endl;
				}
			if(m_isSuccess == false) {
				cout<<"HTTP数据发送失败!退出程序"<<endl;
				return ;
			}
			}
   			{
				string 	m_strDataFromServer = m_httpClientToServer.GetReciveMessage();
    			cout<<"HTTP数据发送成功! 接收到的数据为："<<m_strDataFromServer<<endl;
				cout<<"接收到的HTTP数据stringToJson----------"<<m_strDataFromServer<<endl;
   				m_jsonDataFromServer = stringToJson(m_strDataFromServer);

				m_jsonDataToServer.clear();
				m_jsonDataToServer["token"] = m_jsonDataFromServer["token"];
				
				this->m_websocketClientObject.Connect(m_websocketClientUrl);
				if(this->isOnConnection_ == false ) {
					cout<<"Wss连接失败!正在尝试重新连接........"<<endl;
					int m_index = 3 ;
					bool m_isSuccess=false;
					while(m_index--) {
						this->m_websocketClientObject.Connect(m_websocketClientUrl);
						if(this->isOnConnection_) {
							cout<<"Wss重新连接成功!"<<endl;
							 m_isSuccess = true;
							break;
						}
					}
					if(m_isSuccess == false) {
						cout<<"----------------------------------------"<<endl;
						cout<<"Wss连接失败!退出程序"<<endl;
						return ;
					}
				}
				this->m_websocketClientObject.Send(m_jsonDataToServer.dump());//WSS 发送token
				cout<<"------------------"<<endl;
   			}
		}else if( m_resultCode == 0 )//wss创建成功
		{
			return ;
		}
		return ;
	}
	// this->isReciveMessage_ = true ;
	// this->messageFromServer_ = message;
	// cout<<"isReciveMessage_ :"<<isReciveMessage_<<endl;
	// cout<<"messageFromServer_ :"<<messageFromServer_<<endl;
	// HttpClient m_httpClientToCollection;
	//m_httpClientToCollection.PutUrl("http://192.168.4.244:8080/sendCommand");
	{
		//1 http发送采集端m_jsonDataToCollection
		cout<<"发送到采集端的数据为:    "<<message<<endl;
		if(m_httpClientToCollection.SendMessage(message) == false) {
			cout<<"HTTP向采集端发送数据失败!正在尝试重新发送........"<<endl;
			int m_index = 3 ;
			bool m_isSuccess=false;
			while(m_index--) {
			if(m_httpClientToCollection.SendMessage(message)) {
				m_isSuccess=true;
				break;
			} else cout<<"HTTP向采集端发送数据失败!正在尝试重新发送........"<<endl;
			}
				if(m_isSuccess == false) {
				cout<<"HTTP向采集端发送数据失败!"<<endl;
				return ;
			}
		}
  		else{	
			//2 http接收采集端函数调用是否成功
			string m_strDataFromCollection = m_httpClientToCollection.GetReciveMessage();
    		cout<<"HTTP向采集端发送数据成功! 接收到的数据为："<<m_strDataFromCollection<<endl;
			//3 wss向服务器发送从采集端接收到的数据
			m_websocketClientObject.Send(m_strDataFromCollection);
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
	WebsocketMessage m_ServerMessage(false,false,false,"","");
	// HttpClient m_httpClientToServer;
	// HttpClient m_httpClientToCollection;
	//WebsocketClient m_websocketClientObject;

	string m_strDataFromServer;
	json m_jsonDataFromServer;
	string m_strnDataToServer;
	json m_jsonDataToServer;
	string m_strDataFromCollection;
	json m_jsonDataFromCollection;
	string m_strnDataToCollection;
	json m_jsonDataToCollection;

	m_ServerMessage.m_httpClientToServer.PutUrl("http://192.168.4.101:8088/cameraLogin");
	m_ServerMessage.m_httpClientToCollection.PutUrl("http://192.168.4.162:8080/sendCommand");
	 m_ServerMessage.m_websocketClientUrl = "ws://192.168.4.101:8089/ws";

//注册回调函数
	//注册wss连接成功回调函数
	std::function<void()> m_OnConnectionOpenFunc = bind(&WebsocketMessage::WssConnectOpenFun,&m_ServerMessage);
	m_ServerMessage.m_websocketClientObject.SetOnOpenFunc(m_OnConnectionOpenFunc);
	//注册wss连接失败回调函数
	std::function<void()> m_OnConnectionFailFunc = bind(&WebsocketMessage::WssConnectFailFun,&m_ServerMessage);
	m_ServerMessage.m_websocketClientObject.SetOnFailFunc(m_OnConnectionFailFunc);
	//注册wss关闭回调函数
	std::function<void()> m_OnCloseFunc = bind(&WebsocketMessage::websocketCloseFunc,&m_ServerMessage);
	m_ServerMessage.m_websocketClientObject.SetOnCloseFunc(m_OnCloseFunc);
	//注册wss接收数据回调函数
	std::function<void(const std::string& message)>  m_onSetMessage = bind(&WebsocketMessage::setMessage, &m_ServerMessage,placeholders::_1);
	m_ServerMessage.m_websocketClientObject.SetMessageFunc(m_onSetMessage);


	
    m_jsonDataToServer["deviceUuid"] = "device_uid";
    m_jsonDataToServer["deviceSecret"] = "device_secret";
   	m_strnDataToServer = m_jsonDataToServer.dump();   
    cout<<"HTTP向客户端发送数据 : "<<m_strnDataToServer<<endl;
	if(m_ServerMessage.m_httpClientToServer.SendMessage(m_strnDataToServer) == false) {
		cout<<"HTTP数据发送失败!正在尝试重新发送........"<<endl;
		int m_index = 3 ;
		bool m_isSuccess=false;
		while(m_index--) {
			if(m_ServerMessage.m_httpClientToServer.SendMessage(m_strnDataToServer)) {
				//cout<<"HTTP数据发送成功!"<<endl;
				m_isSuccess=true;
				break;
			} else cout<<"HTTP数据发送失败!正在尝试重新发送........"<<endl;
		}
		if(m_isSuccess == false) {
			cout<<"HTTP数据发送失败!退出程序"<<endl;
			return 0;
		}
	}
   {
	m_strDataFromServer = m_ServerMessage.m_httpClientToServer.GetReciveMessage();
    cout<<"HTTP数据发送成功! 接收到的数据为："<<m_strDataFromServer<<endl;

	//解析JSON数据
	cout<<"接收到的HTTP数据stringToJson----------"<<m_strDataFromServer<<endl;
   m_jsonDataFromServer = stringToJson(m_strDataFromServer);

	m_jsonDataToServer.clear();
	m_jsonDataToServer["token"] = m_jsonDataFromServer["token"];
	cout<<"------------------"<<endl;

//websocket通信
// retryWss:
	m_ServerMessage.m_websocketClientObject.Connect(m_ServerMessage.m_websocketClientUrl);
	if(m_ServerMessage.isOnConnection_ == false ) {
		cout<<"Wss连接失败!正在尝试重新连接........"<<endl;
		int m_index = 3 ;
		bool m_isSuccess=false;
		while(m_index--) {
			m_ServerMessage.m_websocketClientObject.Connect(m_ServerMessage.m_websocketClientUrl);
			if(m_ServerMessage.isOnConnection_) {
				cout<<"Wss重新连接成功!"<<endl;
				 m_isSuccess = true;
				break;
			}
		}
		if(m_isSuccess == false) {
			cout<<"----------------------------------------"<<endl;
			cout<<"Wss连接失败!退出程序"<<endl;
			return 0;
		}
	}
	m_ServerMessage.m_websocketClientObject.Send(m_jsonDataToServer.dump());//WSS 发送token

	while(true) ;
	// if (m_ServerMessage.isReciveMessage_)
	// {
	// 	m_ServerMessage.isReciveMessage_=false;
	// 	//解析JSON数据
	// 	string str=m_ServerMessage.messageFromServer_;
	// cout<<"接收到的WSS数据stringToJson----------"<<str<<endl;
	// 	m_jsonDataFromServer = stringToJson(str);
	// 	int m_resultCode = m_jsonDataFromServer["resultCode"];
	// 	cout<<"m_ServerMessage.messageFromServer_ :"<<m_ServerMessage.messageFromServer_<<endl;
	// 	cout<<"m_resultCode: "<<m_resultCode<<endl;
	// 	if( m_resultCode == 1 )//wss创建失败
	// 	{
	// 		cout<<"----------------------------------------"<<endl;
	// 		cout<<"与服务器创建wss失败! 正在向服务器发送token重新连接......."<<endl;
	// 		// goto retryWss;
	// 	}else  if( m_resultCode == 0 )//wss创建成功
	// 	{
	// 		while(true){
	// 			if(m_ServerMessage.isOnConnection_ == false ) {
	// 				cout<<"----------------------------------------"<<endl;
	// 				cout<<"与服务器wss连接断开! 正在向服务器发送token重新连接......."<<endl;
	// 				// goto retryWss;
	// 			}
	// 			if(m_ServerMessage.isReciveMessage_){
	// 				m_ServerMessage.isReciveMessage_ = false;
	// 				m_strnDataToCollection = m_ServerMessage.messageFromServer_ ;

	// 					// {
	// 					// 	//1 http发送采集端m_jsonDataToCollection
	// 					// 	cout<<"发送到采集端的数据为:    "<<m_strnDataToCollection<<endl;
	// 					// 	if(m_httpClientToCollection.SendMessage(m_strnDataToCollection) == false) {
	// 					// 		cout<<"HTTP向采集端发送数据失败!正在尝试重新发送........"<<endl;
	// 					// 		int m_index = 3 ;
	// 					// 		bool m_isSuccess=false;
	// 					// 		while(m_index--) {
	// 					// 		if(m_httpClientToCollection.SendMessage(m_strnDataToServer)) {
	// 					// 			m_isSuccess=true;
	// 					// 			break;
	// 					// 		} else cout<<"HTTP向采集端发送数据失败!正在尝试重新发送........"<<endl;
	// 					// 		}
	// 					// 		if(m_isSuccess == false) {
	// 					// 			cout<<"HTTP向采集端发送数据失败!退出程序"<<endl;
	// 					// 			return 0;
	// 					// 		}
	// 					// 	}
  	// 					// 	else{	
	// 					// 		//2 http接收采集端函数调用是否成功
	// 					// 		m_strDataFromCollection = m_httpClientToCollection.GetReciveMessage();
    // 					// 		cout<<"HTTP向采集端发送数据成功! 接收到的数据为："<<m_strDataFromCollection<<endl;
	// 					// 		//3 wss向服务器发送从采集端接收到的数据
	// 					// 		m_ServerMessage.m_websocketClientObject.Send(m_strDataFromCollection);
	// 					// 	}
							
	// 					// }
	// 			}
	// 		}
	// 	}

	// }
	m_ServerMessage.m_websocketClientObject.Close();
	
  }


	return 0;
}
