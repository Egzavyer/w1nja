#include "../include/request.h"

Request::Request()
{
    method = "";
    body = "";
}

std::string Request::getMethod()
{
    return method;
}

void Request::setMethod(const std::string &newMethod)
{
    method = newMethod;
}

std::string Request::getBody()
{
    return body;
}

void Request::setBody(const std::string &newBody)
{
    body = newBody;
}