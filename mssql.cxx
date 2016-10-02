#define UNICODE
#include <sql.h>
#include <sqlext.h>
#include "mssql.hxx"
#include "string.hxx"

namespace tmplORM
{
	namespace mssql
	{
		namespace driver
		{
template<typename T> void swap(const T &a, const T &b) noexcept//(std::swap(const_cast<T &>(a), const_cast<T &>(b)))
	{ std::swap(const_cast<T &>(a), const_cast<T &>(b)); }

tSQLExecErrorType_t translateError(const int16_t result)
{
	if (result == SQL_NEED_DATA)
		return tSQLExecErrorType_t::needData;
	else if (result == SQL_NO_DATA)
		return tSQLExecErrorType_t::noData;
#ifdef SQL_PARAM_DATA_AVAILABLE
	else if (result == SQL_PARAM_DATA_AVAILABLE)
		return tSQLExecErrorType_t::dataAvail;
#endif
	else if (result == SQL_INVALID_HANDLE)
		return tSQLExecErrorType_t::handleInv;
	else if (result == SQL_ERROR)
		return tSQLExecErrorType_t::generalError;
	else if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
		return tSQLExecErrorType_t::unknown;
	return tSQLExecErrorType_t::ok;
}

tSQLClient_t::tSQLClient_t() noexcept : dbHandle(nullptr), connection(nullptr), haveConnection(false), needsCommit(false), _error()
{
	if (error(SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &dbHandle), SQL_HANDLE_ENV, dbHandle) || !dbHandle)
		return;

	else if (error(SQLSetEnvAttr(dbHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void *>(long(SQL_OV_ODBC3)), 0), SQL_HANDLE_ENV, dbHandle))
		return;
	else if (error(SQLAllocHandle(SQL_HANDLE_DBC, dbHandle, &connection), SQL_HANDLE_DBC, connection) || !connection)
		return;
}

tSQLClient_t::~tSQLClient_t() noexcept
{
	if (haveConnection)
	{
		if (needsCommit)
			rollback();
		disconnect();
	}
	SQLFreeHandle(SQL_HANDLE_DBC, connection);
	SQLFreeHandle(SQL_HANDLE_ENV, dbHandle);
}

tSQLClient_t &tSQLClient_t::operator =(tSQLClient_t &&con) noexcept
{
	std::swap(dbHandle, con.dbHandle);
	std::swap(connection, con.connection);
	std::swap(haveConnection, con.haveConnection);
	std::swap(needsCommit, con.needsCommit);
	std::swap(_error, con._error);
	return *this;
}

bool tSQLClient_t::connect(const stringPtr_t &connString) const noexcept
{
	if (!dbHandle || !connection || haveConnection)
		return false;
	auto odbcString = utf16::convert(connString.get());
	int16_t temp;

	return !error(SQLBrowseConnect(connection, odbcString, utf16::length(odbcString), nullptr, 0, &temp), SQL_HANDLE_DBC, connection);
}

bool tSQLClient_t::connect(const char *const driver, const char *const host, const uint32_t port, const char *const user, const char *const passwd) const noexcept
	{ return connect(formatString("Driver=%s;Server=tcp:%s,%u;Trusted_Connection=no;UID=%s;PID=%s", driver, host, port, user, passwd)); }

bool tSQLClient_t::selectDB(const char *const db) const noexcept
	{ return connect(formatString("Database=%s", db)); }

void tSQLClient_t::disconnect() const noexcept
{
	if (haveConnection)
		haveConnection = error(SQLDisconnect(connection), SQL_HANDLE_DBC, connection);
}

bool tSQLClient_t::beginTransact() const noexcept
{
	if (valid() && !needsCommit)
	{
		needsCommit = !error(SQLSetConnectAttr(connection, SQL_ATTR_AUTOCOMMIT, reinterpret_cast<void *>(long(SQL_AUTOCOMMIT_ON)), 0),
			SQL_HANDLE_DBC, connection);
	}
	return needsCommit;
}

bool tSQLClient_t::endTransact(const bool commitSuccess) const noexcept
{
	if (valid() && needsCommit)
		needsCommit = error(SQLEndTran(SQL_HANDLE_DBC, connection, commitSuccess ? SQL_COMMIT : SQL_ROLLBACK), SQL_HANDLE_DBC, connection);
	return !needsCommit;
}

tSQLResult_t tSQLClient_t::query(const char *const queryStmt) const noexcept
{
	auto query = prepare(queryStmt, 0);
	if (!query.valid())
		return tSQLResult_t();
	return query.execute();
}

bool tSQLClient_t::error(const tSQLExecErrorType_t err, const int16_t handleType, void *const handle) const noexcept
{
	_error = std::move(tSQLExecError_t(err, handleType, handle));
	return err != tSQLExecErrorType_t::ok;
}

bool tSQLClient_t::error(const int16_t err, const int16_t handleType, void *const handle) const noexcept
	{ return error(translateError(err), handleType, handle); }
bool tSQLClient_t::error(const tSQLExecErrorType_t err) const noexcept
	{ return error(err, SQL_HANDLE_ENV, nullptr); }

bool tSQLQuery_t::error(const int16_t err) const noexcept
	{ return !client || client->error(err, SQL_HANDLE_STMT, queryHandle); }
		}
	}
}