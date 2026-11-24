
#pragma once

#include "database/Database.h"
#include "models/User.h"

#include <optional>
#include <string>
#include <vector>

class UserRepository
{
public:
    explicit UserRepository(Database& database);

    // Поиск пользователя
    std::optional<User> findById(int userId);

    std::optional<User> findByLogin(
        const std::string& login
    );

    // Получение всех пользователей
    std::vector<User> findAll();

    // CRUD
    int create(const User& user);

    void update(const User& user);

    void remove(int userId);

private:
    Database& database_;
};

