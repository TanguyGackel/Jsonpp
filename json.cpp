//
// Created by Tanguy on 23/12/2023.
//

#include <iostream>
#include "json.h"

void json::handle_member(serializable *object) {
    token tok;
    receive_token(tok);
    if(tok.type == token_type::endobject)
        return;
    if(tok.type != token_type::string)
        throw std::invalid_argument("json not compliant to rfc 8259. A member must start start with a string");

    std::string name = get<std::string>(tok.literal);

    receive_token(tok);
    if(tok.type != token_type::nameseparator)
        throw std::invalid_argument("json not compliant to rfc 8259. Member's name and value must be separate by a name separator");

    std::map<std::string, void*> &map_attributes = object->get_attributes();

    receive_token(tok);
    switch (tok.type) {
        case token_type::numberdouble:{
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(double*)obj_variable->second = std::stod(get<std::string>(tok.literal));
            }
            break;
        }
        case token_type::numberlong:{
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(long long *) obj_variable->second = std::stoll(get<std::string>(tok.literal));
            }
            break;
        }
        case token_type::string:{
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(std::string*)obj_variable->second = get<std::string>(tok.literal);
            }
            break;
        }
        case token_type::boolean:{
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(bool*)obj_variable->second = get<std::string>(tok.literal) == "true";
            }
            break;
        }
        case token_type::null:
            break;
        case token_type::beginobject:{
            auto obj_variable = map_attributes.find(name);

            std::map<std::string, void(*)(serializable**)> &map_constructors = object->get_constructors();
            auto constructor = map_constructors.find(name);

            if (obj_variable != map_attributes.end())
                if(constructor != map_constructors.end())
                    create_obj((serializable**)obj_variable->second, constructor->second);
            break;
        }
        case token_type::beginarray:{
            auto obj_variable = map_attributes.find(name);



            break;
        }
        default:
            throw std::invalid_argument("json not compliant to rfc 8259. " + get<std::string>(tok.literal) + " is not a legal value");
    }

    receive_token(tok);
    switch(tok.type) {
        case token_type::valueseparator:
            handle_member(object);
            break;
        case token_type::endobject:
            return;
        default:
            throw std::invalid_argument("json not compliant to rfc 8259. Members must be separated either by a value separator or by an end object");
    }
}

void json::receive_token(token &tok) noexcept{
    if(!queueToken.empty()) {
        tok = queueToken.front();
        queueToken.pop();
        return;
    }
    tok.type = token_type::eof;
    tok.literal = '\0';
}

void json::create_array() {

}

void json::create_obj(serializable** object ,void(*func)(serializable**)){
    (func)(object);
    handle_member(*object);
}

void json::deserializer(const std::string &json, double &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(queueToken.front().type == token_type::numberdouble)
        object = std::stod(get<std::string>(queueToken.front().literal));
    else
        throw std::invalid_argument("json string must contains a unique double");
}
void json::deserializer(const std::string &json, long long &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(queueToken.front().type == token_type::numberlong)
        object = std::stoll(get<std::string>(queueToken.front().literal));
    else
        throw std::invalid_argument("json string must contains a unique long long");
}
void json::deserializer(const std::string &json, bool &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(queueToken.front().type == token_type::boolean)
        object = get<std::string>(queueToken.front().literal) == "true";
    else
        throw std::invalid_argument("json string must contains a unique boolean");
}
void json::deserializer(const std::string &json, std::string &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(queueToken.front().type == token_type::string)
        object = get<std::string>(queueToken.front().literal);
    else
        throw std::invalid_argument("json string must contains a unique string");
}