#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>

#include "sha512.h"

using std::cout;
using std::endl;
using std::cin;

using THREADFUNCPTR = void* (*)(void*);

typedef struct login_username_info
{
    std::string username;
    int username_ret;
}login_username_info;

typedef struct login_password_info
{
    std::string password;
    std::string encoded_password;
    int password_ret;
}login_pass_info;

void* username_parse(void* threadargs);
void* password_parse(void* threadargs);

constexpr auto username_header = "username::";
constexpr auto password_header = "password::";

int main()
{
    pthread_t threads[2];

    struct login_username_info username_data;
    struct login_password_info password_data;

    struct login_username_info* username_data_ptr = &username_data;
    struct login_password_info* password_data_ptr = &password_data;

    int username_return_val, password_return_val;
    cout << "What is your username" << endl;
    cin >> username_data_ptr->username;
    cout << "What is your password" << endl;
    cin >> password_data_ptr->password;

    pthread_create(&threads[0], nullptr, (THREADFUNCPTR)username_parse, (void*)&username_data_ptr->username);
    pthread_create(&threads[1], nullptr, (THREADFUNCPTR)password_parse, (void*)&password_data_ptr->password);
    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);

    switch (username_data_ptr->username_ret) {
        case 0:
            cout << "The operation has finished" << endl;
            break;
        case 1:
            cout << "Your username was found" << endl;
            break;
        case 2:
            cout << "There was an error" << endl;
            break;
        default:
            cout << "An uncaught error has occured" << endl;
    }
     switch (password_data_ptr->password_ret) {
        case -1:
            cout << "There was an error" << endl;
            break;
        case 0:
            cout << "The operation has finished" << endl;
            break;
        case 1:
            cout << "Your password was found" << endl;
            break;
        default:
            cout << "An uncaught error has occured" << endl;
    }
    return 0;
}

void* username_parse(void* threadargs)
{
    std::ifstream user_data;
    std::string username_from_file, temp_username_from_file;

    auto* data = static_cast<struct login_username_info*>(threadargs);
    std::string username = data->username;

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        cout << "The login file could no be opened" << endl;
        data->username_ret = 2;
        return nullptr;
    }
    while (!user_data.eof()) 
    {
        user_data >> username_from_file;
        temp_username_from_file = username_from_file;
        int size_of_string = username_from_file.size();

        if (temp_username_from_file.erase(10, size_of_string) == password_header) {
            continue;
        }

        username_from_file = username_from_file.erase(0, 10);

        if (username_from_file == username)
        {
            user_data.close();
            data->username_ret = 1;
            return nullptr;
        }
    }

    user_data.close();
    data->username_ret = 0;
    return nullptr;
}

void* password_parse(void* threadargs)
{

    std::ifstream user_data;
    std::string password_from_file, temp_password_from_file;

    auto* data = static_cast<struct login_password_info*>(threadargs);
    std::string password = data->password;

    data->encoded_password = sha512(password);

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        cout << "The login file could no be opened" << endl;
        data->password_ret = 2;
        return nullptr;
    }
    while (!user_data.eof())
    {
        user_data >> password_from_file;
        temp_password_from_file = password_from_file;
        int size_of_string = password_from_file.size();

        if (temp_password_from_file.erase(10, size_of_string) == username_header) {
            continue;
        }

        password_from_file = password_from_file.erase(0, 10);

        
        if (password_from_file == data->encoded_password)
        {
            user_data.close();
            data->password_ret = 1;
            return nullptr;
        }
        
    }

    user_data.close();
    data->password_ret = 0;
    return nullptr;
}