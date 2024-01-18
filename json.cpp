//
// Created by Tanguy on 23/12/2023.
//

#include <iostream>
#include "json.h"

long long json::handle_long_long(token &tok) {
    return std::stoll(get<std::string>(tok.literal));
}
double json::handle_double(token &tok) {
    return std::stod(get<std::string>(tok.literal));
}
std::string json::handle_string(token &tok) {
    return get<std::string>(tok.literal);
}
bool json::handle_bool(token &tok) {
    return get<std::string>(tok.literal) == "true";
}

void json::handle_array(void *v, const std::string &name, serializable *object) {
    token tok;
    receive_token(tok);

    std::variant<std::vector<std::string> *, std::vector<double> *, std::vector<long long> *,
            std::vector<bool> *, std::vector<void *> *, std::vector<serializable *> *> vector;

    while (tok.type == token_type::null && tok.type != token_type::eof) {
        receive_token(tok);
        if (tok.type == token_type::endarray)
            return;
        if (tok.type != token_type::valueseparator)
            throw std::invalid_argument("Values in an array must be separated by a value separator");
        receive_token(tok);
    }

    switch (tok.type) {
        case token_type::string:
            vector = (std::vector<std::string> *) v;
            break;
        case token_type::numberdouble:
            vector = (std::vector<double> *) v;
            break;
        case token_type::numberlong:
            vector = (std::vector<long long> *) v;
            break;
        case token_type::boolean:
            vector = (std::vector<bool> *) v;
            break;
        case token_type::beginobject:
            vector = (std::vector<serializable *> *) v;
            break;
        case token_type::beginarray:
            vector = (std::vector<void *> *) v;
            break;
        case token_type::endarray:
            return;
        default :
            throw std::invalid_argument(
                    "json not compliant with rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
    }

    while (tok.type !=
           token_type::endarray) {  //Todo check if get in loops has performance overhead, and if so, use std::visit
        if (tok.type == token_type::eof)
            throw std::invalid_argument(
                    "json not compliant with rfc 8259. An array must be closed with an end array symbol");

        switch (tok.type) {
            case token_type::numberdouble: {
                get<std::vector<double> *>(vector)->push_back(handle_double(tok));
                break;
            }
            case token_type::numberlong: {
                get<std::vector<long long> *>(vector)->push_back(handle_long_long(tok));
                break;
            }
            case token_type::string: {
                get<std::vector<std::string> *>(vector)->push_back(handle_string(tok));
                break;
            }
            case token_type::boolean: {
                get<std::vector<bool> *>(vector)->push_back(handle_bool(tok));
                break;
            }
            case token_type::null:
                break;
            case token_type::beginobject: {
                std::map<std::string, void (*)(serializable **)> &map_constructors = object->get_constructors();
                auto constructor = map_constructors.find(name);
                if (constructor != map_constructors.end()) {
                    serializable *obj = nullptr;
                    create_obj((serializable **) &obj, constructor->second);
                    get<std::vector<serializable *>*>(vector)->push_back(obj);
                }
                break;
            }
            case token_type::beginarray: {

                break;
            }
            default:
                throw std::invalid_argument(
                        "json not compliant with rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
        }
        receive_token(tok);
        if (tok.type == token_type::endarray)
            break;
        if (tok.type != token_type::valueseparator)
            throw std::invalid_argument("Values in an array must be separated by a value separator");
        receive_token(tok);
    }
}

void json::handle_member(serializable *object) {
    token tok;
    receive_token(tok);
    if (tok.type == token_type::endobject)
        return;
    if (tok.type != token_type::string)
        throw std::invalid_argument("json not compliant with rfc 8259. A member must start start with a string");

    std::string name = get<std::string>(tok.literal);

    receive_token(tok);
    if (tok.type != token_type::nameseparator)
        throw std::invalid_argument("json not compliant with rfc 8259. Member's name and value must be separate by a name separator");

    std::map<std::string, void *> &map_attributes = object->get_attributes();

    auto obj_variable = map_attributes.find(name);
    if (obj_variable != map_attributes.end()) {
        receive_token(tok);
        switch (tok.type) {
            case token_type::numberdouble: {
                *(double *) obj_variable->second = handle_double(tok);
                break;
            }
            case token_type::numberlong: {
                *(long long *) obj_variable->second = handle_long_long(tok);
                break;
            }
            case token_type::string: {
                *(std::string *) obj_variable->second = handle_string(tok);
                break;
            }
            case token_type::boolean: {
                *(bool *) obj_variable->second = handle_bool(tok);
                break;
            }
            case token_type::null:
                break;
            case token_type::beginobject: {
                std::map<std::string, void (*)(serializable **)> &map_constructors = object->get_constructors();
                auto constructor = map_constructors.find(name);
                if (constructor != map_constructors.end())
                    create_obj((serializable **) obj_variable->second, constructor->second);
                break;
            }
            case token_type::beginarray: {
                handle_array(obj_variable->second, name, object);
                break;
            }
            default:
                throw std::invalid_argument("json not compliant with rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
        }
    }

    receive_token(tok);
    switch (tok.type) {
        case token_type::valueseparator:
            handle_member(object);
            break;
        case token_type::endobject:
            return;
        default:
            throw std::invalid_argument("json not compliant with rfc 8259. Members must be separated either by a value separator or by an end object");
    }
}

void json::receive_token(token &tok) noexcept {
    if (!queueToken.empty()) {
        tok = queueToken.front();
        queueToken.pop();
        return;
    }
    tok.type = token_type::eof;
    tok.literal = '\0';
}

void json::create_obj(serializable **object, void(*func)(serializable **)) {
    (func)(object);
    handle_member(*object);
}

void json::deserializer(const std::string &json, double &object) {
    lexer lex(json);
    lex.read_input(queueToken);

    if (queueToken.front().type == token_type::numberdouble)
        object = std::stod(get<std::string>(queueToken.front().literal));
    else
        throw std::invalid_argument("json string must contains a unique double");
}

void json::deserializer(const std::string &json, long long &object) {
    lexer lex(json);
    lex.read_input(queueToken);

    if (queueToken.front().type == token_type::numberlong)
        object = std::stoll(get<std::string>(queueToken.front().literal));
    else
        throw std::invalid_argument("json string must contains a unique long long");
}

void json::deserializer(const std::string &json, bool &object) {
    lexer lex(json);
    lex.read_input(queueToken);

    if (queueToken.front().type == token_type::boolean)
        object = get<std::string>(queueToken.front().literal) == "true";
    else
        throw std::invalid_argument("json string must contains a unique boolean");
}

void json::deserializer(const std::string &json, std::string &object) {
    lexer lex(json);
    lex.read_input(queueToken);

    if (queueToken.front().type == token_type::string)
        object = get<std::string>(queueToken.front().literal);
    else
        throw std::invalid_argument("json string must contains a unique string");
}