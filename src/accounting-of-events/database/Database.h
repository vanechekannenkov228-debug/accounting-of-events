#pragma once

#include <sqlite3.h>

#include <string>
#include <stdexcept>

class Database {
public:
    // Конструктор: открывает подключение к БД
    explicit Database(const std::string& databasePath);

    // Деструктор: закрывает подключение
    ~Database();

    // Запрет копирования объекта
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Выполнение SQL-команд без возврата результата
    void execute(const std::string& sql);

    // Получение указателя на соединение SQLite
    sqlite3* getConnection() const;

private:
    sqlite3* db_ = nullptr;

    // Настройка подключения
    void configure();
};

