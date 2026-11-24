#include "exporters/DataExporter.h"

#include <sqlite3.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

DataExporter::DataExporter(Database& database)
    : database_(database)
{
}

// ============================================================
// Получение данных из SQLite
// ============================================================

DataExporter::Value DataExporter::executeQuery(
    const std::string& sql
)
{
    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        database_.getConnection(),
        sql.c_str(),
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(
            "SQL error: " +
            std::string(sqlite3_errmsg(database_.getConnection()))
        );
    }

    Value root;
    root.type = ValueType::Object;

    Value rows;
    rows.type = ValueType::Array;

    try
    {
        int columnCount = sqlite3_column_count(statement);

        while ((result = sqlite3_step(statement)) == SQLITE_ROW)
        {
            Value row;
            row.type = ValueType::Object;

            for (int i = 0; i < columnCount; ++i)
            {
                const char* name =
                    sqlite3_column_name(statement, i);

                std::string columnName =
                    name ? name : "column_" + std::to_string(i);

                Value value;

                int type = sqlite3_column_type(statement, i);

                if (type == SQLITE_NULL)
                {
                    value.type = ValueType::Null;
                }
                else
                {
                    const unsigned char* text =
                        sqlite3_column_text(statement, i);

                    value.data = text
                        ? reinterpret_cast<const char*>(text)
                        : "";

                    if (type == SQLITE_INTEGER ||
                        type == SQLITE_FLOAT)
                    {
                        value.type = ValueType::Number;
                    }
                    else
                    {
                        value.type = ValueType::String;
                    }
                }

                // Двойное подчёркивание обозначает
                // принадлежность к вложенному объекту.
                size_t separator = columnName.find("__");

                if (separator != std::string::npos)
                {
                    std::string objectName =
                        columnName.substr(0, separator);

                    std::string fieldName =
                        columnName.substr(separator + 2);

                    Value& nested = row.object[objectName];
                    nested.type = ValueType::Object;

                    nested.object[fieldName] = value;
                }
                else
                {
                    row.object[columnName] = value;
                }
            }

            rows.array.push_back(row);
        }

        if (result != SQLITE_DONE)
        {
            throw std::runtime_error(
                sqlite3_errmsg(database_.getConnection())
            );
        }
    }
    catch (...)
    {
        sqlite3_finalize(statement);
        throw;
    }

    sqlite3_finalize(statement);

    root.object["data"] = rows;

    return root;
}

// ============================================================
// Экранирование специальных символов
// ============================================================

std::string DataExporter::escapeJson(
    const std::string& value
)
{
    std::ostringstream result;

    for (unsigned char c : value)
    {
        switch (c)
        {
            case '"':  result << "\\\""; break;
            case '\\': result << "\\\\"; break;
            case '\b': result << "\\b";  break;
            case '\f': result << "\\f";  break;
            case '\n': result << "\\n";  break;
            case '\r': result << "\\r";  break;
            case '\t': result << "\\t";  break;

            default:
                if (c < 0x20)
                {
                    result << "\\u"
                           << std::hex
                           << std::setw(4)
                           << std::setfill('0')
                           << static_cast<int>(c)
                           << std::dec;
                }
                else
                {
                    result << static_cast<char>(c);
                }
        }
    }

    return result.str();
}

std::string DataExporter::escapeXml(
    const std::string& value
)
{
    std::string result;

    for (char c : value)
    {
        switch (c)
        {
            case '&':  result += "&amp;";  break;
            case '<':  result += "&lt;";   break;
            case '>':  result += "&gt;";   break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&apos;"; break;

            default:
                result += c;
        }
    }

    return result;
}

std::string DataExporter::escapeYaml(
    const std::string& value
)
{
    // Используем двойные кавычки и JSON-экранирование.
    return "\"" + escapeJson(value) + "\"";
}

std::string DataExporter::escapeCsv(
    const std::string& value
)
{
    std::string result = "\"";

    for (char c : value)
    {
        if (c == '"')
            result += "\"\"";
        else
            result += c;
    }

    result += "\"";

    return result;
}

// ============================================================
// JSON
// ============================================================

std::string DataExporter::toJson(
    const Value& value,
    int indent
)
{
    std::string spaces(indent, ' ');

    if (value.type == ValueType::Null)
        return "null";

    if (value.type == ValueType::String)
        return "\"" + escapeJson(value.data) + "\"";

    if (value.type == ValueType::Number)
        return value.data;

    std::ostringstream result;

    if (!value.array.empty() || value.type == ValueType::Array)
    {
        result << "[\n";

        for (size_t i = 0; i < value.array.size(); ++i)
        {
            result << std::string(indent + 2, ' ')
                   << toJson(value.array[i], indent + 2);

            if (i + 1 < value.array.size())
                result << ",";

            result << "\n";
        }

        result << spaces << "]";

        return result.str();
    }

    result << "{\n";

    size_t index = 0;

    for (const auto& item : value.object)
    {
        result << std::string(indent + 2, ' ')
               << "\"" << escapeJson(item.first) << "\": "
               << toJson(item.second, indent + 2);

        if (++index < value.object.size())
            result << ",";

        result << "\n";
    }

    result << spaces << "}";

    return result.str();
}

// ============================================================
// XML
// ============================================================

std::string DataExporter::toXml(
    const Value& value,
    const std::string& tag
)
{
    std::ostringstream result;

    // Для массива создаём один корневой элемент,
    // внутри которого располагаются все записи.
    if (value.type == ValueType::Array)
    {
        result << "<" << tag << ">\n";

        for (const auto& item : value.array)
        {
            result << toXml(item, "item");
        }

        result << "</" << tag << ">\n";
        return result.str();
    }

    result << "<" << tag << ">";

    if (value.type == ValueType::Object)
    {
        result << "\n";

        for (const auto& item : value.object)
        {
            result << toXml(item.second, item.first);
        }
    }
    else if (value.type == ValueType::Null)
    {
        // Пустой XML-элемент обозначает NULL.
    }
    else
    {
        result << escapeXml(value.data);
    }

    result << "</" << tag << ">\n";
    return result.str();
}

// ============================================================
// YAML
// ============================================================

std::string DataExporter::toYaml(
    const Value& value,
    int indent
)
{
    const std::string spaces(indent, ' ');
    std::ostringstream result;

    if (value.type == ValueType::Null)
        return "null";

    if (value.type == ValueType::String)
        return escapeYaml(value.data);

    if (value.type == ValueType::Number)
        return value.data;

    if (value.type == ValueType::Array)
    {
        if (value.array.empty())
            return spaces + "[]\n";

        for (const auto& item : value.array)
        {
            result << spaces << "-";

            if (item.type == ValueType::Object ||
                item.type == ValueType::Array)
            {
                result << "\n";
                result << toYaml(item, indent + 2);
            }
            else
            {
                result << " " << toYaml(item, 0) << "\n";
            }
        }

        return result.str();
    }

    if (value.type == ValueType::Object)
    {
        if (value.object.empty())
            return spaces + "{}\n";

        for (const auto& item : value.object)
        {
            result << spaces << item.first << ":";

            if (item.second.type == ValueType::Object ||
                item.second.type == ValueType::Array)
            {
                result << "\n";
                result << toYaml(item.second, indent + 2);
            }
            else
            {
                result << " " << toYaml(item.second, 0) << "\n";
            }
        }

        return result.str();
    }

    return "";
}

// ============================================================
// CSV: преобразование вложенных объектов в плоскую таблицу
// ============================================================

void DataExporter::flatten(
    const Value& value,
    const std::string& prefix,
    std::vector<std::pair<std::string, std::string>>& result
)
{
    if (value.type == ValueType::Object)
    {
        for (const auto& item : value.object)
        {
            std::string name = prefix.empty()
                ? item.first
                : prefix + "." + item.first;

            flatten(item.second, name, result);
        }
    }
    else
    {
        std::string data;

        if (value.type != ValueType::Null)
            data = value.data;

        result.emplace_back(prefix, data);
    }
}

// ============================================================
// Сохранение файлов
// ============================================================

void DataExporter::writeFile(
    const std::string& fileName,
    const std::string& content
)
{
    std::filesystem::path outputDirectory =
        std::filesystem::path("..") / "out";

    std::filesystem::create_directories(outputDirectory);

    std::ofstream file(
        outputDirectory / fileName,
        std::ios::out | std::ios::trunc
    );

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Cannot create file: " + fileName
        );
    }

    file << content;

    if (!file.good())
    {
        throw std::runtime_error(
            "Error writing file: " + fileName
        );
    }
}

// ============================================================
// Общий экспорт
// ============================================================

void DataExporter::exportQuery(
    const std::string& sql,
    const std::string& rootName
)
{
    Value result = executeQuery(sql);

    const Value& rows = result.object.at("data");

    // JSON
    writeFile(
        "data.json",
        toJson(rows) + "\n"
    );

    // XML
    writeFile(
        "data.xml",
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
        toXml(rows, rootName)
    );

    // YAML
    Value yamlRoot;
    yamlRoot.type = ValueType::Object;
    yamlRoot.object[rootName] = rows;

    writeFile(
        "data.yaml",
        toYaml(yamlRoot)
    );

    // CSV
    std::ostringstream csv;

    if (!rows.array.empty())
    {
        std::vector<std::pair<std::string, std::string>> firstRow;

        flatten(rows.array.front(), "", firstRow);

        // Заголовки
        for (size_t i = 0; i < firstRow.size(); ++i)
        {
            csv << escapeCsv(firstRow[i].first);

            if (i + 1 < firstRow.size())
                csv << ",";
        }

        csv << "\n";

        // Данные
        for (const auto& row : rows.array)
        {
            std::vector<std::pair<std::string, std::string>> fields;

            flatten(row, "", fields);

            for (size_t i = 0; i < fields.size(); ++i)
            {
                csv << escapeCsv(fields[i].second);

                if (i + 1 < fields.size())
                    csv << ",";
            }

            csv << "\n";
        }
    }

    writeFile("data.csv", csv.str());
}

// ============================================================
// Экспорт мероприятий
// ============================================================

void DataExporter::exportEvents()
{
    const std::string sql =
        "SELECT "
        "e.id, "
        "e.title, "
        "e.description, "
        "e.location, "
        "e.event_datetime, "
        "e.Max_partipants, "
        "e.status, "

        "u.id AS organizer__id, "
        "u.full_name AS organizer__name, "
        "u.login AS organizer__login, "

        "et.id AS event_type__id, "
        "et.name AS event_type__name "

        "FROM Event e "

        "JOIN User u "
        "ON e.organizer_id = u.id "

        "JOIN Event_type et "
        "ON e.event_type_id = et.id "

        "ORDER BY e.id;";

    exportQuery(sql, "events");
}
