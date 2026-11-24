
#pragma once

#include "repositories/UserRepository.h"
#include "repositories/EventRepository.h"
#include "repositories/EventTypeRepository.h"
#include "repositories/RegistrationRepository.h"

class EventService
{
public:
    EventService(
        UserRepository& userRepository,
        EventRepository& eventRepository,
        EventTypeRepository& eventTypeRepository,
        RegistrationRepository& registrationRepository
    );

    int createEvent(const Event& event);

    void updateEvent(const Event& event);

    void completeEvent(int eventId);

    void cancelEvent(int eventId);

    void deleteEvent(int eventId);

private:
    UserRepository& userRepository_;
    EventRepository& eventRepository_;
    EventTypeRepository& eventTypeRepository_;
    RegistrationRepository& registrationRepository_;
};
