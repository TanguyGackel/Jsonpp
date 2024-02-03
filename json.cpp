//
// Created by Tanguy on 23/12/2023.
//

#include <iostream>
#include "json.h"

using variantvector = std::variant<std::vector<std::string> *, std::vector<double> *,
        std::vector<long long> *, std::vector<bool> *, std::vector<void*>*, std::vector<serializable*>*>;

void json::write_member(std::string &json, serializable &object){

    std::map<std::string, std::variant<void*, void **>> &map_attributes = object.get_attributes();
    std::map<std::string, variable_type> &map_types = object.get_types();

    for (auto &pair: map_attributes) {
        json += "\"";
        json += pair.first;
        json += "\"";
        json.push_back(':');

        variable_type v = map_types[pair.first];

        switch (v) {
            case variable_type::j_double:
                json += std::to_string(*(double *) get<void*>(pair.second));
                break;
            case variable_type::j_long:
                json += std::to_string(*(long long *) get<void*>(pair.second));
                break;
            case variable_type::j_string:
                json += "\"";
                json += *(std::string *) get<void*>(pair.second);
                json += "\"";
                break;
            case variable_type::j_boolean:
                json += *(bool *) get<void*>(pair.second) == 1 ? "true" : "false";
                break;
            case variable_type::j_object: {
                json.push_back('{');
                serializable *obj = *(serializable **) get<void **>(pair.second);
                if (obj != nullptr) {
                    write_member(json, *obj);
                    json.pop_back();
                }
                json.push_back('}');
                break;
            }
            case variable_type::j_array_string:
            case variable_type::j_array_double:
            case variable_type::j_array_long:
            case variable_type::j_array_boolean:
            case variable_type::j_array_object:
                json.push_back('[');
                write_array(json, get<void*>(pair.second), v);
                json.push_back(']');
                break;
            default :
                throw std::invalid_argument("The type you provided for " + pair.first + " is not a json compliant type");
        }
        json.push_back(',');
    }
}

void json::write_array(std::string &json, void* vector, variable_type &type) {

    switch (type){
        case variable_type::j_array_string:
            for(const auto& i : *(std::vector<std::string>*)vector){
                json += i;
                json.push_back(',');
            }
            break;
        case variable_type::j_array_double:
            for(const auto& i : *(std::vector<double>*)vector){
                json += std::to_string(i);
                json.push_back(',');
            }
            break;
        case variable_type::j_array_long:
            for(const auto& i : *(std::vector<long long>*)vector){
                json += std::to_string(i);
                json.push_back(',');
            }
            break;
        case variable_type::j_array_boolean:
            for(const auto& i : *(std::vector<bool>*)vector){
                json += std::to_string(i);
                json.push_back(',');
            }
            break;
        case variable_type::j_array_object:{
            for(const auto& i : *(std::vector<serializable *>*)vector){
                json.push_back('{');
                write_member(json, *i);
                json.pop_back();
                json.push_back('}');
                json.push_back(',');
            }
            break;
        }
//        case variable_type::j_array_array
        default :
            throw std::invalid_argument("Wrong array type");
    }
    if(json.back() != '[') {
        json.pop_back();
    }
}

void json::serializer(std::string &json, serializable &object) {
    json.clear();
    json.push_back('{');

    write_member(json, object);

    json.pop_back();
    json.push_back('}');
}

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

    variantvector vector;

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
            throw std::invalid_argument("json not compliant with rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
    }

    while (tok.type != token_type::endarray) {
        if (tok.type == token_type::eof)
            throw std::invalid_argument("json not compliant with rfc 8259. An array must be closed with an end array symbol");

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
                throw std::invalid_argument("json not compliant with rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
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
        throw std::invalid_argument("json not compliant with rfc 8259. A member must start with a string");

    std::string name = get<std::string>(tok.literal);

    receive_token(tok);
    if (tok.type != token_type::nameseparator)
        throw std::invalid_argument("json not compliant with rfc 8259. Member's name and value must be separate by a name separator");

    std::map<std::string, std::variant<void*, void **>> &map_attributes = object->get_attributes();

    auto obj_variable = map_attributes.find(name);
    if (obj_variable != map_attributes.end()) {
        receive_token(tok);
        switch (tok.type) {
            case token_type::numberdouble: {
                *(double *) get<void*>(obj_variable->second) = handle_double(tok);
                break;
            }
            case token_type::numberlong: {
                *(long long *) get<void*>(obj_variable->second) = handle_long_long(tok);
                break;
            }
            case token_type::string: {
                *(std::string *) get<void*>(obj_variable->second) = handle_string(tok);
                break;
            }
            case token_type::boolean: {
                *(bool *) get<void*>(obj_variable->second) = handle_bool(tok);
                break;
            }
            case token_type::null:
                break;
            case token_type::beginobject: {

                std::map<std::string, void (*)(serializable **)> &map_constructors = object->get_constructors();
                auto constructor = map_constructors.find(name);
                if (constructor != map_constructors.end()) {
                    create_obj((serializable**)get<void**>(obj_variable->second), constructor->second);
                }
                break;
            }
            case token_type::beginarray: {
                handle_array(get<void*>(obj_variable->second), name, object);
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