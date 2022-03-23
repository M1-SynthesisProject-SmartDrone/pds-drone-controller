#include "ApiHandler.h"

#include <cpr/cpr.h>

#include "ApiErrorException.h"
#include "data_server/responses/ErrorResponse.hpp"

using namespace std;

ApiHandler::ApiHandler(string rootUrl)
{
    // Just remove the last '/' if exists
    if (rootUrl.back() == '/')
    {
        this->rootUrl = rootUrl.substr(0, rootUrl.size()-1);
    }
    else
    {
        this->rootUrl = rootUrl;
    }
}

ApiHandler::~ApiHandler() {}

string formatPath(string path)
{
    if (path[0] != '/')
    {
        return '/' + path;
    }
    return path;
}

void handleResponse(cpr::Response& apiResponse, Response& response)
{
    // Check if we have an error status
    long statusCode = apiResponse.status_code;
    if (statusCode >= 400)
    {
        try
        {
            // Try to get the error message
            ErrorResponse errorResponse;
            errorResponse.deserialize(apiResponse.text);
            throw ApiErrorException(errorResponse.type, errorResponse.status, errorResponse.detail);
        }
        catch (ApiErrorException& e)
        {
            throw e;
        }
        catch (exception& e)
        {
            // Couldn't even read the error message
            throw ApiErrorException("unknown", 500, "Cannot read the error message");
        }
    }
    else
    {
        response.deserialize(apiResponse.text);
    }
}

void ApiHandler::get(std::string path, Response& response)
{
    path = formatPath(path);
    auto apiResponse = cpr::Get(cpr::Url(rootUrl + path));
    handleResponse(apiResponse, response);
}

void ApiHandler::post(std::string path, Request& request, Response& response)
{
    path = formatPath(path);
    auto apiResponse = cpr::Post(
        cpr::Url(rootUrl + path),
        cpr::Header{ {"Content-Type", "application/json; charset=UTF-8"} },
        cpr::Body(request.serialize())
    );
    handleResponse(apiResponse, response);
}

void ApiHandler::put(std::string path, Request& request, Response& response)
{
    path = formatPath(path);
    auto apiResponse = cpr::Put(
        cpr::Url(rootUrl + path),
        cpr::Header{ {"Content-Type", "application/json; charset=UTF-8"} },
        cpr::Body(request.serialize())
    );
    handleResponse(apiResponse, response);
}

void ApiHandler::patch(std::string path, Request& request, Response& response)
{
    path = formatPath(path);
    auto apiResponse = cpr::Patch(
        cpr::Url(rootUrl + path),
        cpr::Header{ {"Content-Type", "application/json; charset=UTF-8"} },
        cpr::Body(request.serialize())
    );
    handleResponse(apiResponse, response);
}

void ApiHandler::del(std::string path, Response& response)
{
    path = formatPath(path);
    auto apiResponse = cpr::Delete(cpr::Url(rootUrl + path));
    handleResponse(apiResponse, response);
}

