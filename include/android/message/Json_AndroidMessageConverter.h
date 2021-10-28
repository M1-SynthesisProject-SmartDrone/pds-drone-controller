#ifndef __JSON_ANDROIDMESSAGECONVERTER_H__
#define __JSON_ANDROIDMESSAGECONVERTER_H__

/**
 * An implementation of the Abstract_AndroidMessageConverter class
 * that can convert JSON formatted strings
 * 
 * @author Aldric Vitali Silvestre
 */
#include "Abstract_AndroidMessageConverter.h"

class Json_AndroidMessageConverter : public Abstract_AndroidMessageConverter
{
public:
    Json_AndroidMessageConverter();
    ~Json_AndroidMessageConverter();

    AndroidMessageReceived convertMessageReceived(std::string message);
private:
    /* data */
};


#endif // __JSON_ANDROIDMESSAGECONVERTER_H__