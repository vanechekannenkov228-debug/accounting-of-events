
#pragma once

#include <string>

enum class EventStatus
{
    Planned,
    Completed,
    Cancelled
};

struct Event
{
    int id{};

    std::string title;
    std::string description;
    std::string location;
    std::string eventDateTime;

    int maxParticipants{};

    EventStatus status{EventStatus::Planned};

    int organizerId{};
    int eventTypeId{};
};

