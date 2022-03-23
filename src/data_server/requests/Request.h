#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <string>

#include <nlohmann/json.hpp>

struct Request
{
    Request() {};
    ~Request() {};

    virtual std::string serialize() = 0;
};

#endif // __REQUEST_H__