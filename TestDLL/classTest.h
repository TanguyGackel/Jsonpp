//
// Created by Tanguy on 02/01/2024.
//

#ifndef JSONPP_CLASSTEST_H
#define JSONPP_CLASSTEST_H


#include <string>
#include "../serializable.h"

class classTest: public serializable{
public:
    double Width;
    long long Height;
    std::string Url;
    bool Animated;
    classTest* cT;

    classTest();

    static void create_class_test(serializable** object){
        *object = new classTest();
    }
};



#endif //JSONPP_CLASSTEST_H
