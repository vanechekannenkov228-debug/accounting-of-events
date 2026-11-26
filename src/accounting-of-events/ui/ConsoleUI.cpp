#include "ui/ConsoleUI.h"

#include <iostream>
#include <limits>
#include <string>
#include <stdexcept>
#include <vector>

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

// Проверка наличия администратора
bool ConsoleUI::hasAdmin()
{
    const std::vector<User> users = userRepository_.findAll();

    for (const User& user : users)
    {
        if (user.role == UserRole::Admin)
            return true;
    }

    return false;
}

// Первоначальная настройка администратора
bool ConsoleUI::setupAdmin()
{
    std::cout << "\n====================================\n";
    std::cout << "   ПЕРВОНАЧАЛЬНАЯ НАСТРОЙКА\n";
    std::cout << "====================================\n";
    std::cout << "Администратор ещё не создан.\n";
    std::cout << "Создайте первую учётную запись администратора.\n";

    while (!hasAdmin())
    {
        std::string fullName;
        std::string login;
        std::string password;

        std::cout << "\nФИО: ";

        if (!std::getline(std::cin, fullName))
            return false;

        std::cout << "Логин: ";

        if (!std::getline(std::cin, login))
            return false;

        std::cout << "Пароль: ";

        if (!std::getline(std::cin, password))
            return false;

        try
        {
            int adminId = authenticationService_.registerAdmin(
                fullName,
                login,
                password
            );

            std::cout << "\nАдминистратор успешно создан!\n";
            std::cout << "Ваш ID: " << adminId << '\n';

            return true;
        }
        catch (const std::exception& error)
        {
            std::cout << "\nОшибка создания администратора: "
                      << error.what() << '\n';

            std::cout << "Попробуйте ещё раз.\n";
        }
    }

    return true;
}

// Запуск программы

void ConsoleUI::run()
{
    // Если администратора нет, запускаем первоначальную настройку.
    if (!hasAdmin())
    {
        if (!setupAdmin())
        {
            std::cout << "\nВвод прерван. Программа завершает работу.\n";
            return;
        }
    }

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


// Главное меню
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

// Регистрация пользователя
void ConsoleUI::registerUser()
{
    std::string fullName;
    std::string login;
    std::string password;

    std::cout << "\n=== Регистрация ===\n";

    std::cout << "ФИО: ";
    std::getline(std::cin, fullName);

    std::cout << "Логин: ";
    std::getline(std::cin, login);

    std::cout << "Пароль: ";
    std::getline(std::cin, password);

    try
    {
        int userId = authenticationService_.registerUser(
            fullName,
            login,
            password
        );

        std::cout << "\nРегистрация успешна!\n";
        std::cout << "Ваша роль: Студент\n";
        std::cout << "Ваш ID: " << userId << '\n';
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка регистрации: "
                  << error.what() << '\n';
    }
}

// Авторизация пользователя
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

        switch (user.role)
        {
            case UserRole::Student:
                showStudentMenu(user);
                break;

            case UserRole::Organizer:
                showOrganizerMenu(user);
                break;

            case UserRole::Admin:
                showAdminMenu(user);
                break;
        }
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка входа: "
                  << error.what() << '\n';
    }
}

// Меню студента
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

// Меню организатора
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

// Меню администратора

void ConsoleUI::showAdminMenu(const User& user)
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\n=== Меню администратора ===\n";
        std::cout << "1. Посмотреть пользователей\n";
        std::cout << "2. Изменить роль пользователя\n";
        std::cout << "3. Удалить пользователя\n";
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
                showUsers();
                break;

            case 2:
                changeUserRole(user);
                break;

            case 3:
                deleteUser(user);
                break;

            case 0:
                std::cout << "Вы вышли из аккаунта.\n";
                break;

            default:
                std::cout << "Неизвестный пункт меню.\n";
        }
    }
}

// Просмотр пользователей
void ConsoleUI::showUsers()
{
    std::cout << "\n=== Список пользователей ===\n";

    try
    {
        const std::vector<User> users = userRepository_.findAll();

        if (users.empty())
        {
            std::cout << "Пользователей пока нет.\n";
            return;
        }

        for (const User& user : users)
        {
            std::cout << "\n----------------------------\n";
            std::cout << "ID: " << user.id << '\n';
            std::cout << "ФИО: " << user.fullName << '\n';
            std::cout << "Логин: " << user.login << '\n';
            std::cout << "Роль: ";

            switch (user.role)
            {
                case UserRole::Student:
                    std::cout << "Студент";
                    break;

                case UserRole::Organizer:
                    std::cout << "Организатор";
                    break;

                case UserRole::Admin:
                    std::cout << "Администратор";
                    break;
            }

            std::cout << '\n';
        }
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка получения пользователей: "
                  << error.what() << '\n';
    }
}

// Изменение роли пользователя
void ConsoleUI::changeUserRole(const User& currentUser)
{
    int userId = 0;
    int roleChoice = 0;

    std::cout << "\n=== Изменение роли пользователя ===\n";

    showUsers();

    std::cout << "\nВведите ID пользователя: ";
    std::cin >> userId;

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
        auto user = userRepository_.findById(userId);

        if (!user.has_value())
        {
            std::cout << "Пользователь не найден.\n";
            return;
        }

        if (user->role == UserRole::Admin)
        {
            std::cout << "Изменять роль администратора нельзя.\n";
            return;
        }

        if (user->id == currentUser.id)
        {
            std::cout << "Нельзя изменять собственную роль.\n";
            return;
        }

        std::cout << "\nВыберите новую роль:\n";
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

            std::cout << "Некорректный ввод.\n";
            return;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        UserRole newRole;

        if (roleChoice == 1)
        {
            newRole = UserRole::Student;
        }
        else if (roleChoice == 2)
        {
            newRole = UserRole::Organizer;
        }
        else
        {
            std::cout << "Неизвестная роль.\n";
            return;
        }

        if (user->role == newRole)
        {
            std::cout << "У пользователя уже установлена эта роль.\n";
            return;
        }

        user->role = newRole;
        userRepository_.update(*user);

        std::cout << "Роль пользователя успешно изменена.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка изменения роли: "
                  << error.what() << '\n';
    }
}

// Удаление пользователя
void ConsoleUI::deleteUser(const User& currentUser)
{
    int userId = 0;

    while (true)
    {
        std::cout << "\n=== Удаление пользователя ===\n";

        // Автоматически выводим актуальный список.
        showUsers();

        const std::vector<User> users = userRepository_.findAll();

        if (users.empty())
        {
            std::cout << "Пользователей нет.\n";
            return;
        }

        std::cout << "\nВведите ID пользователя для удаления\n";
        std::cout << "(0 — вернуться назад): ";

        std::cin >> userId;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Некорректный ID.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        if (userId == 0)
        {
            std::cout << "Возврат в меню администратора.\n";
            return;
        }

        try
        {
            auto user = userRepository_.findById(userId);

            if (!user.has_value())
            {
                std::cout << "Пользователь не найден.\n";
                continue;
            }

            // Запрещаем удалять собственную учётную запись.
            if (user->id == currentUser.id)
            {
                std::cout << "Нельзя удалить собственную учётную запись.\n";
                continue;
            }

            // Проверяем, не является ли пользователь последним администратором.
            if (user->role == UserRole::Admin)
            {
                int adminCount = 0;

                for (const User& existingUser : users)
                {
                    if (existingUser.role == UserRole::Admin)
                    {
                        ++adminCount;
                    }
                }

                if (adminCount <= 1)
                {
                    std::cout << "Нельзя удалить последнего администратора.\n";
                    continue;
                }
            }

            // Показываем информацию о выбранном пользователе.
            std::cout << "\nВыбран пользователь:\n";
            std::cout << "ID: " << user->id << '\n';
            std::cout << "ФИО: " << user->fullName << '\n';
            std::cout << "Логин: " << user->login << '\n';

            std::cout << "Роль: ";

            switch (user->role)
            {
                case UserRole::Student:
                    std::cout << "Студент";
                    break;

                case UserRole::Organizer:
                    std::cout << "Организатор";
                    break;

                case UserRole::Admin:
                    std::cout << "Администратор";
                    break;
            }

            std::cout << "\n\nУдалить этого пользователя? (Д/Н): ";

            std::string confirmation;
            std::getline(std::cin, confirmation);

            if (confirmation != "Д" &&
                confirmation != "д" &&
                confirmation != "Y" &&
                confirmation != "y")
            {
                std::cout << "Удаление отменено.\n";
                continue;
            }

            // Выполняем удаление.
            userRepository_.remove(userId);

            std::cout << "\nПользователь успешно удалён.\n";
        }
        catch (const std::exception& error)
        {
            std::cout << "\nНе удалось удалить пользователя.\n";
            std::cout << "Причина: " << error.what() << '\n';
            std::cout << "Возможно, пользователь связан с мероприятиями "
                         "или регистрациями.\n";
        }
    }
}

// Просмотр мероприятий
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

// Запись на мероприятие
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

// Просмотр своих регистраций
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

// Создание мероприятия
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
        int eventId = eventService_.createEvent(user, event);

        std::cout << "Мероприятие успешно создано!\n";
        std::cout << "ID мероприятия: " << eventId << '\n';
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка создания мероприятия: "
                  << error.what() << '\n';
    }
}

// Завершение мероприятия
void ConsoleUI::completeEvent(const User& user)
{
    int eventId = 0;

    std::cout << "\n=== Завершение мероприятия ===\n";

    showEvents();

    std::cout << "Введите ID мероприятия для завершения: ";
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
        eventService_.completeEvent(user, eventId);
        std::cout << "Мероприятие завершено.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка: " << error.what() << '\n';
    }
}

// Отмена мероприятия
void ConsoleUI::cancelEvent(const User& user)
{
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
        eventService_.cancelEvent(user, eventId);
        std::cout << "Мероприятие отменено.\n";
    }
    catch (const std::exception& error)
    {
        std::cout << "Ошибка: " << error.what() << '\n';
    }
}
