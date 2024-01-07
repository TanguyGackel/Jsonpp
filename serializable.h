//
// Created by Tanguy on 29/12/2023.
//

#ifndef JSONPP_SERIALIZABLE_H
#define JSONPP_SERIALIZABLE_H

#include <map>
#include <string>
class serializable {

    protected:

    public:
        ~serializable()= default;
        std::map<std::string, void*> attributes;
//        std::map<std::string, void(*)(serializable**)> constructors;
//        std::map<std::string, std::string> attributes_types;

        [[nodiscard]] std::map<std::string, void*>& get_attributes(){return attributes;}
//        [[nodiscard]] std::map<std::string, void(*)(serializable**)>& get_constructors(){return constructors;}
//        [[nodiscard]] std::map<std::string, std::string>& get_attributes_types(){return attributes_types;}

};


#endif //JSONPP_SERIALIZABLE_H
