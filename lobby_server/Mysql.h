#pragma once

#include <BasicMessage.h>
#include <Channel.h>
#include <Util.h>

#include <boost\utility.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\make_shared.hpp>

#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

class SIMPLEAMQPCLIENT_EXPORT CMysql : boost::noncopyable
{
public:
	CMysql();
	~CMysql();

public:
	bool initialize_mysql(void);
	bool initialize_AMQ(void);
	bool rpc_send(string arg);

	//by lsy 16.06.01
public:
	typedef boost::shared_ptr<CMysql> ptr_t;

	//friend ptr_t boost::make_shared<CMysql>(AmqpClient::Channel::ptr_t const &a1, std::string const &a2);

	//static ptr_t Create(AmqpClient::Channel::ptr_t channel, const std::string &rpc_name)
	//{
	//	return boost::make_shared<CMysql>(channel, rpc_name);
	//}

	string m_incoming_tag;
	string m_outgoing_tag;
	AmqpClient::Channel::ptr_t m_channel;


	string Call(const std::string &message);
	AmqpClient::BasicMessage::ptr_t Call(AmqpClient::BasicMessage::ptr_t message);
};



#ifdef _MSC_VER
#pragma warning(pop)
#endif
