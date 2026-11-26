cmake_minimum_required(VERSION 3.16)

project(AccountingOfEvents)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(AccountingOfEvents
    src/accounting-of-events/main.cpp
    src/accounting-of-events/database/Database.cpp
    src/accounting-of-events/repositories/UserRepository.cpp
    src/accounting-of-events/repositories/EventRepository.cpp
    src/accounting-of-events/repositories/EventTypeRepository.cpp
    src/accounting-of-events/repositories/RegistrationRepository.cpp
    src/accounting-of-events/services/RegistrationService.cpp
    src/accounting-of-events/services/EventService.cpp
    src/accounting-of-events/services/AuthenticationService.cpp
    src/accounting-of-events/ui/ConsoleUI.cpp
    src/accounting-of-events/exporters/DataExporter.cpp
)

target_include_directories(AccountingOfEvents PRIVATE
    src/accounting-of-events
)

target_link_libraries(AccountingOfEvents PRIVATE
    SQLite3::SQLite3
    OpenSSL::Crypto
)

# Путь к базе данных 
set(ACCOUNTING_DATABASE_PATH
    "${CMAKE_SOURCE_DIR}/database/accounting.db"
    CACHE FILEPATH "Path to SQLite database"
)

target_compile_definitions(AccountingOfEvents PRIVATE
    ACCOUNTING_DATABASE_PATH="${ACCOUNTING_DATABASE_PATH}"
)

