#pragma once

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
    int ret_value;
}user_account_creation;

void* username_parse(void* threadargs);
void* password_parse(void* threadargs);

constexpr auto username_header = "username::";
constexpr auto password_header = "password::";