#include "stdafx.h"
#include "SimpleRpcClient.h"

#include <BasicMessage.h>


/*
namespace AmqpClient
{
	SimpleRpcClient::SimpleRpcClient(Channel::ptr_t channel, const std::string &rpc_name) :
		m_channel(channel), m_outgoing_tag(rpc_name),
		// Declare the reply queue, by passing an empty string, the broker will
		// give us a name
		m_incoming_tag(m_channel->DeclareQueue(""))
	{
		m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
		m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
	}

	SimpleRpcClient::~SimpleRpcClient()
	{
	}

	std::string SimpleRpcClient::Call(const std::string &message)
	{
		BasicMessage::ptr_t outgoing_msg = BasicMessage::Create();
		outgoing_msg->Body(message);

		BasicMessage::ptr_t reply = Call(outgoing_msg);
		return reply->Body();
	}

	BasicMessage::ptr_t SimpleRpcClient::Call(BasicMessage::ptr_t message)
	{
		message->ReplyTo(m_incoming_tag);
		m_channel->BasicPublish("amq.direct", m_outgoing_tag, message);

		BasicMessage::ptr_t incoming_msg = m_channel->BasicConsumeMessage(m_incoming_tag)->Message();

		return incoming_msg;
	}

} // namespace AmqpClient

*/