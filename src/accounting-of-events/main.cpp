
#include <iostream>
#include <exception>

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
    try
    {
        // Подключение к базе данных
        Database database("accounting.db");

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

