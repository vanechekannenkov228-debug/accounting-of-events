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

// Проверка прав организатора
void EventService::checkOrganizerAccess(const User& currentUser)
{
    auto user = userRepository_.findById(currentUser.id);

    if (!user)
        throw std::runtime_error("User not found");

    if (user->role != UserRole::Organizer)
        throw std::runtime_error("Access denied: organizer role required");
}

// Создание мероприятия
int EventService::createEvent(const User& currentUser, const Event& event)
{
    checkOrganizerAccess(currentUser);

    if (event.organizerId != currentUser.id)
        throw std::runtime_error("You can create events only for yourself");

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

    auto eventType = eventTypeRepository_.findById(event.eventTypeId);

    if (!eventType)
        throw std::runtime_error("Event type not found");

    Event newEvent = event;
    newEvent.id = 0;
    newEvent.status = EventStatus::Planned;

    return eventRepository_.create(newEvent);
}

// Обновление мероприятия
void EventService::updateEvent(const User& currentUser, const Event& event)
{
    checkOrganizerAccess(currentUser);

    auto existingEvent = eventRepository_.findById(event.id);

    if (!existingEvent)
        throw std::runtime_error("Event not found");

    if (existingEvent->organizerId != currentUser.id)
        throw std::runtime_error("You can modify only your own events");

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
        {
            throw std::runtime_error(
                "Maximum participants cannot be less than active registrations"
            );
        }
    }

    if (event.organizerId != existingEvent->organizerId)
        throw std::runtime_error("Changing event organizer is not allowed");

    auto eventType = eventTypeRepository_.findById(event.eventTypeId);

    if (!eventType)
        throw std::runtime_error("Event type not found");

    Event updatedEvent = event;
    updatedEvent.status = existingEvent->status;

    eventRepository_.update(updatedEvent);
}

// Завершение мероприятия
void EventService::completeEvent(const User& currentUser, int eventId)
{
    checkOrganizerAccess(currentUser);

    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    if (event->organizerId != currentUser.id)
        throw std::runtime_error("You can modify only your own events");

    if (event->status != EventStatus::Planned)
        throw std::runtime_error("Only planned events can be completed");

    event->status = EventStatus::Completed;

    eventRepository_.update(*event);
}

// Отмена мероприятия
void EventService::cancelEvent(const User& currentUser, int eventId)
{
    checkOrganizerAccess(currentUser);

    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    if (event->organizerId != currentUser.id)
        throw std::runtime_error("You can modify only your own events");

    if (event->status != EventStatus::Planned)
        throw std::runtime_error("Only planned events can be cancelled");

    event->status = EventStatus::Cancelled;

    eventRepository_.update(*event);
}

// Удаление мероприятия
void EventService::deleteEvent(const User& currentUser, int eventId)
{
    checkOrganizerAccess(currentUser);

    auto event = eventRepository_.findById(eventId);

    if (!event)
        throw std::runtime_error("Event not found");

    if (event->organizerId != currentUser.id)
        throw std::runtime_error("You can delete only your own events");

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

