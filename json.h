//
// Created by Tanguy on 23/12/2023.
//

#ifndef JSONPP_JSON_H
#define JSONPP_JSON_H

#include <string>
#include <queue>

#include "serializable.h"
#include "token.h"
#include "lexer.h"

class json {

private:
    std::queue<token> queueToken;

    void receive_token(token&) noexcept;
    void create_obj(serializable** object ,void(*)(serializable**));
    void create_array();
    void handle_member(serializable*);

public:
//    template <typename T> std::string serializer(T);
    template <typename T> void deserializer(const std::string&, serializable**);
    void deserializer(const std::string&, double&);
    void deserializer(const std::string&, long long&);
    void deserializer(const std::string&, bool&);
    void deserializer(const std::string&, std::string&);
};


template <typename T> void json::deserializer(const std::string &json, serializable** object){
    if(!std::is_base_of<serializable, T>())
        throw std::invalid_argument("Type given as a template to the deserializer must inherit from base class 'serializable'");

    lexer lex(json);
    lex.read_input(queueToken);

    token tok;
    receive_token(tok);

    if(tok.type == token_type::beginobject) {
        *object = new T();
        handle_member(*object);
    }
    else if (tok.type == token_type::beginarray)
        create_array();
    else
        throw std::invalid_argument("json string must start with { or [");
}
#endif //JSONPP_JSON_H