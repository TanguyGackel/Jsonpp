//
// Created by Tanguy on 29/12/2023.
//

#ifndef JSONPP_SERIALIZABLE_H
#define JSONPP_SERIALIZABLE_H

#include <map>
#include <string>
class serializable {
protected:
    std::map<std::string, void*> attributes;
    std::map<std::string, void(*)(serializable**)> constructors;

public:
    ~serializable()= default;
    [[nodiscard]] std::map<std::string, void*>& get_attributes(){return attributes;}
    [[nodiscard]] std::map<std::string, void(*)(serializable**)>& get_constructors(){return constructors;}
};


#endif //JSONPP_SERIALIZABLE_H
