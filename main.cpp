#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>

#include "sha512.h" //Header file for the SHA512 encoding function
#include "sha256.h" //Header file for the SHA256 encoding function

using std::cout;
using std::endl;
using std::cin;

using THREADFUNCPTR = void* (*)(void*);

typedef struct login_username_info
{
    std::string username;
    std::string return_string;
    int username_ret;
}login_username_info;

typedef struct login_password_info
{
    std::string password;
    std::string encoded_password;
    std::string return_string;
    int password_ret;
}login_pass_info;

typedef struct user_account_creation
{
    std::string username;
    std::string password;
    std::string encoded_password;
    std::string user_information;
    int ret_value;
}user_account_creation;

void* username_parse(void* threadargs);
void* password_parse(void* threadargs);
void* create_account(void* threadargs);

const auto username_header = "username::";
const auto password_header = "password::";
const auto name_header = "name::";
const auto display_name_header = "display::";

int
main()
{
    pthread_t threads[2];
    pthread_t acc_creation_thread;

    struct login_username_info username_data;
    struct login_password_info password_data;
    struct user_account_creation account_data;

    struct login_username_info* username_data_ptr = &username_data;
    struct login_password_info* password_data_ptr = &password_data;
    struct user_account_creation* account_data_ptr = &account_data;
    bool chosen_choice = true;

    while (chosen_choice)
    {

        std::string user_input;
        cout << "Welcome" << endl;
        cout << "Do you want to create an account (C) or login (L)?" << endl << "Or \"E\" to exit" << endl;
        cin >> user_input;
        
        if (user_input == "C")
        {
            std::string user_info, user_info_temp;
            cout << "What is your name" << endl;
            cin >> user_info_temp;
            user_info += name_header + user_info_temp + "\n";
            
            cout << "What do you want your display name to be" << endl;
            cin >> user_info_temp;
            user_info += display_name_header + user_info_temp + "\n";

            account_data_ptr->user_information = user_info;

            cout << "What do you want your username name to be" << endl;
            cin >> account_data_ptr->username;
            account_data_ptr->username += "\n";

            cout << "What do you want your password to be" << endl;
            cin >> user_info_temp;

            cout << "Reenter your password confirm" << endl;
            cin >> user_info;
            user_info += "\n";

            if (user_info == user_info_temp)
            {
                account_data_ptr->password = user_info;
            }

            pthread_create(&acc_creation_thread, nullptr, (THREADFUNCPTR)create_account, (void*)account_data_ptr);
            cout << "Hello" << endl;
            pthread_join(acc_creation_thread, nullptr);
            switch (account_data_ptr->ret_value) {
                case -1:
                    cout << "Your account could no be created" << endl;
                    break;
                case 1:
                    cout << "Your account was created successfully" << endl;
                    break;
                default:
                    cout << "An unhandled error has occurred" << endl;
                    break;
            }
            cout << "Account created" << endl;
        }

        else if (user_input == "L")
        {   
            int username_return_val, password_return_val;
            cout << "What is your username" << endl;
            cin >> username_data_ptr->username;
            cout << "What is your password" << endl;
            cin >> password_data_ptr->password;

            pthread_create(&threads[0], nullptr, (THREADFUNCPTR)username_parse, (void*)&username_data_ptr->username);
            pthread_create(&threads[1], nullptr, (THREADFUNCPTR)password_parse, (void*)&password_data_ptr->password);
            pthread_join(threads[0], nullptr);
            pthread_join(threads[1], nullptr);

            cout << username_data_ptr->return_string << endl << password_data_ptr->return_string << endl;
        }

        else if (user_input == "E")
        {
            chosen_choice = false;
            break;
        }

        else
        {
            cout << "That command is not recognized, please try again";
        }
    }
    return 0;
}

void*
create_account(void* threadargs)
{
    std::ofstream user_data;
    auto* data = static_cast<struct user_account_creation*>(threadargs);

    user_data.open("shadow.txt", std::ios::app);
    if (!user_data.is_open())
    {
        data->ret_value = -1;
        return nullptr;
    }

    data->encoded_password = sha512(data->password);

    std::string username_file_string = username_header + data->username;
    std::string password_file_string = password_header + data->encoded_password;

    user_data << "\n";

    user_data << data->user_information;
    user_data << username_file_string;
    user_data << password_file_string;

    data->ret_value = 1;
    return nullptr;
}

void*
username_parse(void* threadargs)
{
    std::ifstream user_data;
    std::string username_from_file, temp_username_from_file;

    auto* data = static_cast<struct login_username_info*>(threadargs);
    std::string username = data->username;

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        data->return_string = "The file containing your password could not be opened";
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
            data->return_string = "Your password was found";
            data->username_ret = 1;
            return nullptr;
        }
    }

    user_data.close();
    data->return_string = "Your password was not found in the file";
    data->username_ret = 0;
    return nullptr;
}

void*
password_parse(void* threadargs)
{

    std::ifstream user_data;
    std::string password_from_file, temp_password_from_file;

    auto* data = static_cast<struct login_password_info*>(threadargs);
    std::string password = data->password;

    data->encoded_password = sha512(password);

    user_data.open("shadow.txt", std::ios::app);

    if (!user_data.is_open())
    {
        data->return_string = "The file containing your password could not be opened";
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
            data->return_string = "Your password was found";
            data->password_ret = 1;
            return nullptr;
        }
        
    }

    user_data.close();
    data->return_string = "Your password was not found in the file";
    data->password_ret = 0;
    return nullptr;
}