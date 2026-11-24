
#pragma once

#include "database/Database.h"
#include "models/Event.h"

#include <optional>
#include <string>
#include <vector>

class EventRepository
{
public:
    explicit EventRepository(Database& database);

    // Поиск мероприятий
    std::optional<Event> findById(int eventId);

    std::vector<Event> findAll();

    std::vector<Event> findByOrganizerId(int organizerId);

    std::vector<Event> findByStatus(EventStatus status);

    // CRUD
    int create(const Event& event);

    void update(const Event& event);

    void remove(int eventId);

private:
    Database& database_;
};

