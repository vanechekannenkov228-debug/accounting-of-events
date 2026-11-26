#pragma once

#include <sqlite3.h>

#include <string>
#include <stdexcept>

class Database
{
public:
    // Открытие или создание базы данных
    explicit Database(const std::string& databasePath);

    // Закрытие подключения
    ~Database();

    // Запрет копирования объекта
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Выполнение SQL-команд
    void execute(const std::string& sql);

    // Получение подключения SQLite
    sqlite3* getConnection() const;

private:
    sqlite3* db_ = nullptr;

    // Настройка подключения
    void configure();

    // Создание структуры базы данных
    void initializeSchema();
};
