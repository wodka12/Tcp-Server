#pragma once

#include "SimpleAmqpClient/BasicMessage.h"
#include "SimpleAmqpClient/Channel.h"
#include "SimpleAmqpClient/Util.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif


/*
namespace AmqpClient
{
	class SIMPLEAMQPCLIENT_EXPORT SimpleRpcClient : boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<SimpleRpcClient> ptr_t;

	    friend ptr_t boost::make_shared<SimpleRpcClient> (AmqpClient::Channel::ptr_t const &a1, std::string const &a2);

		static ptr_t Create(AmqpClient::Channel::ptr_t channel, const std::string &rpc_name)
		{
			return boost::make_shared<SimpleRpcClient>(channel, rpc_name);
		}

	private:
		explicit SimpleRpcClient(Channel::ptr_t channel, const std::string &rpc_name);

	public:
		virtual ~SimpleRpcClient();

		std::string Call(const std::string &message);
		BasicMessage::ptr_t Call(BasicMessage::ptr_t message);
	private:
		Channel::ptr_t m_channel;
		const std::string m_outgoing_tag;
		const std::string m_incoming_tag;

	};


} // namespace AmqpClient

#ifdef _MSC_VER
# pragma warning ( pop )
#endif

*/