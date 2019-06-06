//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_REGITEM_H
#define EXPERIMENT2_REGITEM_H
#include "def.h"
struct RegItem{
    int value;
    int id;
    int srcid;

    RegItem() {
        value = 0;
        id = 0;
        srcid = -1;
    }
    static void showTitle();
    void show();
};


#endif //EXPERIMENT2_REGITEM_H
