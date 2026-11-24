
#include "repositories/UserRepository.h"
#include "database/Statement.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::string roleToString(UserRole role)
    {
        switch (role)
        {
            case UserRole::Student:
                return "Student";

            case UserRole::Organizer:
                return "Organizer";
        }

        throw std::runtime_error("Unknown user role");
    }

    UserRole stringToRole(const std::string& role)
    {
        if (role == "Student")
        {
            return UserRole::Student;
        }

        if (role == "Organizer")
        {
            return UserRole::Organizer;
        }

        throw std::runtime_error(
            "Unknown user role: " + role
        );
    }

    User readUser(Statement& statement)
    {
        User user;

        user.id = statement.getInt(0);
        user.fullName = statement.getText(1);
        user.login = statement.getText(2);
        user.passwordHash = statement.getText(3);
        user.role = stringToRole(statement.getText(4));

        return user;
    }
}

UserRepository::UserRepository(Database& database)
    : database_(database)
{
}

// Поиск по ID
std::optional<User> UserRepository::findById(int userId)
{
    const std::string sql =
        "SELECT id, full_name, login, password_hash, role "
        "FROM User WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, userId);

    if (!statement.step())
    {
        return std::nullopt;
    }

    return readUser(statement);
}

// Поиск по логину
std::optional<User> UserRepository::findByLogin(
    const std::string& login
)
{
    const std::string sql =
        "SELECT id, full_name, login, password_hash, role "
        "FROM User WHERE login = ?;";

    Statement statement(database_, sql);

    statement.bindText(1, login);

    if (!statement.step())
    {
        return std::nullopt;
    }

    return readUser(statement);
}

// Получение всех пользователей
std::vector<User> UserRepository::findAll()
{
    const std::string sql =
        "SELECT id, full_name, login, password_hash, role "
        "FROM User ORDER BY id;";

    Statement statement(database_, sql);

    std::vector<User> users;

    while (statement.step())
    {
        users.push_back(readUser(statement));
    }

    return users;
}

// Создание пользователя
int UserRepository::create(const User& user)
{
    const std::string sql =
        "INSERT INTO User "
        "(full_name, login, password_hash, role) "
        "VALUES (?, ?, ?, ?);";

    Statement statement(database_, sql);

    statement.bindText(1, user.fullName);
    statement.bindText(2, user.login);
    statement.bindText(3, user.passwordHash);
    statement.bindText(4, roleToString(user.role));

    statement.execute();

    return static_cast<int>(statement.lastInsertId());
}

// Обновление пользователя
void UserRepository::update(const User& user)
{
    const std::string sql =
        "UPDATE User "
        "SET full_name = ?, "
        "login = ?, "
        "password_hash = ?, "
        "role = ? "
        "WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindText(1, user.fullName);
    statement.bindText(2, user.login);
    statement.bindText(3, user.passwordHash);
    statement.bindText(4, roleToString(user.role));
    statement.bindInt(5, user.id);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error(
            "User not found or no changes made"
        );
    }
}

// Удаление пользователя
void UserRepository::remove(int userId)
{
    const std::string sql =
        "DELETE FROM User WHERE id = ?;";

    Statement statement(database_, sql);

    statement.bindInt(1, userId);

    statement.execute();

    if (statement.changes() == 0)
    {
        throw std::runtime_error("User not found");
    }
}

