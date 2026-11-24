#pragma once

#include "services/AuthenticationService.h"
#include "services/RegistrationService.h"
#include "services/EventService.h"

class ConsoleUI
{
public:
    ConsoleUI(
        AuthenticationService& authenticationService,
        RegistrationService& registrationService,
        EventService& eventService,
        UserRepository& userRepository,
        EventRepository& eventRepository,
        RegistrationRepository& registrationRepository
    );

    void run();

private:
    AuthenticationService& authenticationService_;
    RegistrationService& registrationService_;
    EventService& eventService_;

    UserRepository& userRepository_;
    EventRepository& eventRepository_;
    RegistrationRepository& registrationRepository_;

    void showMainMenu();
    void showStudentMenu(const User& user);
    void showOrganizerMenu(const User& user);

    void registerUser();
    void loginUser();

    void showEvents();
    void registerForEvent(const User& user);
    void showMyRegistrations(const User& user);

    void createEvent(const User& user);
    void completeEvent(const User& user);
    void cancelEvent(const User& user);
};

