
#include "ui/ConsoleUI.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

ConsoleUI::ConsoleUI(
    AuthenticationService& authenticationService,
    RegistrationService& registrationService,
    EventService& eventService,
    UserRepository& userRepository,
    EventRepository& eventRepository,
    RegistrationRepository& registrationRepository
)
    : authenticationService_(authenticationService),
      registrationService_(registrationService),
      eventService_(eventService),
      userRepository_(userRepository),
      eventRepository_(eventRepository),
      registrationRepository_(registrationRepository)
{
}

void ConsoleUI::run()
{
    bool running = true;

    while (running)
    {
        try
        {
            showMainMenu();

            int choice;
            std::cout << "Select option: ";

            if (!(std::cin >> choice))
            {
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(),
                    '\n'
                );

                std::cout << "Invalid input.\n\n";
                continue;
            }

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            switch (choice)
            {
                case 1:
                    registerUser();
                    break;

                case 2:
                    loginUser();
                    break;

                case 0:
                    running = false;
                    std::cout << "Goodbye!\n";
                    break;

                default:
                    std::cout << "Unknown option.\n";
                    break;
            }
        }
        catch (const std::exception& error)
        {
            std::cout << "\nError: " << error.what() << "\n";
        }

        std::cout << '\n';
    }
}

void ConsoleUI::showMainMenu()
{
    std::cout << "============================\n";
    std::cout << "     ACCOUNTING OF EVENTS\n";
    std::cout << "============================\n";
    std::cout << "1. Register\n";
    std::cout << "2. Login\n";
    std::cout << "0. Exit\n";
    std::cout << "============================\n";
}

void ConsoleUI::registerUser()
{
    std::string fullName;
    std::string login;
    std::string password;
    int roleChoice;

    std::cout << "\n===== USER REGISTRATION =====\n";

    std::cout << "Full name: ";
    std::getline(std::cin, fullName);

    std::cout << "Login: ";
    std::getline(std::cin, login);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    std::cout << "\nSelect role:\n";
    std::cout << "1. Student\n";
    std::cout << "2. Organizer\n";
    std::cout << "Role: ";

    if (!(std::cin >> roleChoice))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid role.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    UserRole role;

    if (roleChoice == 1)
        role = UserRole::Student;
    else if (roleChoice == 2)
        role = UserRole::Organizer;
    else
    {
        std::cout << "Invalid role.\n";
        return;
    }

    int userId = authenticationService_.registerUser(
        fullName,
        login,
        password,
        role
    );

    std::cout << "\nRegistration successful!\n";
    std::cout << "Your user ID: " << userId << '\n';
}

void ConsoleUI::loginUser()
{
    std::string login;
    std::string password;

    std::cout << "\n===== LOGIN =====\n";

    std::cout << "Login: ";
    std::getline(std::cin, login);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    User user = authenticationService_.login(login, password);

    std::cout << "\nWelcome, " << user.fullName << "!\n";

    if (user.role == UserRole::Student)
        showStudentMenu(user);
    else if (user.role == UserRole::Organizer)
        showOrganizerMenu(user);
    else
        throw std::runtime_error("Unknown user role");
}

void ConsoleUI::showStudentMenu(const User& user)
{
    bool loggedIn = true;

    while (loggedIn)
    {
        std::cout << "\n============================\n";
        std::cout << "       STUDENT MENU\n";
        std::cout << "============================\n";
        std::cout << "1. Show events\n";
        std::cout << "2. Register for event\n";
        std::cout << "3. My registrations\n";
        std::cout << "0. Logout\n";
        std::cout << "============================\n";
        std::cout << "Select option: ";

        int choice;

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Invalid input.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        try
        {
            switch (choice)
            {
                case 1:
                    showEvents();
                    break;

                case 2:
                    registerForEvent(user);
                    break;

                case 3:
                    showMyRegistrations(user);
                    break;

                case 0:
                    loggedIn = false;
                    std::cout << "Logged out.\n";
                    break;

                default:
                    std::cout << "Unknown option.\n";
                    break;
            }
        }
        catch (const std::exception& error)
        {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}

void ConsoleUI::showOrganizerMenu(const User& user)
{
    bool loggedIn = true;

    while (loggedIn)
    {
        std::cout << "\n============================\n";
        std::cout << "      ORGANIZER MENU\n";
        std::cout << "============================\n";
        std::cout << "1. Show events\n";
        std::cout << "2. Create event\n";
        std::cout << "3. Complete event\n";
        std::cout << "4. Cancel event\n";
        std::cout << "0. Logout\n";
        std::cout << "============================\n";
        std::cout << "Select option: ";

        int choice;

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Invalid input.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        try
        {
            switch (choice)
            {
                case 1:
                    showEvents();
                    break;

                case 2:
                    createEvent(user);
                    break;

                case 3:
                    completeEvent(user);
                    break;

                case 4:
                    cancelEvent(user);
                    break;

                case 0:
                    loggedIn = false;
                    std::cout << "Logged out.\n";
                    break;

                default:
                    std::cout << "Unknown option.\n";
                    break;
            }
        }
        catch (const std::exception& error)
        {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}

void ConsoleUI::showEvents()
{
    auto events = eventRepository_.findAll();

    std::cout << "\n===== EVENTS =====\n";

    if (events.empty())
    {
        std::cout << "No events found.\n";
        return;
    }

    for (const auto& event : events)
    {
        std::cout << "\nID: " << event.id << '\n';
        std::cout << "Title: " << event.title << '\n';
        std::cout << "Description: " << event.description << '\n';
        std::cout << "Location: " << event.location << '\n';
        std::cout << "Date: " << event.eventDateTime << '\n';
        std::cout << "Max participants: "
                  << event.maxParticipants << '\n';

        std::cout << "Status: ";

        switch (event.status)
        {
            case EventStatus::Planned:
                std::cout << "Planned";
                break;

            case EventStatus::Completed:
                std::cout << "Completed";
                break;

            case EventStatus::Cancelled:
                std::cout << "Cancelled";
                break;
        }

        std::cout << '\n';
    }
}

void ConsoleUI::registerForEvent(const User& user)
{
    int eventId;

    std::cout << "\n===== EVENT REGISTRATION =====\n";
    std::cout << "Enter event ID: ";

    if (!(std::cin >> eventId))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid event ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    int registrationId = registrationService_.registerStudent(
        user.id,
        eventId
    );

    std::cout << "Registration successful!\n";
    std::cout << "Registration ID: " << registrationId << '\n';
}

void ConsoleUI::showMyRegistrations(const User& user)
{
    auto registrations =
        registrationRepository_.findByStudentId(user.id);

    std::cout << "\n===== MY REGISTRATIONS =====\n";

    bool found = false;

    for (const auto& registration : registrations)
    {
        auto event = eventRepository_.findById(registration.eventId);

        if (!event)
            continue;

        found = true;

        std::cout << "\nRegistration ID: " << registration.id << '\n';
        std::cout << "Event: " << event->title << '\n';
        std::cout << "Date: " << event->eventDateTime << '\n';

        std::cout << "Registration status: ";

        if (registration.status == RegistrationStatus::Registered)
            std::cout << "Registered";
        else
            std::cout << "Cancelled";

        std::cout << '\n';
    }

    if (!found)
        std::cout << "You have no registrations.\n";
}

void ConsoleUI::createEvent(const User& user)
{
    Event event;

    std::cout << "\n===== CREATE EVENT =====\n";

    std::cout << "Title: ";
    std::getline(std::cin, event.title);

    std::cout << "Description: ";
    std::getline(std::cin, event.description);

    std::cout << "Location: ";
    std::getline(std::cin, event.location);

    std::cout << "Date and time (YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, event.eventDateTime);

    std::cout << "Maximum participants: ";

    if (!(std::cin >> event.maxParticipants))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid capacity.\n";
        return;
    }

    std::cout << "Event type ID: ";

    if (!(std::cin >> event.eventTypeId))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid event type ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    event.organizerId = user.id;

    int eventId = eventService_.createEvent(event);

    std::cout << "\nEvent created successfully!\n";
    std::cout << "Event ID: " << eventId << '\n';
}

void ConsoleUI::completeEvent(const User& user)
{
    (void)user;

    int eventId;

    std::cout << "\n===== COMPLETE EVENT =====\n";
    std::cout << "Enter event ID: ";

    if (!(std::cin >> eventId))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid event ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    eventService_.completeEvent(eventId);

    std::cout << "Event completed successfully.\n";
}

void ConsoleUI::cancelEvent(const User& user)
{
    (void)user;

    int eventId;

    std::cout << "\n===== CANCEL EVENT =====\n";
    std::cout << "Enter event ID: ";

    if (!(std::cin >> eventId))
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Invalid event ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    eventService_.cancelEvent(eventId);

    std::cout << "Event cancelled successfully.\n";
}

