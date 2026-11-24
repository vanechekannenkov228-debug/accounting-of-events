
#include "repositories/EventTypeRepository.h"
#include "database/Statement.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    EventType readEventType(Statement& statement)
    {
        EventType eventType;

        eventType.id = statement.getInt(0);
        eventType.name = statement.getText(1);

        return eventType;
    }
}

EventTypeRepository::EventTypeRepository(Database& database)
    : database_(database)
{
}

std::optional<EventType> EventTypeRepository::findById(int typeId)
{
    const std::string sql =
        "SELECT id, name "
        "FROM Event_type "
        "WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, typeId);

    if (!statement.step())
        return std::nullopt;

    return readEventType(statement);
}

std::optional<EventType> EventTypeRepository::findByName(
    const std::string& name
)
{
    const std::string sql =
        "SELECT id, name "
        "FROM Event_type "
        "WHERE name = ?;";

    Statement statement(database_, sql);

    statement.bindText(1, name);

    if (!statement.step())
        return std::nullopt;

    return readEventType(statement);
}

std::vector<EventType> EventTypeRepository::findAll()
{
    const std::string sql =
        "SELECT id, name "
        "FROM Event_type "
        "ORDER BY id;";

    Statement statement(database_, sql);

    std::vector<EventType> eventTypes;

    while (statement.step())
    {
        eventTypes.push_back(readEventType(statement));
    }

    return eventTypes;
}

int EventTypeRepository::create(const EventType& eventType)
{
    const std::string sql =
        "INSERT INTO Event_type (name) "
        "VALUES (?);";

    Statement statement(database_, sql);

    statement.bindText(1, eventType.name);

    statement.execute();

    return static_cast<int>(statement.lastInsertId());
}

void EventTypeRepository::update(const EventType& eventType)
{
    const std::string sql =
        "UPDATE Event_type "
        "SET name = ? "
        "WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindText(1, eventType.name);
    statement.bindInt(2, eventType.id);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error("Event type not found or not updated");
    }
}

void EventTypeRepository::remove(int typeId)
{
    const std::string sql =
        "DELETE FROM Event_type "
        "WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, typeId);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error("Event type not found");
    }
}

