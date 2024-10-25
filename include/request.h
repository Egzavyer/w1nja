#ifndef REQUEST_H
#define REQUEST_H

#include <string>

class Request
{
private:
    std::string method;
    std::string body;

public:
    Request();
    std::string getMethod();
    void setMethod(const std::string &newMethod);
    std::string getBody();
    void setBody(const std::string &newBody);
};

#endif /* REQUEST_H */