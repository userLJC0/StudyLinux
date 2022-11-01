#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H
 
// 不包含TLS Client
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
 
// 包含TLS Client
// #include <websocketpp/config/asio_client.hpp>
// #include <websocketpp/client.hpp>
 
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
 
 
#include <string>
#include <locale>
#include <codecvt>
#include <iconv.h>
#include <iostream>
#include <string.h>
#include <malloc.h>

#include <map>
#include <string>
// extern "C"{
// 	#include "json-c/json.h"
// }
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
using json = nlohmann::json;
using namespace std;

enum ServerCommand{
	CREATEUDP,
	STOPUDP,
	STARTPUSH,
	STOPPUSH,
	GETINFO,
	DIRECTPUSH
};
enum OperationResult{
	 SUCCESS, FAILURE
};
// typedef std::function<void(string)> OnSetRecivedMessageFunc;
//typedef OperationResult (*FunTemplate)(ServerCommand, std::string&, map<string,string>& ); 
typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef std::function<void()> OnOpenFunc;
typedef std::function<void()> OnFailFunc;
typedef std::function<void()> OnCloseFunc;
typedef std::function<void(const std::string& message)> OnMessageFunc;
  extern  int CodeConvert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen,
                 char *outbuf, size_t outlen);
extern  int u_2_g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) ;
extern  int g_2_u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
extern  std::string GBK_To_UTF8(const std::string& strGBK);
extern  std::string UTF_to_GBK(const char* utf8);	

// 保存一个连接的metadata
class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
    connection_metadata(websocketpp::connection_hdl hdl, std::string url)
        : m_Hdl(hdl)
        , m_Status("Connecting")
        , m_Url(url)
        , m_Server("N/A")
    {}
    void on_open(client * c, websocketpp::connection_hdl hdl);
    void on_fail(client * c, websocketpp::connection_hdl hdl);
    void on_close(client * c, websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    websocketpp::connection_hdl get_hdl() const;
    std::string get_status() const;
private:
    websocketpp::connection_hdl m_Hdl;  // websocketpp表示连接的编号
    std::string m_Status;               // 连接自动状态
    std::string m_Url;                  // 连接的URI
    std::string m_Server;               // 服务器信息
    std::string m_Error_reason;         // 错误原因
};



class WebsocketClient 
{
public:
	WebsocketClient();
	virtual~WebsocketClient();
 
public:
	bool Connect(std::string const & url);
	bool Close(std::string reason = "");
	bool Send(std::string message);
 
	connection_metadata::ptr GetConnectionMetadataPtr();
 
	void OnOpen(client * c, websocketpp::connection_hdl hdl);
	void OnFail(client * c, websocketpp::connection_hdl hdl);
	void OnClose(client * c, websocketpp::connection_hdl hdl);
	void OnMessage(websocketpp::connection_hdl, client::message_ptr msg);
 
 
	void SetOnOpenFunc(OnOpenFunc func);
	void SetOnFailFunc(OnFailFunc func);
	void SetOnCloseFunc(OnCloseFunc func);
	void SetMessageFunc(OnMessageFunc func);

    void ProcessData(string message);
    //void CallBackFun(FunTemplate& f);
	// void SetOnSetRecivedMessageFunc(OnSetRecivedMessageFunc func);
private:
	connection_metadata::ptr m_ConnectionMetadataPtr;
	client m_WebsocketClient;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_Thread; // 线程
 
	OnOpenFunc m_OnOpenFunc;
	OnFailFunc m_OnFailFunc;
	OnCloseFunc m_OnCloseFunc;
	OnMessageFunc m_MessageFunc;

    //FunTemplate funcFromControlServer; 
	// OnSetRecivedMessageFunc m_OnSetRecivedMessageFunc;
};
#endif // !WEBSOCKET_ENDPOINT_H
 