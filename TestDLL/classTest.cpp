//
// Created by Tanguy on 02/01/2024.
//

#include <iostream>
#include "classTest.h"
classTest::classTest() {
    Width = 50.1;
    Height = 50;
    Animated = false;
    cT = nullptr;
    attributes = {
            {"Width", (void*)&Width},
            {"Height", (void*)&Height},
            {"Url", (void*)&Url},
            {"Animated", (void*)&Animated},
            {"cT", (void*)&cT}
    };
    constructors = {
            {"cT", &create_class_test}
    };
}
