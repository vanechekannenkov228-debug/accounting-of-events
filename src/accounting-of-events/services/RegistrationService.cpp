
#include "RegistrationService.h"

#include <stdexcept>
#include <string>

RegistrationService::RegistrationService(
    Database& database,
    UserRepository& userRepository,
    EventRepository& eventRepository,
    RegistrationRepository& registrationRepository
)
    : database_(database),
      userRepository_(userRepository),
      eventRepository_(eventRepository),
      registrationRepository_(registrationRepository)
{
}

int RegistrationService::registerStudent(int studentId, int eventId)
{
    // Начинаем локальную транзакцию.
    // SAVEPOINT также работает внутри внешней транзакции.
    database_.execute("SAVEPOINT registration_operation;");

    try
    {
        // Получаем блокировку записи до чтения количества регистраций.
        // Для существующего мероприятия UPDATE захватывает write lock SQLite.
        database_.execute(
            "UPDATE Event SET ID = ID WHERE ID = " +
            std::to_string(eventId) + ";"
        );

        // 1. Проверяем существование пользователя
        auto user = userRepository_.findById(studentId);

        if (!user)
        {
            throw std::runtime_error("Student not found");
        }

        // 2. Проверяем роль пользователя
        if (user->role != UserRole::Student)
        {
            throw std::runtime_error(
                "Only students can register for events"
            );
        }

        // 3. Проверяем существование мероприятия
        auto event = eventRepository_.findById(eventId);

        if (!event)
        {
            throw std::runtime_error("Event not found");
        }

        // 4. Проверяем статус мероприятия
        if (event->status != EventStatus::Planned)
        {
            throw std::runtime_error(
                "Event is not available for registration"
            );
        }

        // 5. Проверяем количество активных регистраций
        auto registrations =
            registrationRepository_.findByEventId(eventId);

        int activeRegistrations = 0;

        for (const auto& registration : registrations)
        {
            if (registration.status == RegistrationStatus::Registered)
            {
                ++activeRegistrations;
            }
        }

        if (activeRegistrations >= event->maxParticipants)
        {
            throw std::runtime_error(
                "Event has no available places"
            );
        }

        // 6. Создаём регистрацию
        Registration registration;
        registration.studentId = studentId;
        registration.eventId = eventId;

        int registrationId =
            registrationRepository_.create(registration);

        // Подтверждаем локальную транзакцию
        database_.execute("RELEASE SAVEPOINT registration_operation;");

        return registrationId;
    }
    catch (...)
    {
        // При любой ошибке отменяем изменения операции
        try
        {
            database_.execute(
                "ROLLBACK TO SAVEPOINT registration_operation;"
            );

            database_.execute(
                "RELEASE SAVEPOINT registration_operation;"
            );
        }
        catch (...)
        {
            // Сохраняем исходное исключение
        }

        throw;
    }
}

void RegistrationService::cancelRegistration(int registrationId)
{
    database_.execute("SAVEPOINT cancellation_operation;");

    try
    {
        auto registration =
            registrationRepository_.findById(registrationId);

        if (!registration)
        {
            throw std::runtime_error("Registration not found");
        }

        if (registration->status == RegistrationStatus::Cancelled)
        {
            throw std::runtime_error(
                "Registration is already cancelled"
            );
        }

        registrationRepository_.cancel(registrationId);

        database_.execute("RELEASE SAVEPOINT cancellation_operation;");
    }
    catch (...)
    {
        try
        {
            database_.execute(
                "ROLLBACK TO SAVEPOINT cancellation_operation;"
            );

            database_.execute(
                "RELEASE SAVEPOINT cancellation_operation;"
            );
        }
        catch (...)
        {
        }

        throw;
    }
}

