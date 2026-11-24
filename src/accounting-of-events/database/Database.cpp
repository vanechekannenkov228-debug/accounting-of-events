
#include "database/Database.h"

#include <iostream>

Database::Database(const std::string& databasePath)
{
    // Открываем существующую базу данных
    int result = sqlite3_open_v2(
        databasePath.c_str(),
        &db_,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::string errorMessage =
            db_ ? sqlite3_errmsg(db_) : sqlite3_errstr(result);

        if (db_ != nullptr)
        {
            sqlite3_close_v2(db_);
            db_ = nullptr;
        }

        throw std::runtime_error(
            "Database connection error: " + errorMessage
        );
    }

    try
    {
        configure();
    }
    catch (...)
    {
        sqlite3_close_v2(db_);
        db_ = nullptr;
        throw;
    }
}

Database::~Database()
{
    if (db_ != nullptr)
    {
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

void Database::configure()
{
    // Включаем расширенные коды ошибок SQLite
    sqlite3_extended_result_codes(db_, 1);

    // Ожидание освобождения БД при занятом соединении
    int result = sqlite3_busy_timeout(db_, 5000);

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(
            "Failed to configure SQLite busy timeout"
        );
    }

    // Включаем проверку внешних ключей
    execute("PRAGMA foreign_keys = ON;");
}

void Database::execute(const std::string& sql)
{
    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db_,
        sql.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK)
    {
        std::string message = errorMessage
            ? errorMessage
            : sqlite3_errmsg(db_);

        sqlite3_free(errorMessage);

        throw std::runtime_error(
            "SQL execution error: " + message
        );
    }
}

sqlite3* Database::getConnection() const
{
    return db_;
}

