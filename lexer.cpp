#include "lexer.h"

lexer::lexer(std::string_view input)
{
    if (input.empty()) {
        throw std::exception();
    }
    this->input = {input};
    position = {0};
    read_position = {1};
    byte = {input[0]};
}

void lexer::next_char() noexcept
{
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

    while (this->byte != '"'){
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

    if(this->byte == '-') {
        next_char();
        current += this->byte;
    }
    if(this->byte == '0'){
        next_char();
        current += this->byte;
        if(std::isdigit(this->byte)){
            t.literal = current;
            t.type = token_type::illegal;
            return;
        }
    }
    next_char();

    while(this->byte != ',' && this->byte != 0x20 && this->byte != 0x09 && this->byte != 0x0A && this->byte != 0x0D && this->byte != ']' && this->byte != '}'){
        current += this->byte;
        if(!std::isdigit(this->byte)){
            if(this->byte == '.' && !hasDecimal){
                hasDecimal = true;
            }
            else if((this->byte == 'e' || this->byte == 'E') && !hasExp){
                hasExp = true;
                next_char();
                current += this->byte;
                if(this->byte != '+' && this->byte != '-' && std::isdigit(this->byte)){
                    t.literal = current;
                    t.type = token_type::illegal;
                }
            }
            else{
                t.literal = current;
                t.type = token_type::illegal;
            }
        }
        next_char();
    }

    this->read_position--;

    t.literal = current;
    t.type = token_type::number;
}

void lexer::read_boolean(token &t) noexcept {
    std::string current;
    current += this->byte;

    int limit = this->byte == 'f' ? 4 : 3;
    std::string verif = this->byte == 'f' ? "false" : "true";

    for(int i = 0; i < limit; i++){
        next_char();
        current += this->byte;
    }

    t.literal = current;
    if(current != verif) {
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

    for(int i = 0; i < limit; i++){
        next_char();
        current += this->byte;
    }

    t.literal = current;
    if(current != verif) {
        t.type = token_type::illegal;
        return;
    }
    t.type = token_type::null;
}

void lexer::next_token(token &t) noexcept
{
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
            break;
        default:
            if(this->byte == '-' || std::isdigit(this->byte))
            {
                read_number(t);
                break;
            }
            t.type = token_type::illegal;
            t.literal = this->byte;
    }

    this->next_char();
}

void lexer::read_input(std::queue<token>& queueToken){
    for(int i = 0; i < this->input.size(); i++){
        token tok;
        this->next_token(tok);
        queueToken.push(tok);
    }
}