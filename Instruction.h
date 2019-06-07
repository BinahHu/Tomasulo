//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_INSTRUCTION_H
#define EXPERIMENT2_INSTRUCTION_H
#include "def.h"
#include "Var.h"

struct Instruction{
    INSTYPE type;
    Var dest;
    Var vj;
    Var vk;
    int id;
    int boardid;
    int jumphis;

    Instruction() {
        type = UNDEF;
        id = -1;
        boardid = -1;
        jumphis = 0;
    }
    static void showTitle();
    void show();
};


#endif //EXPERIMENT2_INSTRUCTION_H
