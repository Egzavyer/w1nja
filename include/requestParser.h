#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "request.h"

#include <string>
#include <iostream>

enum class ParseState
{
    BEGIN,
    METHOD,
    HEADERS,
    BODY,
    DONE
};

class RequestParser
{
private:
    ParseState state;
    Request currRequest;

public:
    RequestParser();
    void parseRequest(const std::string &rawRequest);
};

#endif /* REQUEST_PARSER_H */