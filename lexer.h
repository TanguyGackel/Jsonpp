//
// Created by Tanguy on 26/12/2023.
//

#ifndef JSONPP_LEXER_H
#define JSONPP_LEXER_H

#include <string_view>
#include <queue>
#include <stdexcept>

#include "token.h"

class lexer {

private:
    std::string_view input;
    size_t position;
    size_t read_position;
    char byte;

    bool checkEOF(token &t) const noexcept;
    void next_char() noexcept;
    void read_string(token &) noexcept;
    void read_number(token &) noexcept;
    void read_boolean(token &) noexcept;
    void read_null(token &) noexcept;
    void next_token(token &) noexcept;

public:
    explicit lexer(std::string_view input);
    void read_input(std::queue<token> &);

};

#endif //JSONPP_LEXER_H
