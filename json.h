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

    void receive_token(token&);
    template <typename T>
    void create_obj(serializable**);
//    void create_obj(serializable** object ,void(*)(serializable**));
    void create_array();
    void handle_member(serializable*);

public:
//    template <typename T> std::string serializer(T);
    template <typename T> void deserializer(const std::string&, serializable**);
    void deserializer(const std::string&, double&);
    void deserializer(const std::string&, bool&);
    void deserializer(const std::string&, std::string&);
};

template <typename T> void json::deserializer(const std::string &json, serializable** object){
    if(!std::is_base_of<serializable, T>())
        throw;

    lexer lex(json);
    lex.read_input(queueToken);

    token tok;
    receive_token(tok);

    if(tok.type != token_type::beginobject)
        throw;

    create_obj<T>(object);
}

template<typename T> void json::create_obj(serializable** object) {
    if (!std::is_base_of<serializable, T>())
        throw;

    *object = new T();
    handle_member(*object);
}
#endif //JSONPP_JSON_H