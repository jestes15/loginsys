#include <iostream>
#include <string>
#include <fstream>

#include "sha512.h"

using std::cout;
using std::endl;
using std::cin;

int username_parse(std::string);
int password_parse(std::string);

constexpr auto username_header = "username::";
constexpr auto password_header = "password::";
//Next challenge, allow the user to create an account, meaing writing the username and password to the shadow file
int main()
{
    std::string username, password, encoded_password;

    int username_return_val, password_return_val;
    cout << "What is your username" << endl;
    cin >> username;
    cout << "What is your password" << endl;
    cin >> password;

    encoded_password = sha512(password);

    username_return_val = username_parse(username);
    password_return_val = password_parse(encoded_password);

    switch (username_return_val) {
        case -1:
            cout << "There was an error" << endl;
            break;
        case 0:
            cout << "The operation has finished" << endl;
            break;
        case 1:
            cout << "Your username was found" << endl;
            break;
        default:
            cout << "An uncaught error has occured" << endl;
    }
     switch (password_return_val) {
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

int username_parse(std::string username)
{
    std::ifstream user_data;
    std::string username_from_file, temp_username_from_file;

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        cout << "The login file could no be opened" << endl;
        return -1;
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
            return 1;
        }
    }

    user_data.close();
    return 0;
}

int password_parse(std::string password) {

    std::ifstream user_data;
    std::string password_from_file, temp_password_from_file;

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        cout << "The login file could no be opened" << endl;
        return -1;
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

        if (password_from_file == password)
        {
            user_data.close();
            return 1;
        }
    }

    user_data.close();
    return 0;
}