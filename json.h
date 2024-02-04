//
// Created by Tanguy on 23/12/2023.
//

#ifndef JSONPP_JSON_H
#define JSONPP_JSON_H

#include <string>

#include "serializable.h"
#include "lexer.h"

class json {

private:
    std::queue<token> queueToken;
    void receive_token(token &) noexcept;
    void create_obj(serializable **, void(*)(serializable **));
    void handle_array(void *, const std::string &, serializable *);
    void handle_member(serializable *);
    static long long handle_long_long(token &);
    static double handle_double(token &);
    static std::string handle_string(token &);
    static bool handle_bool(token &);
    void write_member(std::string &, serializable &);
    void write_array(std::string &, void *, variable_type &);

public:
    void serializer(std::string &, serializable &);
    template<typename T>
    void deserializer(const std::string &, serializable **);
    void deserializer(const std::string &, double &);
    void deserializer(const std::string &, long long &);
    void deserializer(const std::string &, bool &);
    void deserializer(const std::string &, std::string &);
};


template<typename T>
void json::deserializer(const std::string &json, serializable **object) {
    if (!std::is_base_of<serializable, T>())
        throw std::invalid_argument("Type given as a template to the deserializer must inherit from base class 'serializable'");

    lexer lex(json);
    lex.read_input(queueToken);

    token tok;
    receive_token(tok);

    if (tok.type == token_type::beginobject) {
        *object = new T();
        handle_member(*object);
    } else if (tok.type == token_type::beginarray) {}
        //Todo
    else
        throw std::invalid_argument("json string must start with { or [");
}

#endif //JSONPP_JSON_H