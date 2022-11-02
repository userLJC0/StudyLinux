#include "WebsocketClient.h"

void WebsocketClient::ProcessData(string message)
{
	//解析JSON数据
 	stringstream sstream;
    sstream << message;
    json JsonObj;
    sstream >> JsonObj;

	string command_str = JsonObj["command"] ;
	printf("command: %s\n", command_str.c_str() );

	map<string,string> data_to_collection;//发送采集端数据
	json data_to_server;//返回服务器数据
	string result_message;
	OperationResult IsSuccess;
	ServerCommand cmd;
		
	if(command_str=="LOGIN")//
	{
		data_to_collection.insert({"command",JsonObj["command"]});
		int tempValue = JsonObj["resultCode"];
		if(tempValue == 0){//登录成功
			data_to_collection.insert({"resultCode","0"});
			data_to_collection.insert({"resultMessage",JsonObj["resultMessage"]});
			cout<<"与服务器wss创建成功, "<<JsonObj["resultMessage"]<<endl;
		}else {//登录失败
			data_to_collection.insert({"resultCode","1"});
			data_to_collection.insert({"resultMessage",JsonObj["resultMessage"]});
			cout<<"与服务器wss创建失败, "<<JsonObj["resultMessage"]<<endl;
				
		}

		{
		//调用函数,最后加了一个发到采集端的map信息
		//IsSuccess = funcFromControlServer (cmd, result_message, data_to_collection);
		IsSuccess = SUCCESS;
		}
		return ;
	}

	if(command_str=="CREATEUDP")//"CREATEUDP"时创建UDP通道
	{
		cmd = ServerCommand::CREATEUDP;
		//1 解析JSON每个键值,构建发送采集端map
			
		data_to_collection.insert({"relayServerIP",JsonObj["relayServerIP"]});//中继服务器ip地址
		data_to_collection.insert({"relayServerPort",JsonObj["relayServerPort"]});//中继服务器端口地址
		data_to_collection.insert({"deviceID",JsonObj["deviceID"]});//采集端唯一标示
		data_to_collection.insert({"playerID",JsonObj["playerID"]});//播放端唯一标示
		data_to_collection.insert({"command",JsonObj["command"]});//"CREATEUDP" 创建UDP通道

	}else if(command_str=="STOPUDP")//"STOPUDP"时关闭UDP通道
	{
		cmd = ServerCommand::STOPUDP;
	}else if(command_str=="STARTPUSH")//“STARTPUSH”时为开始udp推流
	{
		cmd = ServerCommand::STARTPUSH;
	}else if(command_str=="STOPPUSH")//“STOPPUSH”关闭推流
	{
		cmd = ServerCommand::STOPPUSH;
	}else if(command_str=="GETINFO")//“GETINFO”获取采集端状态信息
	{
		cmd = ServerCommand::GETINFO;
	}else if(command_str=="DIRECTPUSH")//“DIRECTPUSH”获取采集端状态信息
	{
		cmd = ServerCommand::DIRECTPUSH;
		//1 解析JSON每个键值,构建发送采集端map
		data_to_collection.insert({"playerIP",JsonObj["playerIP"]});//推送设备IP地址
		data_to_collection.insert({"videoPort",JsonObj["videoPort"]});//推送设备视频端口
		data_to_collection.insert({"audioPort",JsonObj["audioPort"]});//推送设备音频端口
	}
	{
		//调用函数,最后加了一个发到采集端的map信息
		//IsSuccess = funcFromControlServer (cmd, result_message, data_to_collection);
		IsSuccess = SUCCESS;
	}
	//2 构建发送服务器端json数据data_to_server
	if(IsSuccess == SUCCESS)	{
		data_to_server["resultCode"] = 0;
    	data_to_server["resultMessage"] = "Success";
	}else if(IsSuccess == FAILURE)	{
		data_to_server["resultCode"] = 1;
		string mssage="Failue  somewhere " + result_message;
    	data_to_server["resultMessage"] = mssage;

	}
	data_to_server["command"] = command_str;

	//向服务器端发送JSON数据
	this->Send(data_to_server.dump());

}

// void WebsocketClient::ProcessData(string message)
// {
// 	//解析JSON数据
// 	struct json_object *obj = json_tokener_parse(message.c_str());// 将符合 json 格式的字符串 转为 json 对象 
// 	struct json_object *json;
// 	json_type type;

// 	// json_object_object_get_ex(obj, "token", &json);// 根据键名 解析出 json 对象
// 	//  type = json_object_get_type(json);// 判断解析出的 json 对象 是什么类型
// 	// if(type == json_type_string)
// 	// 	printf("token: %s\n", json_object_get_string(json));
	
// 	json_object_object_get_ex(obj, "command", &json);
// 	 type = json_object_get_type(json);
// 	if(type == json_type_string)
// 	{
// 		string command_str=json_object_get_string(json);
// 		printf("command: %s\n", command_str.c_str() );

// 		map<string,string> data_to_collection;//发送采集端数据
// 		struct json_object *data_to_server;//返回服务器数据
// 		string result_message;
// 		OperationResult IsSuccess;
// 		ServerCommand cmd;
		
// 		if(command_str=="LOGIN")//"CREATEUDP"时创建UDP通道
// 		{
// 			data_to_collection.insert({"command",json_object_get_string(json)});
// 			json_object_object_get_ex(obj, "resultCode", &json);
// 			if(json_object_get_int(json)==0){//登录成功
// 				data_to_collection.insert({"resultCode","0"});
// 				json_object_object_get_ex(obj, "resultMessage", &json);
// 				data_to_collection.insert({"resultMessage",json_object_get_string(json)});
// 				cout<<"与服务器wss创建成功, "<<json_object_get_string(json)<<endl;
// 			}else {//登录失败
// 				data_to_collection.insert({"resultCode","1"});
// 				json_object_object_get_ex(obj, "resultMessage", &json);
// 				data_to_collection.insert({"resultMessage",json_object_get_string(json)});
// 				cout<<"与服务器wss创建失败, "<<json_object_get_string(json)<<endl;
				
// 			}

// 			{
// 			//调用函数,最后加了一个发到采集端的map信息
// 			//IsSuccess = funcFromControlServer (cmd, result_message, data_to_collection);
// 			IsSuccess = SUCCESS;
// 			}
// 			return ;
// 		}

// 		if(command_str=="CREATEUDP")//"CREATEUDP"时创建UDP通道
// 		{
// 			cmd = CREATEUDP;
// 			//1 解析JSON每个键值,构建发送采集端map
			
// 			json_object_object_get_ex(obj, "relayServerIP", &json);//中继服务器ip地址
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"relayServerIP",json_object_get_string(json)});
	
// 			json_object_object_get_ex(obj, "relayServerPort", &json);//中继服务器端口地址
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"relayServerPort",json_object_get_string(json)});

// 			json_object_object_get_ex(obj, "deviceID", &json);//采集端唯一标示
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"deviceID",json_object_get_string(json)});

// 			json_object_object_get_ex(obj, "playerID", &json);//播放端唯一标示
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"playerID",json_object_get_string(json)});

// 			json_object_object_get_ex(obj, "command", &json);//"CREATEUDP"时创建UDP通道
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"command",json_object_get_string(json)});

			
			
// 		}else if(command_str=="STOPUDP")//"STOPUDP"时关闭UDP通道
// 		{
// 			cmd = STOPUDP;

// 		}else if(command_str=="STARTPUSH")//“STARTPUSH”时为开始udp推流
// 		{
// 			cmd=STARTPUSH;
// 		}else if(command_str=="STOPPUSH")//“STARTPUSH”时为开始udp推流
// 		{
// 			cmd = STOPPUSH;
// 		}else if(command_str=="STARTPUSH")//“STOPPUSH”关闭推流
// 		{
// 			cmd = STARTPUSH;

// 		}else if(command_str=="GETINFO")//“GETINFO”获取采集端状态信息
// 		{
// 			cmd = GETINFO;

// 		}else if(command_str=="DIRECTPUSH")//“DIRECTPUSH”获取采集端状态信息
// 		{
// 			cmd = DIRECTPUSH;
// 			//1 解析JSON每个键值,构建发送采集端map
// 			json_object_object_get_ex(obj, "playerIP", &json);//播放端唯一标示
// 			type = json_object_get_type(json);
// 			if(type == json_type_string)
// 				data_to_collection.insert({"playerIP",json_object_get_string(json)});

// 		}

// 		{
// 			//调用函数,最后加了一个发到采集端的map信息
// 			//IsSuccess = funcFromControlServer (cmd, result_message, data_to_collection);
// 			IsSuccess = SUCCESS;
// 		}
// 		//2 构建发送服务器端json数据data_to_server
// 		if(IsSuccess == SUCCESS)	{
// 			json_object_object_add(data_to_server, "resultCode", json_object_new_int(1));
// 			json_object_object_add(data_to_server, "resultMessage", json_object_new_string("Success"));
// 		}else if(IsSuccess == FAILURE)	{
// 			json_object_object_add(data_to_server, "resultCode", json_object_new_int(0));
// 			string mssage="Failue  somewhere" + result_message;
// 			json_object_object_add(data_to_server, "resultMessage", json_object_new_string(mssage.c_str()));
// 		}
		
// 		json_object_object_add(data_to_server, "command", json_object_new_string(command_str.c_str()));

// 		//向服务器端发送JSON数据
		
//    		// printf("%s\n", buf);
// 		//char str1[4096]; char content_len[100];

//         //strcat(str1, "POST http://192.168.4.190:8089/ HTTP/1.1\r\n");
//         //strcat(str1,"Host: 192.168.4.190:8089\r\n");
	
// 		const char* buf = json_object_to_json_string(data_to_server);
//    		//sprintf(content_len,"Content-Length: %ld\r\n\r\n",strlen(buf));//header与body分界：一个空行
//    		//strcat(str1,content_len);
//    		//strcat(str1,buf);


// 		string sendmessage(buf);
// 		this->Send(sendmessage);
			
// 	}

// }

extern  int CodeConvert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen,
                 char *outbuf, size_t outlen) {
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;

    memset(outbuf, 0, outlen);

    if ((int)iconv(cd, pin, &inlen, pout, &outlen) == -1)
    {
        iconv_close(cd);
        return -1;
    }
    iconv_close(cd);
    *(*pout) = '\0';

    return 0;
}

extern  int u_2_g(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    return CodeConvert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

extern  int g_2_u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    return CodeConvert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

extern  std::string GBK_To_UTF8(const std::string& strGBK)
{
    int length = strGBK.size()*2+1;

    char *temp = (char*)malloc(sizeof(char)*length);

    if(g_2_u((char*)strGBK.c_str(),strGBK.size(),temp,length) >= 0)
    {
        std::string str_result;
        str_result.append(temp);
        free(temp);
        return str_result;
    }else
    {
        free(temp);
        return "";
    }
}

extern  std::string UTF_to_GBK(const char* utf8)
{
    int length = strlen(utf8);

    char *temp = (char*)malloc(sizeof(char)*length);

    if(u_2_g((char*)utf8,length,temp,length) >= 0)
    {
        std::string str_result;
        str_result.append(temp);
        free(temp);

        return str_result;
    }else
    {
        free(temp);
        return "";
    }
}
			 
WebsocketClient::WebsocketClient()
{
	m_WebsocketClient.clear_access_channels(websocketpp::log::alevel::all);  // 开启全部接入日志级别
	m_WebsocketClient.clear_error_channels(websocketpp::log::elevel::all);   // 开启全部错误日志级别
 
	m_WebsocketClient.init_asio();       // 初始化asio
	m_WebsocketClient.start_perpetual(); // 避免请求为空时退出，实际上，也是避免asio退出
 
	// 独立运行client::run()的线程，主要是避免阻塞
	m_Thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_WebsocketClient);
 
	m_OnOpenFunc = nullptr;
	m_OnFailFunc = nullptr;
	m_OnCloseFunc = nullptr;
	m_MessageFunc = nullptr;
}
 
WebsocketClient::~WebsocketClient()
{
	m_WebsocketClient.stop_perpetual();
 
	if (m_ConnectionMetadataPtr != nullptr && m_ConnectionMetadataPtr->get_status() == "Open")
	{
		websocketpp::lib::error_code ec;
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), websocketpp::close::status::going_away, "", ec); // 关闭连接
 
		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}
 
	m_Thread->join();
}
 
bool WebsocketClient::Connect(std::string const & url)
{
	websocketpp::lib::error_code ec;
 
	// 创建connect的共享指针，注意，此时创建并没有实际建立
	client::connection_ptr con = m_WebsocketClient.get_connection(url, ec);  
 
	if (ec) {
		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
		return false;
	}
 
	// 创建连接的metadata信息，并保存
	connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), url);
	m_ConnectionMetadataPtr = metadata_ptr;
 
	// 注册连接打开的Handler
	//con->set_open_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_open,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_open_handler(websocketpp::lib::bind(
		&WebsocketClient::OnOpen,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));
 
	// 注册连接失败的Handler
	//con->set_fail_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_fail,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_fail_handler(websocketpp::lib::bind(
		&WebsocketClient::OnFail,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));
 
	// 注册连接关闭的Handler
	//con->set_close_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_close,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_close_handler(websocketpp::lib::bind(
		&WebsocketClient::OnClose,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));
 
	// 注册连接接收消息的Handler
	//con->set_message_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_message,
	//	metadata_ptr,
	//	websocketpp::lib::placeholders::_1,
	//	websocketpp::lib::placeholders::_2
	//));
	con->set_message_handler(websocketpp::lib::bind(
		&WebsocketClient::OnMessage,
		this,
		websocketpp::lib::placeholders::_1,
		websocketpp::lib::placeholders::_2
	));
 
	// 进行连接
	m_WebsocketClient.connect(con);
 
	//std::cout << "Websocket连接成功" << std::endl;
 
	// 注意，不能在Websocket连接完成之后马上就发送消息，不然会出现Invalid State的错误，
	// 导致消息发送不成功，所以在连接成功之后，主线程休眠1秒
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 
	return true;
}
 
bool WebsocketClient::Close(std::string reason)
{
	websocketpp::lib::error_code ec;
 
	if (m_ConnectionMetadataPtr != nullptr)
	{
		int close_code = websocketpp::close::status::normal;
		// 关闭连接
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), close_code, reason, ec);
		if (ec) {
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
			return false;
		}
		std::cout << "关闭Websocket连接成功" << std::endl;
	}
 
	return true;
}
 
bool WebsocketClient::Send(std::string message)
{
	websocketpp::lib::error_code ec;
 
	if (m_ConnectionMetadataPtr != nullptr)
	{
		// 连接发送数据
		m_WebsocketClient.send(m_ConnectionMetadataPtr->get_hdl(),GBK_To_UTF8(message), websocketpp::frame::opcode::text, ec);
		if (ec) 
		{
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			std::string errorMessage = ec.message();
			return false;
		}
		std::cout << "WSS发送数据成功!发送数据为:"<< message << std::endl;
	}
 
	return true;
}
 
connection_metadata::ptr WebsocketClient::GetConnectionMetadataPtr()
{
	return m_ConnectionMetadataPtr;
}
 
void WebsocketClient::OnOpen(client * c, websocketpp::connection_hdl hdl)
{
	cout<<"WSS连接连接成功!"<<endl;
	if (m_OnOpenFunc != nullptr)
	{
		cout<<"调用m_OnOpenFunc"<<endl;
		m_OnOpenFunc();
	}
	
}
 
void WebsocketClient::OnFail(client * c, websocketpp::connection_hdl hdl)
{
	cout<<"WSS连接连接失败!"<<endl;
	if (m_OnFailFunc != nullptr)
	{
		cout<<"调用m_OnFailFunc"<<endl;
		m_OnFailFunc();
	}	
}
 
void WebsocketClient::OnClose(client * c, websocketpp::connection_hdl hdl)
{
	cout<<"WSS连接断开!"<<endl;
	if (m_OnCloseFunc != nullptr)
	{
		cout<<"调用m_OnCloseFunc"<<endl;
		m_OnCloseFunc();
	}
}
 
void WebsocketClient::OnMessage(websocketpp::connection_hdl, client::message_ptr msg)
{
	if (msg->get_opcode() == websocketpp::frame::opcode::text)
	{
		std::string message = UTF_to_GBK(msg->get_payload().c_str());
		std::cout << "收到来自服务器的消息：" << message << std::endl;

		if (m_MessageFunc != nullptr)
		{
			std::cout << "调用 m_MessageFunc " << std::endl;
			m_MessageFunc(message);
			//cout<<"+++++++++++++++++++++++1111111"<<endl;
		}
		
		//	ProcessData( message );
		
	}
	else
	{
		std::string message = websocketpp::utility::to_hex(msg->get_payload());
		std::cout << "message: " << message << std::endl;
	}
}
 
void WebsocketClient::SetOnOpenFunc(OnOpenFunc func)
{
	m_OnOpenFunc = func;
}
 
void WebsocketClient::SetOnFailFunc(OnFailFunc func)
{
	m_OnFailFunc = func;
}
 
void WebsocketClient::SetOnCloseFunc(OnCloseFunc func)
{
	m_OnCloseFunc = func;
}
 
void WebsocketClient::SetMessageFunc(OnMessageFunc func)
{
	m_MessageFunc = func;
}
 
//   void WebsocketClient::CallBackFun(FunTemplate& f)
//   {
// 		funcFromControlServer=f;
//   }

// void WebsocketClient::SetOnSetRecivedMessageFunc(OnSetRecivedMessageFunc func)
// {
// 	m_OnSetRecivedMessageFunc=func;
// }

  void connection_metadata::on_open(client * c, websocketpp::connection_hdl hdl)
    {
        m_Status = "Open";

        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_Server = con->get_response_header("Server");
    }

void connection_metadata:: on_fail(client * c, websocketpp::connection_hdl hdl)
    {
        m_Status = "Failed";

        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_Server = con->get_response_header("Server");
        m_Error_reason = con->get_ec().message();
    }

	void  connection_metadata::on_close(client * c, websocketpp::connection_hdl hdl)
    {
        m_Status = "Closed";
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        std::stringstream s;
        s << "close code: " << con->get_remote_close_code() << " ("
            << websocketpp::close::status::get_string(con->get_remote_close_code())


            << "), close reason: " << con->get_remote_close_reason();
        m_Error_reason = s.str();
    }

	 void  connection_metadata:: on_message(websocketpp::connection_hdl, client::message_ptr msg)
    {
        if (msg->get_opcode() == websocketpp::frame::opcode::text)
        {
            std::string message = UTF_to_GBK(msg->get_payload().c_str());
            std::cout << "收到来自服务器的消息：" << message << std::endl;
        }
        else
        {
            std::string message = websocketpp::utility::to_hex(msg->get_payload());
        }
    }

	 websocketpp::connection_hdl  connection_metadata:: get_hdl() const
    {
        return m_Hdl;
    }

	 std::string  connection_metadata::get_status() const
    {
        return m_Status;
    }