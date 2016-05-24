#include "stdafx.h"
#include "Mysql.h"
#include <mysql.h>

//#pragma comment(lib, "libmysql.lib")

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
#endif

	return true;
	//printf(mysql_get_client_info());


}
