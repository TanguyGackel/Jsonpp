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
        throw;

    std::string name = get<std::string>(tok.literal);

    receive_token(tok);
    if(tok.type != token_type::nameseparator)
        throw;

    std::map<std::string, void*> &map_attributes = object->get_attributes();

    receive_token(tok);
    switch (tok.type) {
        case token_type::numberdouble:{
            double value = std::stod(get<std::string>(tok.literal));
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(double*)obj_variable->second = value;
            }
            break;
        }
        case token_type::numberlong:{
            long long value = std::stoll(get<std::string>(tok.literal));
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(long long*)obj_variable->second = value;
            }
            break;
        }
        case token_type::string:{
            std::string value = get<std::string>(tok.literal);
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(std::string*)obj_variable->second = value;
            }
            break;
        }
        case token_type::boolean:{
            bool value = get<std::string>(tok.literal) == "true";
            auto obj_variable = map_attributes.find(name);
            if (obj_variable != map_attributes.end()) {
                *(bool*)obj_variable->second = value;
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
        case token_type::beginarray:

            break;
        default:
            throw;
    }

    receive_token(tok);
    switch(tok.type) {
        case token_type::valueseparator:
            handle_member(object);
            break;
        case token_type::endobject:
            return;
        default:
            throw;
    }
}

void json::receive_token(token &tok) {
    tok = queueToken.front();
    queueToken.pop();
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

    if(std::holds_alternative<std::string>(queueToken.front().literal))
        object = std::stod(get<std::string>(queueToken.front().literal));
    else
        throw;
}
void json::deserializer(const std::string &json, bool &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(std::holds_alternative<std::string>(queueToken.front().literal))
        object = get<std::string>(queueToken.front().literal) == "true";
    else
        throw;
}
void json::deserializer(const std::string &json, std::string &object){
    lexer lex(json);
    lex.read_input(queueToken);

    if(std::holds_alternative<std::string>(queueToken.front().literal))
        object = get<std::string>(queueToken.front().literal);
    else
        throw;
}