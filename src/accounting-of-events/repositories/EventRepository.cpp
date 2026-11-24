
#include "repositories/EventRepository.h"
#include "database/Statement.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::string statusToString(EventStatus status)
    {
        switch (status)
        {
            case EventStatus::Planned:
                return "planned";

            case EventStatus::Completed:
                return "completed";

            case EventStatus::Cancelled:
                return "cancelled";
        }

        throw std::runtime_error("Unknown event status");
    }

    EventStatus stringToStatus(const std::string& status)
    {
        if (status == "planned")
            return EventStatus::Planned;

        if (status == "completed")
            return EventStatus::Completed;

        if (status == "cancelled")
            return EventStatus::Cancelled;

        throw std::runtime_error(
            "Unknown event status: " + status
        );
    }

    Event readEvent(Statement& statement)
    {
        Event event;

        event.id = statement.getInt(0);
        event.title = statement.getText(1);
        event.description = statement.getText(2);
        event.location = statement.getText(3);
        event.eventDateTime = statement.getText(4);
        event.maxParticipants = statement.getInt(5);
        event.status = stringToStatus(statement.getText(6));
        event.organizerId = statement.getInt(7);
        event.eventTypeId = statement.getInt(8);

        return event;
    }
}

EventRepository::EventRepository(Database& database)
    : database_(database)
{
}

std::optional<Event> EventRepository::findById(int eventId)
{
    const std::string sql =
        "SELECT id, title, description, location, "
        "event_datetime, Max_partipants, status, "
        "organizer_id, event_type_id "
        "FROM Event WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, eventId);

    if (!statement.step())
        return std::nullopt;

    return readEvent(statement);
}

std::vector<Event> EventRepository::findAll()
{
    const std::string sql =
        "SELECT id, title, description, location, "
        "event_datetime, Max_partipants, status, "
        "organizer_id, event_type_id "
        "FROM Event ORDER BY id;";

    Statement statement(database_, sql);

    std::vector<Event> events;

    while (statement.step())
    {
        events.push_back(readEvent(statement));
    }

    return events;
}

std::vector<Event> EventRepository::findByOrganizerId(
    int organizerId
)
{
    const std::string sql =
        "SELECT id, title, description, location, "
        "event_datetime, Max_partipants, status, "
        "organizer_id, event_type_id "
        "FROM Event WHERE organizer_id = ? "
        "ORDER BY id;";

    Statement statement(database_, sql);

    statement.bindInt(1, organizerId);

    std::vector<Event> events;

    while (statement.step())
    {
        events.push_back(readEvent(statement));
    }

    return events;
}

std::vector<Event> EventRepository::findByStatus(
    EventStatus status
)
{
    const std::string sql =
        "SELECT id, title, description, location, "
        "event_datetime, Max_partipants, status, "
        "organizer_id, event_type_id "
        "FROM Event WHERE status = ? "
        "ORDER BY id;";

    Statement statement(database_, sql);

    statement.bindText(1, statusToString(status));

    std::vector<Event> events;

    while (statement.step())
    {
        events.push_back(readEvent(statement));
    }

    return events;
}

int EventRepository::create(const Event& event)
{
    const std::string sql =
        "INSERT INTO Event "
        "(title, description, location, event_datetime, "
        "Max_partipants, status, organizer_id, event_type_id) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    Statement statement(database_, sql);

    statement.bindText(1, event.title);
    statement.bindText(2, event.description);
    statement.bindText(3, event.location);
    statement.bindText(4, event.eventDateTime);
    statement.bindInt(5, event.maxParticipants);
    statement.bindText(6, statusToString(event.status));
    statement.bindInt(7, event.organizerId);
    statement.bindInt(8, event.eventTypeId);

    statement.execute();

    return static_cast<int>(statement.lastInsertId());
}

void EventRepository::update(const Event& event)
{
    const std::string sql =
        "UPDATE Event SET "
        "title = ?, "
        "description = ?, "
        "location = ?, "
        "event_datetime = ?, "
        "Max_partipants = ?, "
        "status = ?, "
        "organizer_id = ?, "
        "event_type_id = ? "
        "WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindText(1, event.title);
    statement.bindText(2, event.description);
    statement.bindText(3, event.location);
    statement.bindText(4, event.eventDateTime);
    statement.bindInt(5, event.maxParticipants);
    statement.bindText(6, statusToString(event.status));
    statement.bindInt(7, event.organizerId);
    statement.bindInt(8, event.eventTypeId);
    statement.bindInt(9, event.id);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error("Event not found or not updated");
    }
}

void EventRepository::remove(int eventId)
{
    const std::string sql =
        "DELETE FROM Event WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, eventId);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error("Event not found");
    }
}

