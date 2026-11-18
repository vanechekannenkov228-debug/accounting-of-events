#include <iostream>
#include <sqlite3.h>
using namespace std; 

int main() {
       sqlite3* db = nullptr; 
       int result = sqlite3_open("accounting-of-events.db", &db);       
       cout << "Hello, world!" << endl;
      if (result != SQLITE_OK) {
       cout << "Ошибка подключения: " << sqlite3_errmsg(db) << endl; 
       sqlite3_close(db); 
       return 1; 
       }
       cout << "Подключение к базе успешно" << endl; 
       sqlite3_close(db); 
    return 0; 
}
