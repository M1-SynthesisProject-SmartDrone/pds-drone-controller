#ifndef __ERRORRESPONSE_H__
#define __ERRORRESPONSE_H__

#include <string>

#include "Response.h"

/**
 * Whenever an error occurs (http status 4XX-5XX), this message will be sent
 * 
 * @author Aldric Vitali Silvestre
 */
struct ErrorResponse : public Response
{
    std::string type;
    int status;
    std::string detail;

    void deserializeJson(nlohmann::json json)
    {
        type = json["type"];
        status = json["status"];
        detail = json["detail"];
    }
};


#endif // __ERRORRESPONSE_H__