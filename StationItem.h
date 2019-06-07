//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_STATIONITEM_H
#define EXPERIMENT2_STATIONITEM_H
#include "def.h"
#include "Instruction.h"
struct StatusRecord;
struct StationItem{
    bool busy;
    STATIONSTATUS status;
    STATIONTYPE type;
    int instid;
    int id;
    int dest;
    int vj;
    int vk;
    int qj;
    int qk;
    int addr;
    int remaincycle;
    StatusRecord* jumpstatus;
    int jumphis;

    StationItem() {
        busy = false;
        instid = 0;
        status = WAITTING;
        dest = 0;
        vj = 0;
        vk = 0;
        qj = -1;
        qk = -1;
        addr = 0;
        remaincycle = 0;
        id = 0;
        type = ADDER;
        jumpstatus = NULL;
        jumphis = 0;
    }

    string getCompName();
    static void showTitle();
    bool ready();
    void update();
    void exec();
    void WriteBackCheck(int sid);
    void show();

};


#endif //EXPERIMENT2_STATIONITEM_H
