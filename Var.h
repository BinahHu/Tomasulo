//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_VAR_H
#define EXPERIMENT2_VAR_H

#include "def.h"

struct Var{
    VARTYPE type;
    int value;

    Var() {
        type = EMPTY;
        value = 0;
    }
    void show();
};


#endif //EXPERIMENT2_VAR_H
