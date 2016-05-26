#include "stdafx.h"
#include "Mysql.h"
#include <mysql.h>

//#pragma comment(lib, "lib_acl_cpp_vc2015d.lib")


#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "battle_test"


#include <BasicMessage.h>
#include <Channel.h>

#ifdef _MSC_VER
# pragma warning ( push )
# pragma warning ( disable: 4275 4251 )
#endif

//typedef boost::shared_ptr<CMysql> ptr_t;

CMysql::CMysql()
{
}


CMysql::~CMysql()
{
}


bool CMysql::initialize_mysql(void)
{
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
	AmqpClient::Channel::ptr_t m_channel = AmqpClient::Channel::Create("localhost");
	if (m_channel != NULL)
	{
		m_channel->DeclareQueue("1");
		m_channel->BindQueue("alanqueue", "amq.direct", "alankey");

		AmqpClient::BasicMessage::ptr_t msg_in = AmqpClient::BasicMessage::Create();

		msg_in->Body("This is a test message.");

		m_channel->BasicPublish("amq.direct", "alankey", msg_in);

		//m_channel->BasicConsume("alanqueue", "consumertag");

		//AmqpClient::BasicMessage::ptr_t msg_out = m_channel->BasicConsumeMessage("consumertag")->Message();

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
