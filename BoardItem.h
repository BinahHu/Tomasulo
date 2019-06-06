//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_BOARDITEM_H
#define EXPERIMENT2_BOARDITEM_H
#include "def.h"

struct BoardItem{
    int instid;
    int IssueTime;
    int ExecCompTime;
    int WriteResultTime;

    BoardItem() {
        instid = 0;
        IssueTime = -1;
        ExecCompTime = -1;
        WriteResultTime = -1;
    }
    static void showTitle();
    void show();
    void update(int status, int val) {
        //0: Issue
        //1: ExeComp
        //2: WriteBack
        if(status == 0) {
            if(IssueTime == -1)
                IssueTime = val;
        }
        else if(status == 1) {
            if(ExecCompTime == -1)
                ExecCompTime = val;
        }
        else if(status == 2) {
            if(WriteResultTime == -1)
                WriteResultTime = val;
        }
    }
};


#endif //EXPERIMENT2_BOARDITEM_H
