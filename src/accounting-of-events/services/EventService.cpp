#include "EventService.h"

#include <stdexcept>

EventService::EventService(
    UserRepository& userRepository,
    EventRepository& eventRepository,
    EventTypeRepository& eventTypeRepository,
    RegistrationRepository& registrationRepository
)
    : userRepository_(userRepository),
      eventRepository_(eventRepository),
      eventTypeRepository_(eventTypeRepository),
      registrationRepository_(registrationRepository)
{
}

int EventService::createEvent(const Event& event)
{
    if (event.title.empty())
        throw std::runtime_error("Event title cannot be empty");

    if (event.description.empty())
        throw std::runtime_error("Event description cannot be empty");

    if (event.location.empty())
        throw std::runtime_error("Event location cannot be empty");

    if (event.eventDateTime.empty())
        throw std::runtime_error("Event date and time cannot be empty");

    if (event.maxParticipants <= 0)
        throw std::runtime_error("Maximum participants must be greater than zero");

    auto organizer = userRepository_.findById(event.organizerId);

    if (!organizer)
        throw std::runtime_error("Organizer not found");

    if (organizer->role != UserRole::Organizer)
        throw std::runtime_error("Only organizers can create events");

    auto eventType = eventTypeRepository_.findById(event.eventTypeId);

    if (!eventType)
        throw std::runtime_error("Event type not found");

    Event newEvent = event;
    newEvent.id = 0;
    newEvent.status = EventStatus::Planned;

    return eventRepository_.create(newEvent);
}

void EventService::updateEvent(const Event& event)
{
    auto existingEvent = eventRepository_.findById(event.id);

    if (!existingEvent)
        throw std::runtime_error("Event not found");

    if (event.title.empty())
        throw std::runtime_error("Event title cannot be empty");

    if (event.description.empty())
        throw std::runtime_error("Event description cannot be empty");

    if (event.location.empty())
        throw std::runtime_error("Event location cannot be empty");

    if (event.eventDateTime.empty())
        throw std::runtime_error("Event date and time cannot be empty");

    if (event.maxParticipants <= 0)
        throw std::runtime_error("Maximum participants must be greater than zero");

    if (event.maxParticipants < existingEvent->maxParticipants)
    {
        auto registrations =
            registrationRepository_.findByEventId(event.id);

        int activeRegistrations = 0;

        for (const auto& registration : registrations)
        {
            if (registration.status == RegistrationStatus::Registered)
                ++activeRegistrations;
        }

        if (event.maxParticipants < activeRegistrations)
            throw std::runtime_error(
                "Maximum participants cannot be less than active registrations"
            );
    }

    auto organizer = userRepository_.findById(event.organizerId);

    if (!organizer)
        throw std::runtime_error("Organizer not found");

    if (organizer->role != UserRole::Organizer)
        throw std::runtime_error("Event owner must have organizer role");

    if (event.organizerId != existingEvent->organizerId)
        throw std::runtime_error("Changing event organizer is not allowed");

    auto eventType = eventTypeRepository_.findById(event.eventTypeId);

    if (!eventType)
        throw std::runtime_error("Event type not found");

    Event updatedEvent = event;
    updatedEvent.status = existingEvent->status;

    eventRepository_.update(updatedEvent);
}

void EventService::completeEvent(int eventId)
{
    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    if (event->status != EventStatus::Planned)
        throw std::runtime_error("Only planned events can be completed");

    event->status = EventStatus::Completed;

    eventRepository_.update(*event);
}

void EventService::cancelEvent(int eventId)
{
    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    if (event->status != EventStatus::Planned)
        throw std::runtime_error("Only planned events can be cancelled");

    event->status = EventStatus::Cancelled;

    eventRepository_.update(*event);
}

void EventService::deleteEvent(int eventId)
{
    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    auto registrations =
        registrationRepository_.findByEventId(eventId);

    if (!registrations.empty())
    {
        throw std::runtime_error(
            "Cannot delete event because registration history exists"
        );
    }

    eventRepository_.remove(eventId);
}
