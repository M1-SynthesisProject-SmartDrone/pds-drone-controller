#ifndef __APIERROREXCEPTION_H__
#define __APIERROREXCEPTION_H__

#include <iostream>
#include <sstream>
#include <exception>

struct ApiErrorException : public std::exception
{
    std::string type;
    int status;
    std::string detail;

    ApiErrorException(std::string type, int status, std::string detail)
    {
        this->type = type;
        this->detail = detail;
        this->status = status;
    }

    const char* what() const throw()
    {
        return message().c_str();
    }

    const std::string message() const
    {
        std::stringstream ss;
        ss << "Error " << this->status << " of type \"" << this->type
            << "\" : " << this->detail << ".";
        return ss.str();
    }
};

#endif // __APIERROREXCEPTION_H__