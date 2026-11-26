#include "services/AuthenticationService.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
    constexpr int SALT_SIZE = 16;
    constexpr int HASH_SIZE = 32;
    constexpr int ITERATIONS = 600000;

    // Удаление пробелов в начале и конце строки.
    std::string trim(const std::string& value)
    {
        std::size_t first = 0;
        std::size_t last = value.size();

        while (first < last &&
               std::isspace(static_cast<unsigned char>(value[first])))
        {
            ++first;
        }

        while (last > first &&
               std::isspace(static_cast<unsigned char>(value[last - 1])))
        {
            --last;
        }

        return value.substr(first, last - first);
    }

    std::string toHex(const unsigned char* data, std::size_t size)
    {
        std::ostringstream result;
        result << std::hex << std::setfill('0');

        for (std::size_t i = 0; i < size; ++i)
            result << std::setw(2) << static_cast<int>(data[i]);

        return result.str();
    }

    std::vector<unsigned char> fromHex(const std::string& hex)
    {
        if (hex.empty() || hex.size() % 2 != 0)
            throw std::runtime_error("Invalid password hash format");

        std::vector<unsigned char> result;
        result.reserve(hex.size() / 2);

        for (std::size_t i = 0; i < hex.size(); i += 2)
        {
            unsigned int value = 0;
            std::istringstream input(hex.substr(i, 2));
            input >> std::hex >> value;

            if (input.fail() || value > 255)
                throw std::runtime_error("Invalid password hash format");

            result.push_back(static_cast<unsigned char>(value));
        }

        return result;
    }

    std::vector<unsigned char> deriveKey(
        const std::string& password,
        const unsigned char* salt,
        int saltSize,
        int iterations
    )
    {
        std::vector<unsigned char> hash(HASH_SIZE);

        if (PKCS5_PBKDF2_HMAC(
                password.c_str(),
                static_cast<int>(password.size()),
                salt,
                saltSize,
                iterations,
                EVP_sha256(),
                HASH_SIZE,
                hash.data()
            ) != 1)
        {
            throw std::runtime_error("Password hashing failed");
        }

        return hash;
    }
}

AuthenticationService::AuthenticationService(
    UserRepository& userRepository
)
    : userRepository_(userRepository)
{
}

std::string AuthenticationService::hashPassword(
    const std::string& password
)
{
    std::array<unsigned char, SALT_SIZE> salt{};

    if (RAND_bytes(salt.data(), SALT_SIZE) != 1)
        throw std::runtime_error("Failed to generate password salt");

    auto hash = deriveKey(
        password,
        salt.data(),
        SALT_SIZE,
        ITERATIONS
    );

    return "pbkdf2-sha256$" +
           std::to_string(ITERATIONS) + "$" +
           toHex(salt.data(), salt.size()) + "$" +
           toHex(hash.data(), hash.size());
}

bool AuthenticationService::verifyPassword(
    const std::string& password,
    const std::string& storedHash
)
{
    try
    {
        const std::string prefix = "pbkdf2-sha256$";

        if (storedHash.rfind(prefix, 0) != 0)
            return false;

        std::size_t firstSeparator = storedHash.find('$', prefix.size());

        if (firstSeparator == std::string::npos)
            return false;

        std::size_t secondSeparator =
            storedHash.find('$', firstSeparator + 1);

        if (secondSeparator == std::string::npos)
            return false;

        int iterations = std::stoi(
            storedHash.substr(prefix.size(),
                              firstSeparator - prefix.size())
        );

        if (iterations < 100000 || iterations > 2000000)
            return false;

        auto salt = fromHex(
            storedHash.substr(
                firstSeparator + 1,
                secondSeparator - firstSeparator - 1
            )
        );

        auto expectedHash = fromHex(
            storedHash.substr(secondSeparator + 1)
        );

        if (salt.size() != SALT_SIZE ||
            expectedHash.size() != HASH_SIZE)
            return false;

        auto actualHash = deriveKey(
            password,
            salt.data(),
            static_cast<int>(salt.size()),
            iterations
        );

        return CRYPTO_memcmp(
            actualHash.data(),
            expectedHash.data(),
            HASH_SIZE
        ) == 0;
    }
    catch (...)
    {
        return false;
    }
}

// Обычная регистрация: только студент.
int AuthenticationService::registerUser(
    const std::string& fullName,
    const std::string& login,
    const std::string& password
)
{
    const std::string cleanFullName = trim(fullName);
    const std::string cleanLogin = trim(login);

    if (cleanFullName.empty())
        throw std::runtime_error("Full name cannot be empty");

    if (cleanLogin.empty())
        throw std::runtime_error("Login cannot be empty");

    if (password.size() < 8)
        throw std::runtime_error("Password must contain at least 8 characters");

    if (userRepository_.findByLogin(cleanLogin))
        throw std::runtime_error("Login is already taken");

    User user;
    user.fullName = cleanFullName;
    user.login = cleanLogin;
    user.passwordHash = hashPassword(password);
    user.role = UserRole::Student;

    return userRepository_.create(user);
}

// Первоначальное создание администратора.
int AuthenticationService::registerAdmin(
    const std::string& fullName,
    const std::string& login,
    const std::string& password
)
{
    const std::string cleanFullName = trim(fullName);
    const std::string cleanLogin = trim(login);

    if (cleanFullName.empty())
        throw std::runtime_error("Full name cannot be empty");

    if (cleanLogin.empty())
        throw std::runtime_error("Login cannot be empty");

    if (password.size() < 8)
        throw std::runtime_error("Password must contain at least 8 characters");

    if (userRepository_.findByLogin(cleanLogin))
        throw std::runtime_error("Login is already taken");

    const auto users = userRepository_.findAll();

    for (const User& existingUser : users)
    {
        if (existingUser.role == UserRole::Admin)
        {
            throw std::runtime_error(
                "Administrator already exists"
            );
        }
    }

    User user;
    user.fullName = cleanFullName;
    user.login = cleanLogin;
    user.passwordHash = hashPassword(password);
    user.role = UserRole::Admin;

    return userRepository_.create(user);
}

// Авторизация.
User AuthenticationService::login(
    const std::string& loginValue,
    const std::string& password
)
{
    const std::string cleanLogin = trim(loginValue);

    if (cleanLogin.empty() || password.empty())
        throw std::runtime_error("Login and password are required");

    auto user = userRepository_.findByLogin(cleanLogin);

    if (!user || !verifyPassword(password, user->passwordHash))
        throw std::runtime_error("Invalid login or password");

    return *user;
}

