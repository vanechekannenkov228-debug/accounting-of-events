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

    int createEvent(const User& currentUser, const Event& event);

    void updateEvent(const User& currentUser, const Event& event);

    void completeEvent(const User& currentUser, int eventId);

    void cancelEvent(const User& currentUser, int eventId);

    void deleteEvent(const User& currentUser, int eventId);

private:
    UserRepository& userRepository_;
    EventRepository& eventRepository_;
    EventTypeRepository& eventTypeRepository_;
    RegistrationRepository& registrationRepository_;

    void checkOrganizerAccess(const User& currentUser);
};

