 
#pragma once

#include <string>

enum class UserRole
{
    Student,
    Organizer
};

struct User
{
    int id{};
    std::string fullName;
    std::string login;
    std::string passwordHash;
    UserRole role{UserRole::Student};
};
	
