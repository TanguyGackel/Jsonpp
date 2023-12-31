//
// Created by Tanguy on 30/12/2023.
//

#ifndef JSONPP_TOKEN_H
#define JSONPP_TOKEN_H

#include <variant>
#include <string>

enum class token_type {
    illegal,
    eof,
    beginobject,
    beginarray,
    endobject,
    endarray,
    nameseparator,
    valueseparator,
    number,
    string,
    boolean,
    null,
};

struct token final {
    token_type type;
    std::variant<std::string, char> literal;
};

#endif //JSONPP_TOKEN_H
