#ifndef __APIHANDLER_H__
#define __APIHANDLER_H__

#include <type_traits>
#include <iostream>
#include <memory>

#include "data_server/responses/Response.h"
#include "data_server/requests/Request.h"


/**
 * This is the main class used to make api calls.
 * This class is designed to make http requests and wait a specific answer.
 *
 * The path send is relative to the root url provided in the constructor
 * 
 * For each call made (get, delete, post, etc.), you must provide the expected object to map
 * response in. If an error is recevied, an instance of ApiErrorException will be thrown
 * 
 * @author Aldric Vitali Silvestre
 */
class ApiHandler
{
private:
    std::string rootUrl;
public:
    ApiHandler(std::string rootUrl);
    ~ApiHandler();

    void get(std::string path, Response& response);

    void post(std::string path, Request& request, Response& response);

    void put(std::string path, Request& request, Response& response);

    void patch(std::string path, Request& request, Response& response);

    void del(std::string path, Response& response);
};

#endif // __APIHANDLER_H__