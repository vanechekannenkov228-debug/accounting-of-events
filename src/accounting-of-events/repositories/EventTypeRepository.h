
#pragma once

#include "database/Database.h"
#include "models/EventType.h"

#include <optional>
#include <string>
#include <vector>

class EventTypeRepository
{
public:
    explicit EventTypeRepository(Database& database);

    // Поиск типов мероприятий
    std::optional<EventType> findById(int typeId);

    std::optional<EventType> findByName(
        const std::string& name
    );

    std::vector<EventType> findAll();

    // CRUD
    int create(const EventType& eventType);

    void update(const EventType& eventType);

    void remove(int typeId);

private:
    Database& database_;
};

