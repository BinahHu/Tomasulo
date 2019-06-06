//
// Created by SeventhHeaven on 2019-06-05.
//

#include "Var.h"
#include "util.h"

void Var::show() {
    switch(type) {
        case EMPTY:
            cout << "EMPTY";
            break;
        case INT:
            if(hexflag)
                cout << hex << this->value << dec;
            else
                cout << value;
            break;
        case REG:
            cout << "F" << value;
            break;
    }
}