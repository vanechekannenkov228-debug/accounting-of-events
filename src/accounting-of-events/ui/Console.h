#pragma once

#include "services/AuthenticationService.h"
#include "services/RegistrationService.h"
#include "services/EventService.h"

#include "repositories/UserRepository.h"
#include "repositories/EventRepository.h"
#include "repositories/RegistrationRepository.h"

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
    // Сервисы
    AuthenticationService& authenticationService_;
    RegistrationService& registrationService_;
    EventService& eventService_;

    // Репозитории
    UserRepository& userRepository_;
    EventRepository& eventRepository_;
    RegistrationRepository& registrationRepository_;

    // Главное меню
    void showMainMenu();

    // Меню пользователей
    void showStudentMenu(const User& user);
    void showOrganizerMenu(const User& user);
    void showAdminMenu(const User& user);

    // Регистрация и авторизация
    void registerUser();
    void loginUser();

    // Первоначальная настройка администратора
    void setupAdmin();
    bool hasAdmin();

    // Управление пользователями
    void showUsers();
    void changeUserRole(const User& user);

    // Работа с мероприятиями
    void showEvents();
    void registerForEvent(const User& user);
    void showMyRegistrations(const User& user);

    void createEvent(const User& user);
    void completeEvent(const User& user);
    void cancelEvent(const User& user);
};

