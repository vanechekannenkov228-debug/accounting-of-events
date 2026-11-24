
#pragma once

#include "Database.h"

#include <sqlite3.h>
#include <stdexcept>
#include <string>

class Statement
{
public:
    Statement(Database& database, const std::string& sql)
    {
        int result = sqlite3_prepare_v2(
            database.getConnection(),
            sql.c_str(),
            -1,
            &statement_,
            nullptr
        );

        if (result != SQLITE_OK)
        {
            throw std::runtime_error(
                sqlite3_errmsg(database.getConnection())
            );
        }
    }

    ~Statement()
    {
        if (statement_ != nullptr)
        {
            sqlite3_finalize(statement_);
        }
    }

    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;

    Statement(Statement&&) = delete;
    Statement& operator=(Statement&&) = delete;

    // Получение указателя на SQLite statement
    sqlite3_stmt* get() const
    {
        return statement_;
    }

    // Привязка текстового параметра
    void bindText(int index, const std::string& value)
    {
        int result = sqlite3_bind_text(
            statement_,
            index,
            value.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        checkError(result, "Failed to bind text parameter");
    }

    // Привязка целочисленного параметра
    void bindInt(int index, int value)
    {
        int result = sqlite3_bind_int(
            statement_,
            index,
            value
        );

        checkError(result, "Failed to bind integer parameter");
    }

    // Привязка 64-битного целого числа
    void bindInt64(int index, long long value)
    {
        int result = sqlite3_bind_int64(
            statement_,
            index,
            value
        );

        checkError(result, "Failed to bind int64 parameter");
    }

    // Привязка числа с плавающей точкой
    void bindDouble(int index, double value)
    {
        int result = sqlite3_bind_double(
            statement_,
            index,
            value
        );

        checkError(result, "Failed to bind double parameter");
    }

    // Привязка NULL
    void bindNull(int index)
    {
        int result = sqlite3_bind_null(
            statement_,
            index
        );

        checkError(result, "Failed to bind NULL parameter");
    }

    // Выполнение INSERT, UPDATE, DELETE
    void execute()
    {
        int result = sqlite3_step(statement_);

        if (result != SQLITE_DONE)
        {
            throw std::runtime_error(
                sqlite3_errmsg(sqlite3_db_handle(statement_))
            );
        }
    }

    // Переход к следующей строке SELECT
    bool step()
    {
        int result = sqlite3_step(statement_);

        if (result == SQLITE_ROW)
        {
            return true;
        }

        if (result == SQLITE_DONE)
        {
            return false;
        }

        throw std::runtime_error(
            sqlite3_errmsg(sqlite3_db_handle(statement_))
        );
    }

    // Получение текстового значения
    std::string getText(int column) const
    {
        const unsigned char* value =
            sqlite3_column_text(statement_, column);

        if (value == nullptr)
        {
            return "";
        }

        return reinterpret_cast<const char*>(value);
    }

    // Получение целочисленного значения
    int getInt(int column) const
    {
        return sqlite3_column_int(statement_, column);
    }

    // Получение 64-битного целого числа
    long long getInt64(int column) const
    {
        return sqlite3_column_int64(statement_, column);
    }

    // Получение числа с плавающей точкой
    double getDouble(int column) const
    {
        return sqlite3_column_double(statement_, column);
    }

    // Проверка значения на NULL
    bool isNull(int column) const
    {
        return sqlite3_column_type(statement_, column) == SQLITE_NULL;
    }

    // Получение ID последней вставленной записи
    long long lastInsertId() const
    {
        return sqlite3_last_insert_rowid(
            sqlite3_db_handle(statement_)
        );
    }

    // Получение количества изменённых строк
    int changes() const
    {
        return sqlite3_changes(
            sqlite3_db_handle(statement_)
        );
    }

private:
    sqlite3_stmt* statement_ = nullptr;

    void checkError(int result, const std::string& message)
    {
        if (result != SQLITE_OK)
        {
            throw std::runtime_error(
                message + ": " +
                sqlite3_errmsg(sqlite3_db_handle(statement_))
            );
        }
    }
};

