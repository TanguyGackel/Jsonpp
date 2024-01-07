//
// Created by Tanguy on 02/01/2024.
//

#include <iostream>
#include "classTest.h"
classTest::classTest() {
    Width = 50;
    Animated = false;
    cT = nullptr;
    attributes = {
            {"Width", (void*)&Width},
            {"Url", (void*)&Url},
            {"Animated", (void*)&Animated},
            {"cT", (void*)&cT}
    };
    constructors = {
            {"cT", &create_class_test}
    };
}
