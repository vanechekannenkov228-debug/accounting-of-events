#include <iostream>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#endif

#include "database/Database.h"

#include "repositories/UserRepository.h"
#include "repositories/EventRepository.h"
#include "repositories/EventTypeRepository.h"
#include "repositories/RegistrationRepository.h"

#include "services/AuthenticationService.h"
#include "services/RegistrationService.h"
#include "services/EventService.h"

#include "ui/ConsoleUI.h"

#include "exporters/DataExporter.h"

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try
    {
        // Подключение к постоянной базе данных проекта
        Database database(ACCOUNTING_DATABASE_PATH);

        // Экспорт данных
        DataExporter dataExporter(database);
        dataExporter.exportEvents();

        // Создание репозиториев
        UserRepository userRepository(database);
        EventRepository eventRepository(database);
        EventTypeRepository eventTypeRepository(database);
        RegistrationRepository registrationRepository(database);

        // Создание сервисов
        AuthenticationService authenticationService(userRepository);

        RegistrationService registrationService(
            database,
            userRepository,
            eventRepository,
            registrationRepository
        );

        EventService eventService(
            userRepository,
            eventRepository,
            eventTypeRepository,
            registrationRepository
        );

        // Создание пользовательского интерфейса
        ConsoleUI ui(
            authenticationService,
            registrationService,
            eventService,
            userRepository,
            eventRepository,
            registrationRepository
        );

        // Запуск приложения
        ui.run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "\nКритическая ошибка: "
                  << error.what() << '\n';

        return 1;
    }

    return 0;
}

