//
// Created by Tanguy on 29/12/2023.
//

#ifndef JSONPP_SERIALIZABLE_H
#define JSONPP_SERIALIZABLE_H

#include <map>
#include <string>

class serializable {

    protected:
        std::map<std::string, void*> metadata;

    public:
        virtual ~serializable()= default;

        [[nodiscard]] virtual std::map<std::string, void*> getMetadata() const {return metadata;}
};


#endif //JSONPP_SERIALIZABLE_H
