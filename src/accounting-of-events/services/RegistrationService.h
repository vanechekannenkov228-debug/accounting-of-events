
#pragma once

#include "database/Database.h"
#include "repositories/UserRepository.h"
#include "repositories/EventRepository.h"
#include "repositories/RegistrationRepository.h"

class RegistrationService
{
public:
    RegistrationService(
        Database& database,
        UserRepository& userRepository,
        EventRepository& eventRepository,
        RegistrationRepository& registrationRepository
    );

    int registerStudent(int studentId, int eventId);

    void cancelRegistration(int registrationId);

private:
    Database& database_;
    UserRepository& userRepository_;
    EventRepository& eventRepository_;
    RegistrationRepository& registrationRepository_;
};

