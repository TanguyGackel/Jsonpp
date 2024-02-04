#include "lexer.h"

lexer::lexer(std::string_view input) {
    if (input.empty()) {
        throw std::invalid_argument("lexer's input can't be void");
    }
    this->input = input;
    position = 0;
    read_position = 1;
    byte = input[0];
}

bool lexer::checkEOF(token &t) const noexcept {
    if (this->byte == '\0') {
        t.type = token_type::illegal;
        return true;
    }
    return false;
}

void lexer::next_char() noexcept {
    if (this->read_position >= this->input.length()) {
        this->byte = '\0';
    } else [[likely]] {
        this->byte = this->input[this->read_position];
    }

    this->position = this->read_position;
    this->read_position += 1;
}

void lexer::read_string(token &t) noexcept {
    std::string current;
    next_char();

    while (this->byte != '"') {
        if (checkEOF(t)) {
            t.literal = current;
            return;
        }

        current += this->byte;
        next_char();
    }

    t.literal = current;
    t.type = token_type::string;
}

void lexer::read_number(token &t) noexcept {
    bool hasDecimal = false;
    bool hasExp = false;

    std::string current;
    current += this->byte;

    if (this->byte == '-') {
        next_char();
        current += this->byte;
    }
    if (this->byte == '0') {
        next_char();
        current += this->byte;
        if (std::isdigit(this->byte) || checkEOF(t)) {
            t.literal = current;
            t.type = token_type::illegal;
            return;
        }
    }
    next_char();

    while (this->byte != ','
           && this->byte != 0x20 && this->byte != 0x09 && this->byte != 0x0A &&
           this->byte != 0x0D //insignificant whitespace
           && this->byte != ']' && this->byte != '}') {                                             //end object/array
        current += this->byte;
        if (!std::isdigit(this->byte)) {
            if (this->byte == '.' && !hasDecimal) {
                if (hasExp) {
                    t.literal = current;
                    t.type = token_type::illegal;
                }
                hasDecimal = true;
            } else if ((this->byte == 'e' || this->byte == 'E') && !hasExp) {
                hasExp = true;
                next_char();
                current += this->byte;
                if ((this->byte != '+' && this->byte != '-' && !std::isdigit(this->byte)) || checkEOF(t)) {
                    t.literal = current;
                    t.type = token_type::illegal;
                    return;
                }
            } else {
                t.literal = current;
                t.type = token_type::illegal;
                return;
            }
        }
        next_char();
        if (checkEOF(t)) {
            t.literal = current;
            return;
        }
    }

    this->read_position--;

    t.literal = current;
    if (hasDecimal)
        t.type = token_type::numberdouble;
    else
        t.type = token_type::numberlong;
}

void lexer::read_boolean(token &t) noexcept {
    std::string current;
    current += this->byte;

    int limit = this->byte == 'f' ? 4 : 3;
    std::string verif = this->byte == 'f' ? "false" : "true";

    for (int i = 0; i < limit; i++) {
        next_char();
        current += this->byte;

        if (checkEOF(t)) {
            t.literal = current;
            return;
        }
    }

    t.literal = current;
    if (current != verif) {
        t.type = token_type::illegal;
        return;
    }
    t.type = token_type::boolean;
}

void lexer::read_null(token &t) noexcept {
    std::string current;
    current += this->byte;

    int limit = 3;
    std::string verif = "null";

    for (int i = 0; i < limit; i++) {
        next_char();
        current += this->byte;

        if (checkEOF(t)) {
            t.literal = current;
            return;
        }
    }

    t.literal = current;
    if (current != verif) {
        t.type = token_type::illegal;
        return;
    }
    t.type = token_type::null;
}

void lexer::next_token(token &t) noexcept {
    switch (this->byte) {
        case '{':
            t.type = token_type::beginobject;
            t.literal = this->byte;
            break;
        case '[':
            t.type = token_type::beginarray;
            t.literal = this->byte;
            break;
        case '}':
            t.type = token_type::endobject;
            t.literal = this->byte;
            break;
        case ']':
            t.type = token_type::endarray;
            t.literal = this->byte;
            break;
        case ':':
            t.type = token_type::nameseparator;
            t.literal = this->byte;
            break;
        case ',':
            t.type = token_type::valueseparator;
            t.literal = this->byte;
            break;
        case '\0':
            t.type = token_type::eof;
            t.literal = this->byte;
            break;
        case '"':
            read_string(t);
            break;
        case 't':
        case 'f':
            read_boolean(t);
            break;
        case 'n':
            read_null(t);
            break;
            //the four next are insignificant whitespace
        case 0x20:
        case 0x09:
        case 0x0A:
        case 0x0D:
            t.type = token_type::insignificant;
            break;
        default:
            if (this->byte == '-' || std::isdigit(this->byte)) {
                read_number(t);
                break;
            }
            t.type = token_type::illegal;
            t.literal = this->byte;
    }

    if (checkEOF(t))
        return;
    this->next_char();
}

void lexer::read_input(std::queue<token> &queueToken) {
    while (this->position < this->input.size()) {
        token tok;
        this->next_token(tok);
        if (tok.type != token_type::insignificant)
            queueToken.push(tok);
    }
}