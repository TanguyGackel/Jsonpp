//
// Created by Tanguy on 02/01/2024.
//

#include <iostream>
#include "../json.h"
#include "classTest.h"

int main(){

    std::string input {R"({"Width":800,"Url":"http://www.example.com/image/481989943","Animated":true,"cT":{"Width":1350,"cT":{"Width":-10e-54,"cT":{"Width":2700.80}}}})"};

    json *j = new json();
    classTest *s = nullptr;

    j->deserializer<classTest>(input, (serializable**)(&s));

    std::cout << s->Url << std::endl;
    std::cout << s->Width << std::endl;
    std::cout << s->Animated << std::endl;
    std::cout << s->cT->Width << std::endl;
    std::cout << s->cT->cT->Width << std::endl;
    std::cout << s->cT->cT->cT->Width << std::endl;

    return 0;
}