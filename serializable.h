//
// Created by Tanguy on 29/12/2023.
//

#ifndef JSONPP_SERIALIZABLE_H
#define JSONPP_SERIALIZABLE_H

#include <map>
#include <string>
#include <variant>

enum class variable_type {
    j_double,
    j_long,
    j_string,
    j_boolean,
    j_object,
    j_array_string,
    j_array_long,
    j_array_double,
    j_array_boolean,
    j_array_object,
//    j_array_array
};

class serializable {
protected:
    std::map<std::string, std::variant<void*, void **>> attributes;
    std::map<std::string, void (*)(serializable **)> constructors;
    std::map<std::string, variable_type> types;
public:
    virtual ~serializable() = default;
    [[nodiscard]] std::map<std::string, std::variant<void*, void **>> &get_attributes() { return attributes; }
    [[nodiscard]] std::map<std::string, void (*)(serializable **)> &get_constructors() { return constructors; }
    [[nodiscard]] std::map<std::string, variable_type> &get_types() { return types; }

};


#endif //JSONPP_SERIALIZABLE_H
