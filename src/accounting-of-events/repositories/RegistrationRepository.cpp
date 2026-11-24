
#include "repositories/RegistrationRepository.h"
#include "database/Statement.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::string registrationStatusToString(
        RegistrationStatus status
    )
    {
        switch (status)
        {
            case RegistrationStatus::Registered:
                return "registered";

            case RegistrationStatus::Cancelled:
                return "cancelled";
        }

        throw std::runtime_error("Unknown registration status");
    }

    RegistrationStatus stringToRegistrationStatus(
        const std::string& status
    )
    {
        if (status == "registered")
            return RegistrationStatus::Registered;

        if (status == "cancelled")
            return RegistrationStatus::Cancelled;

        throw std::runtime_error(
            "Unknown registration status: " + status
        );
    }

    std::string attendanceStatusToString(
        AttendanceStatus status
    )
    {
        switch (status)
        {
            case AttendanceStatus::Attended:
                return "attended";

            case AttendanceStatus::Absent:
                return "absent";
        }

        throw std::runtime_error("Unknown attendance status");
    }

    AttendanceStatus stringToAttendanceStatus(
        const std::string& status
    )
    {
        if (status == "attended")
            return AttendanceStatus::Attended;

        if (status == "absent")
            return AttendanceStatus::Absent;

        throw std::runtime_error(
            "Unknown attendance status: " + status
        );
    }

    Registration readRegistration(Statement& statement)
    {
        Registration registration;

        registration.id = statement.getInt(0);
        registration.eventId = statement.getInt(1);
        registration.studentId = statement.getInt(2);

        registration.status =
            stringToRegistrationStatus(statement.getText(3));

        if (statement.isNull(4))
        {
            registration.attendanceStatus = std::nullopt;
        }
        else
        {
            registration.attendanceStatus =
                stringToAttendanceStatus(statement.getText(4));
        }

        return registration;
    }
}

RegistrationRepository::RegistrationRepository(Database& database)
    : database_(database)
{
}

std::optional<Registration> RegistrationRepository::findById(
    int registrationId
)
{
    const std::string sql =
        "SELECT id, event_id, student_id, "
        "registration_status, attendance_status "
        "FROM Registration "
        "WHERE id = ?;";

    Statement statement(database_, sql);
    statement.bindInt(1, registrationId);

    if (!statement.step())
        return std::nullopt;

    return readRegistration(statement);
}

std::vector<Registration> RegistrationRepository::findAll()
{
    const std::string sql =
        "SELECT id, event_id, student_id, "
        "registration_status, attendance_status "
        "FROM Registration "
        "ORDER BY id;";

    Statement statement(database_, sql);

    std::vector<Registration> registrations;

    while (statement.step())
    {
        registrations.push_back(readRegistration(statement));
    }

    return registrations;
}

std::vector<Registration> RegistrationRepository::findByStudentId(
    int studentId
)
{
    const std::string sql =
        "SELECT id, event_id, student_id, "
        "registration_status, attendance_status "
        "FROM Registration "
        "WHERE student_id = ? "
        "ORDER BY id;";

    Statement statement(database_, sql);
    statement.bindInt(1, studentId);

    std::vector<Registration> registrations;

    while (statement.step())
    {
        registrations.push_back(readRegistration(statement));
    }

    return registrations;
}

std::vector<Registration> RegistrationRepository::findByEventId(
    int eventId
)
{
    const std::string sql =
        "SELECT id, event_id, student_id, "
        "registration_status, attendance_status "
        "FROM Registration "
        "WHERE event_id = ? "
        "ORDER BY id;";

    Statement statement(database_, sql);
    statement.bindInt(1, eventId);

    std::vector<Registration> registrations;

    while (statement.step())
    {
        registrations.push_back(readRegistration(statement));
    }

    return registrations;
}

int RegistrationRepository::create(
    const Registration& registration
)
{
    const std::string sql =
        "INSERT INTO Registration "
        "(event_id, student_id, registration_status, "
        "attendance_status) "
        "VALUES (?, ?, ?, ?);";

    Statement statement(database_, sql);

    statement.bindInt(1, registration.eventId);
    statement.bindInt(2, registration.studentId);

    statement.bindText(
        3,
        registrationStatusToString(registration.status)
    );

    if (registration.attendanceStatus.has_value())
    {
        statement.bindText(
            4,
            attendanceStatusToString(
                registration.attendanceStatus.value()
            )
        );
    }
    else
    {
        statement.bindNull(4);
    }

    statement.execute();

    return static_cast<int>(statement.lastInsertId());
}

void RegistrationRepository::cancel(int registrationId)
{
    const std::string sql =
        "UPDATE Registration "
        "SET registration_status = 'cancelled' "
        "WHERE id = ? AND registration_status = 'registered';";

    Statement statement(database_, sql);

    statement.bindInt(1, registrationId);
    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error(
            "Registration not found or already cancelled"
        );
    }
}

void RegistrationRepository::setAttendance(
    int registrationId,
    AttendanceStatus attendanceStatus
)
{
    const std::string sql =
        "UPDATE Registration "
        "SET attendance_status = ? "
        "WHERE id = ? AND registration_status = 'registered';";

    Statement statement(database_, sql);

    statement.bindText(
        1,
        attendanceStatusToString(attendanceStatus)
    );

    statement.bindInt(2, registrationId);
    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error(
            "Active registration not found"
        );
    }
}

void RegistrationRepository::clearAttendance(int registrationId)
{
    const std::string sql =
        "UPDATE Registration "
        "SET attendance_status = NULL "
        "WHERE id = ? AND registration_status = 'registered';";

    Statement statement(database_, sql);

    statement.bindInt(1, registrationId);
    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error(
            "Active registration not found"
        );
    }
}

