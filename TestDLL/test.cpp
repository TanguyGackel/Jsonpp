//
// Created by Tanguy on 02/01/2024.
//

#include <iostream>
#include "../json.h"
#include "classTest.h"

int main(){

    std::string input {R"({ "Width": 80.545, "Height": 540,"Url":"http://www.example.com/image/481989943" , "Animated":true})"};

    json *j = new json();
    classTest *s = nullptr;

    j->deserializer<classTest>(input, (serializable**)(&s));

    std::cout << s->Url << std::endl;
    std::cout << s->Width << std::endl;
    std::cout << s->Height << std::endl;
    std::cout << s->Animated << std::endl;


    return 0;
}