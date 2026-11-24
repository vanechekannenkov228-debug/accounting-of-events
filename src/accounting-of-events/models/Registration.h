#pragma once

#include <optional>
#include <string>

enum class RegistrationStatus {
    Registered,
    Cancelled
};

enum class AttendanceStatus {
    Attended,
    Absent
};

struct Registration {
    int id{};
    int eventId{};
    int studentId{};

    RegistrationStatus status{
        RegistrationStatus::Registered
    };

    std::optional<AttendanceStatus> attendanceStatus;
};
