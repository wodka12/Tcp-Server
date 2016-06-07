#include "stdafx.h"
#include "Mysql.h"
#include <mysql.h>

#include <iostream>
//#pragma comment(lib, "lib_acl_cpp_vc2015d.lib")

#import "E:\GitHub\source\test_dll\bin\Debug\test_dll.tlb" raw_interfaces_only

#import "E:\GitHub\source\testdll2\bin\Debug\testdll2.tlb" raw_interfaces_only


#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "battle_test"


CMysql::CMysql()
{
}


CMysql::~CMysql()
{
}

using namespace testdll2;
bool CMysql::initialize_mysql(void)
{

	//by lsy 2016.06.03
	/*
	포팅은 성공했으나 포팅후 포인트깨지는 문제가 발생
	포팅유지할지 RPC처리 디버깅할지 차후 고민
	굳이 메세지큐를 쓸지도 고민...
	*/
#if 0
	HRESULT hr = CoInitialize(NULL);
	Class1Ptr ptr(__uuidof(ITestdll2));

	long lResult = 0;

	int buff_test_result = ptr->Add(1, 2, &lResult);
	ptr->Release();

	CoUninitialize();
	//by lsy 2016.06.03
#endif

#if 1
	MYSQL mysql;

	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0))
	{

		printf("%s\n", mysql_error(&mysql));

		return false;

	}
	printf("[%s]_DB Connection OK\n", DB_NAME);

	mysql_close(&mysql);

	//AMQ TEST 2016.05.26
	m_channel = AmqpClient::Channel::Create("localhost");

	if (m_channel != NULL)
	{
#if 0
		m_channel->DeclareQueue("22");
		m_channel->BindQueue("alanqueue", "amq.direct", "alankey");

		AmqpClient::BasicMessage::ptr_t msg_in = AmqpClient::BasicMessage::Create();

		msg_in->Body("This is a test message.");

		m_channel->BasicPublish("amq.direct", "alankey", msg_in);

		//m_channel->BasicConsume("alanqueue", "consumertag");

		//AmqpClient::BasicMessage::ptr_t msg_out = m_channel->BasicConsumeMessage("consumertag")->Message();
#endif

#if 1 //rpc test
		//m_incoming_tag = m_channel->DeclareQueue("");
		m_outgoing_tag = "";

		//m_channel->BindQueue(m_incoming_tag, "amq.direct", m_incoming_tag);
		//m_channel->BasicConsume(m_incoming_tag, m_incoming_tag);
		//m_channel->BasicConsume(m_incoming_tag, "", true, true, false, 2);

		//call
		string message = "30";
		Call(message);

#endif //rpc test

	}
	//AMQ TEST 2016.05.26
#endif


	/*
	CoInitialize(NULL);

	ICShapSamplePtr ptr(__uuidof(CShapSample));
	ptr->ShowMessage();

	ptr->Release();
	CoUninitialize();
	*/
	return true;
	//printf(mysql_get_client_info());
}

string CMysql::Call(const string &message)
{
	AmqpClient::BasicMessage::ptr_t outgoing_msg = AmqpClient::BasicMessage::Create();
	outgoing_msg->Body(message);

	AmqpClient::BasicMessage::ptr_t reply = Call(outgoing_msg);
	
	return "";
}


AmqpClient::BasicMessage::ptr_t CMysql::Call(AmqpClient::BasicMessage::ptr_t message)
{
	//message->ReplyTo(m_incoming_tag);

	//m_channel->BasicPublish("", m_outgoing_tag, message);

	AmqpClient::BasicMessage::ptr_t incoming_msg = NULL;

	//incoming_msg = m_channel->BasicConsumeMessage(m_incoming_tag)->Message();
	
	//m_channel->BasicConsumeMessage(m_incoming_tag);

#if 1
	

	char *szBroker = getenv("AMQP_BROKER");

	if (szBroker != NULL)
		m_channel = AmqpClient::Channel::Create("localhost");
	else
		m_channel = AmqpClient::Channel::Create("localhost");

	AmqpClient::BasicMessage::ptr_t the_message = AmqpClient::BasicMessage::Create("Body Content");
	try
	{
		//m_channel->DeclareQueue("BasicReturnTestQueue");
		//m_channel->BasicConsume("BasicReturnTestQueue", "consumer_tag1");

		m_incoming_tag = m_channel->DeclareQueue(m_outgoing_tag);

		//m_channel->BindQueue("BasicReturnTestQueue", "amq.direct", "BasicReturnTestQueue");


		//m_channel->BasicPublish("", "BasicReturnTestQueue", the_message, true, false);
		//m_channel->BasicPublish("", "ThisDoesntExist", the_message, true, false);

		

		m_channel->BasicConsume(m_outgoing_tag, "consumer_tag1", false, true);

		message->CorrelationId("123");
		message->ReplyTo(m_incoming_tag);

		//m_channel->BasicPublish("", m_outgoing_tag, message, true, false);
		m_channel->BasicPublish("", "rpc_queue7", message, true, false);

#if 0
		for (int i = 0; i < 2; ++i)
		{
			AmqpClient::Envelope::ptr_t env;

			if (m_channel->BasicConsumeMessage("consumer_tag1", env, 0))
			{
				std::cout << "Received message with body: " << env->Message()->Body() << std::endl;
			}
		}
#endif
		AmqpClient::Envelope::ptr_t env;
		while (1) {
			AmqpClient::Envelope::ptr_t env;

			if (m_channel->BasicConsumeMessage("consumer_tag1", env, 0))
			{
				std::cout << "Received message with body: " << env->Message()->Body() << std::endl;
				break;
			}
		}

	}
	catch (DWORD dwError)
	{
		std::cout << "\nMessage body: ";
	}
#endif

	return incoming_msg;
}
