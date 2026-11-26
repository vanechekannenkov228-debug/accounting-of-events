
#pragma once

#include "repositories/UserRepository.h"

#include <string>

class AuthenticationService
{
public:
    explicit AuthenticationService(UserRepository& userRepository);

    // Регистрация обычного пользователя
    int registerUser(
        const std::string& fullName,
        const std::string& login,
        const std::string& password
    );

    // Первоначальное создание администратора
    int registerAdmin(
        const std::string& fullName,
        const std::string& login,
        const std::string& password
    );

    // Авторизация
    User login(
        const std::string& login,
        const std::string& password
    );

private:
    UserRepository& userRepository_;

    std::string hashPassword(const std::string& password);

    bool verifyPassword(
        const std::string& password,
        const std::string& storedHash
    );
};

