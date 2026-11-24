#pragma once

#include "database/Database.h"

#include <map>
#include <string>
#include <vector>

class DataExporter
{
private:
    // Тип значения, полученного из базы данных
    enum class ValueType
    {
        Null,
        String,
        Number,
	Object,
	Array
    };

    // Универсальная структура данных
    struct Value
    {
        ValueType type = ValueType::Null;
        std::string data;

        std::map<std::string, Value> object;
        std::vector<Value> array;
    };

    Database& database_;

    // Получение данных из SQLite
    Value executeQuery(const std::string& sql);

    // Сериализация
    std::string toJson(const Value& value, int indent = 0);
    std::string toXml(const Value& value, const std::string& tag);
    std::string toYaml(const Value& value, int indent = 0);

    // Вспомогательные функции
    std::string escapeJson(const std::string& value);
    std::string escapeXml(const std::string& value);
    std::string escapeYaml(const std::string& value);
    std::string escapeCsv(const std::string& value);

    void flatten(
        const Value& value,
        const std::string& prefix,
        std::vector<std::pair<std::string, std::string>>& result
    );

    void writeFile(
        const std::string& fileName,
        const std::string& content
    );

public:
    explicit DataExporter(Database& database);

    // Экспорт произвольного SQL-запроса
    void exportQuery(
        const std::string& sql,
        const std::string& rootName
    );

    // Экспорт мероприятий со связанными данными
    void exportEvents();
};

