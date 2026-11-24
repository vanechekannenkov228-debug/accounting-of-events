
#include "ui/ConsoleUI.h"

#include <iostream>
#include <limits>
#include <string>
#include <stdexcept>

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
    int choice = -1;

    while (choice != 0)
    {
        showMainMenu();

        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Некорректный ввод.\n";
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
                std::cout << "Завершение программы.\n";
                break;

            default:
                std::cout << "Неизвестный пункт меню.\n";
        }
    }
}

void ConsoleUI::showMainMenu()
{
    std::cout << "\n";
    std::cout << "============================\n";
    std::cout << "     ACCOUNTING OF EVENTS\n";
    std::cout << "============================\n";
    std::cout << "1. Регистрация\n";
    std::cout << "2. Вход\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите действие: ";
}

void ConsoleUI::registerUser()
{
    std::string fullName;
    std::string login;
    std::string password;
    int roleChoice = 0;

    std::cout << "\n=== Регистрация ===\n";

    std::cout << "ФИО: ";
    std::getline(std::cin, fullName);

    std::cout << "Логин: ";
    std::getline(std::cin, login);

    std::cout << "Пароль: ";
    std::getline(std::cin, password);

    std::cout << "\nВыберите роль:\n";
    std::cout << "1. Студент\n";
    std::cout << "2. Организатор\n";
    std::cout << "Ваш выбор: ";

    std::cin >> roleChoice;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректный ввод роли.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    UserRole role;

    if (roleChoice == 1)
    {
        role = UserRole::Student;
    }
    else if (roleChoice == 2)
    {
        role = UserRole::Organizer;
    }
    else
    {
        std::cout << "Неизвестная роль.\n";
        return;
    }

    try
    {
        int userId = authenticationService_.registerUser(
            fullName,
            login,
            password,
            role
        );

        std::cout << "Регистрация успешна!\n";
        std::cout << "Ваш ID: " << userId << '\n';
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка регистрации: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::loginUser()
{
    std::string login;
    std::string password;

    std::cout << "\n=== Вход ===\n";

    std::cout << "Логин: ";
    std::getline(std::cin, login);

    std::cout << "Пароль: ";
    std::getline(std::cin, password);

    try
    {
        User user = authenticationService_.login(login, password);

        std::cout << "\nДобро пожаловать, "
                  << user.fullName << "!\n";

        if (user.role == UserRole::Student)
        {
            showStudentMenu(user);
        }
        else
        {
            showOrganizerMenu(user);
        }
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка входа: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::showStudentMenu(const User& user)
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\n=== Меню студента ===\n";
        std::cout << "1. Посмотреть мероприятия\n";
        std::cout << "2. Записаться на мероприятие\n";
        std::cout << "3. Мои регистрации\n";
        std::cout << "0. Выйти из аккаунта\n";
        std::cout << "Ваш выбор: ";

        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Некорректный ввод.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

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
                std::cout << "Вы вышли из аккаунта.\n";
                break;

            default:
                std::cout << "Неизвестный пункт меню.\n";
        }
    }
}

void ConsoleUI::showOrganizerMenu(const User& user)
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\n=== Меню организатора ===\n";
        std::cout << "1. Посмотреть мероприятия\n";
        std::cout << "2. Создать мероприятие\n";
        std::cout << "3. Завершить мероприятие\n";
        std::cout << "4. Отменить мероприятие\n";
        std::cout << "0. Выйти из аккаунта\n";
        std::cout << "Ваш выбор: ";

        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Некорректный ввод.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

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
                std::cout << "Вы вышли из аккаунта.\n";
                break;

            default:
                std::cout << "Неизвестный пункт меню.\n";
        }
    }
}

void ConsoleUI::showEvents()
{
    std::cout << "\n=== Список мероприятий ===\n";

    try
    {
        std::vector<Event> events = eventRepository_.findAll();

        if (events.empty())
        {
            std::cout << "Мероприятий пока нет.\n";
            return;
        }

        for (const Event& event : events)
        {
            std::cout << "\n----------------------------\n";
            std::cout << "ID: " << event.id << '\n';
            std::cout << "Название: " << event.title << '\n';
            std::cout << "Описание: " << event.description << '\n';
            std::cout << "Место: " << event.location << '\n';
            std::cout << "Дата и время: " << event.eventDateTime << '\n';
            std::cout << "Максимум участников: "
                      << event.maxParticipants << '\n';
            std::cout << "ID организатора: "
                      << event.organizerId << '\n';
            std::cout << "ID типа мероприятия: "
                      << event.eventTypeId << '\n';

            std::cout << "Статус: ";

            switch (event.status)
            {
                case EventStatus::Planned:
                    std::cout << "Запланировано";
                    break;

                case EventStatus::Completed:
                    std::cout << "Завершено";
                    break;

                case EventStatus::Cancelled:
                    std::cout << "Отменено";
                    break;
            }

            std::cout << '\n';
        }
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка получения мероприятий: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::registerForEvent(const User& user)
{
    int eventId = 0;

    std::cout << "\n=== Запись на мероприятие ===\n";
    std::cout << "Введите ID мероприятия: ";
    std::cin >> eventId;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректный ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    try
    {
        registrationService_.registerStudent(user.id, eventId);
        std::cout << "Вы успешно записаны на мероприятие!\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка записи: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::showMyRegistrations(const User& user)
{
    std::cout << "\n=== Мои регистрации ===\n";

    try
    {
        std::vector<Registration> registrations =
            registrationRepository_.findByStudentId(user.id);

        if (registrations.empty())
        {
            std::cout << "У вас пока нет регистраций.\n";
            return;
        }

        for (const Registration& registration : registrations)
        {
            std::cout << "\n----------------------------\n";
            std::cout << "ID регистрации: "
                      << registration.id << '\n';
            std::cout << "ID мероприятия: "
                      << registration.eventId << '\n';

            auto event = eventRepository_.findById(
                registration.eventId
            );

            if (event.has_value())
            {
                std::cout << "Название: " << event->title << '\n';
            }

            std::cout << "Статус регистрации: ";

            if (registration.status == RegistrationStatus::Registered)
            {
                std::cout << "Активна";
            }
            else
            {
                std::cout << "Отменена";
            }

            std::cout << '\n';

            std::cout << "Посещаемость: ";

            if (!registration.attendanceStatus.has_value())
            {
                std::cout << "Не отмечена";
            }
            else if (
                registration.attendanceStatus.value()
                == AttendanceStatus::Attended
            )
            {
                std::cout << "Присутствовал";
            }
            else
            {
                std::cout << "Отсутствовал";
            }

            std::cout << '\n';
        }
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка получения регистраций: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::createEvent(const User& user)
{
    Event event;

    event.organizerId = user.id;

    std::cout << "\n=== Создание мероприятия ===\n";

    std::cout << "Название: ";
    std::getline(std::cin, event.title);

    std::cout << "Описание: ";
    std::getline(std::cin, event.description);

    std::cout << "Место проведения: ";
    std::getline(std::cin, event.location);

    std::cout << "Дата и время (ГГГГ-ММ-ДД ЧЧ:ММ): ";
    std::getline(std::cin, event.eventDateTime);

    std::cout << "Максимальное количество участников: ";
    std::cin >> event.maxParticipants;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректное количество участников.\n";
        return;
    }

    std::cout << "ID типа мероприятия: ";
    std::cin >> event.eventTypeId;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректный ID типа мероприятия.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    try
    {
        int eventId = eventService_.createEvent(event);

        std::cout << "Мероприятие успешно создано!\n";
        std::cout << "ID мероприятия: " << eventId << '\n';
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка создания мероприятия: "
                  << error.what() << '\n';
    }
}

void ConsoleUI::completeEvent(const User& user)
{
    (void)user;

    int eventId = 0;

    std::cout << "\n=== Завершение мероприятия ===\n";
    std::cout << "Введите ID мероприятия: ";
    std::cin >> eventId;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректный ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    try
    {
        eventService_.completeEvent(eventId);
        std::cout << "Мероприятие завершено.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка: " << error.what() << '\n';
    }
}

void ConsoleUI::cancelEvent(const User& user)
{
    (void)user;

    int eventId = 0;

    std::cout << "\n=== Отмена мероприятия ===\n";
    std::cout << "Введите ID мероприятия: ";
    std::cin >> eventId;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Некорректный ID.\n";
        return;
    }

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    try
    {
        eventService_.cancelEvent(eventId);
        std::cout << "Мероприятие отменено.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка: " << error.what() << '\n';
    }
}

