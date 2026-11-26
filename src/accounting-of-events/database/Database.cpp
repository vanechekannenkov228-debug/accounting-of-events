#include "database/Database.h"

#include <filesystem>

Database::Database(const std::string& databasePath)
{
    // Создание каталог базы данных, если его нет.
    std::filesystem::path path(databasePath);

    if (!path.parent_path().empty())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    // Открываем существующую базу или создаём новую.
    int result = sqlite3_open_v2(
        databasePath.c_str(),
        &db_,
        SQLITE_OPEN_READWRITE |
        SQLITE_OPEN_CREATE |
        SQLITE_OPEN_FULLMUTEX,
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
        initializeSchema();
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
    // Включаем расширенные коды ошибок SQLite.
    sqlite3_extended_result_codes(db_, 1);

    // Ожидание освобождения базы при блокировке.
    int result = sqlite3_busy_timeout(db_, 5000);

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(
            "Failed to configure SQLite busy timeout"
        );
    }

    // Включаем проверку внешних ключей.
    execute("PRAGMA foreign_keys = ON;");
}

void Database::initializeSchema()
{
    // Создаём таблицы в одной транзакции.
    execute("BEGIN IMMEDIATE;");

    try
    {
        execute(
            "CREATE TABLE IF NOT EXISTS User ("
            "ID INTEGER PRIMARY KEY, "
            "Full_name TEXT NOT NULL, "
            "Login TEXT NOT NULL UNIQUE, "
            "Password_hash TEXT NOT NULL, "
            "Role TEXT NOT NULL "
            "CHECK (Role IN ('Student', 'Organizer', 'Admin'))"
            ");"
        );

        execute(
            "CREATE TABLE IF NOT EXISTS Event_type ("
            "ID INTEGER NOT NULL PRIMARY KEY, "
            "Name TEXT NOT NULL UNIQUE"
            ");"
        );

        execute(
            "CREATE TABLE IF NOT EXISTS Event ("
            "ID INTEGER NOT NULL PRIMARY KEY, "
            "Title TEXT NOT NULL, "
            "Description TEXT NOT NULL, "
            "Location TEXT NOT NULL, "
            "Event_datetime TEXT NOT NULL, "
            "Max_partipants INTEGER NOT NULL "
            "CHECK (Max_partipants > 0), "
            "Status TEXT NOT NULL DEFAULT 'planned' "
            "CHECK (Status IN ('planned', 'completed', 'cancelled')), "
            "Organizer_ID INTEGER NOT NULL, "
            "Event_type_ID INTEGER NOT NULL, "
            "FOREIGN KEY (Organizer_ID) REFERENCES User(ID), "
            "FOREIGN KEY (Event_type_ID) REFERENCES Event_type(ID)"
            ");"
        );

        execute(
            "CREATE TABLE IF NOT EXISTS Registration ("
            "ID INTEGER NOT NULL PRIMARY KEY, "
            "Event_ID INTEGER NOT NULL, "
            "Student_ID INTEGER NOT NULL, "
            "Registration_status TEXT NOT NULL DEFAULT 'registered' "
            "CHECK (Registration_status IN ('registered', 'cancelled')), "
            "Attendance_status TEXT, "
            "CHECK (Attendance_status IN ('attended', 'absent') "
            "OR Attendance_status IS NULL), "
            "FOREIGN KEY (Event_ID) REFERENCES Event(ID), "
            "FOREIGN KEY (Student_ID) REFERENCES User(ID), "
            "UNIQUE (Student_ID, Event_ID)"
            ");"
        );

        execute("COMMIT;");
    }
    catch (...)
    {
        try
        {
            execute("ROLLBACK;");
        }
        catch (...)
        {
            // Сохраняем исходную ошибку.
        }

        throw;
    }
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
