
#pragma once

#include "database/Database.h"
#include "models/Registration.h"

#include <vector>
#include <optional>

class RegistrationRepository
{
public:
    explicit RegistrationRepository(Database& database);

    // Поиск регистраций
    std::optional<Registration> findById(int registrationId);

    std::vector<Registration> findAll();

    std::vector<Registration> findByStudentId(int studentId);

    std::vector<Registration> findByEventId(int eventId);

    // Создание регистрации
    int create(const Registration& registration);

    // Отмена регистрации
    void cancel(int registrationId);

    // Учёт посещаемости
    void setAttendance(
        int registrationId,
        AttendanceStatus attendanceStatus
    );

    // Сброс отметки посещаемости
    void clearAttendance(int registrationId);

private:
    Database& database_;
};

