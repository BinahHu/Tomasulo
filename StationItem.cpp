//
// Created by SeventhHeaven on 2019-06-05.
//

#include "StationItem.h"
#include "util.h"

void StationItem::showTitle() {
    cout << "Comp\tBusy\tOp\tVj\tVk\tQj\tQk\tAddr\tstatus\tremaincycle" << endl;
}

string StationItem::getCompName() {
    string comp;
    int pos = id;
    switch(type) {
        default:
            comp = "Undefined";
            break;
        case ADDER:
            comp = "Adder";
            pos -= ASS;
            break;
        case MULTER:
            comp = "Multi";
            pos -= MSS;
            break;
        case LOADER:
            comp = "Loader";
            pos -= LSS;
            break;
    }
    return comp + to_string(pos);
}

void StationItem::show() {
    cout << getCompName() << '\t';
    cout << (busy ? "yes" : "no");
    if(!busy)
        return;
    cout << '\t';
    cout << INSTYPENAME[insts[instid].type] << '\t';
    if(qj == -1) cout << vj;
    cout << '\t';
    if(qk == -1) cout << vk;
    cout << '\t';
    if(qj != -1) cout << station[qj].getCompName();
    cout << '\t';
    if(qk != -1) cout << station[qk].getCompName();
    cout << '\t';
    if(type == LOADER)
        cout << this->addr;
    cout << '\t';
    cout << STATIONSTATUSNAME[status] << '\t';
    cout << remaincycle << '\t';
}

bool StationItem::ready() {
    if(status == READY)
        return true;
    if(qj == -1 && qk == -1) {
        if(status == WAITTING) {
            status = READY;
            if(debugflag)
                cout << "Station " << id << " is ready now" << endl;
            return true;
        } else
            return false;
    }
    return false;
}

void StationItem::update() {
    if(status != RUNNING)
        return;
    if(--remaincycle == 0)
    {
        status = OVER;
        if(debugflag)
            cout << "Station " << id << " is over" << endl;
        updateBoard(instid, 1, cycle);
        releaseComp(id);
    }
}

void StationItem::WriteBackCheck(int sid) {
    if(qj == sid) {
        vj = station[sid].dest;
        qj = -1;
    }
    if(qk == sid) {
        vk = station[sid].dest;
        qk = -1;
    }
}

void StationItem::exec() {
    switch(insts[instid].type) {
        default:
        case JUMP:
            return;
        case ADD:
            dest = vj + vk;
            break;
        case SUB:
            dest = vj - vk;
            break;
        case MUL:
            dest = vj * vk;
            break;
        case DIV:
            dest = vj / vk;
            break;
        case LD:
            dest = vj;
            break;
    }
}