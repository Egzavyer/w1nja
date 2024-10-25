#include "../include/requestParser.h"

RequestParser::RequestParser()
{
    state = ParseState::BEGIN;
    currRequest;
}

void RequestParser::parseRequest(const std::string &rawRequest)
{
    while (state != ParseState::DONE)
    {
        switch (state)
        {
        case ParseState::BEGIN:
        {
            break;
        }
        case ParseState::METHOD:
        {
            break;
        }
        case ParseState::HEADERS:
        {
            break;
        }

        case ParseState::BODY:
        {
            break;
        }

        case ParseState::DONE:
        {
            std::cout << "Parsing Complete\n";
            break;
        }
        }
    }
}