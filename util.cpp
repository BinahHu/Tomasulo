//
// Created by SeventhHeaven on 2019-06-05.
//

#include "util.h"
vector<Instruction> insts;
vector<BoardItem> board;
vector<StationItem> station;
vector<RegItem> regs;

int AdderCompNum = MAC;
int MultCompNum = MMC;
int LoadCompNum = MLC;

bool blockbyjmp = false;
int cycle = 0;
int inst2issue = 0;

//const int INSTTIME[INSTTYPENUM] = {3, 1, 3, 3, 12, 40};
const int INSTTIME[INSTTYPENUM] = {3, 1, 3, 3, 4, 4};
const char* INSTYPENAME[INSTTYPENUM+1] = {"LD", "JUMP", "ADD", "SUB", "MUL", "DIV", "UNDEF"};
const char* VARTYPENUM[3] = {"REG", "INT", "EMPTY"};
const char* STATIONSTATUSNAME[4] = {"WAITTING", "RUNNING", "READY", "OVER"};

struct StatusRecord{
    vector<Instruction> Sinsts;
    vector<BoardItem> Sboard;
    vector<StationItem> Sstation;
    vector<RegItem> Sregs;
    int Scycle;
    int Sinst2issue;

    void save() {
        Sinsts.resize(insts.size());
        memcpy((void*)(&Sinsts), (void*)(&insts), sizeof(struct Instruction) * insts.size());

        Sboard.resize(board.size());
        memcpy((void*)(&Sboard), (void*)(&board), sizeof(struct BoardItem) * board.size());

        Sstation.resize(station.size());
        memcpy((void*)(&Sstation), (void*)(&station), sizeof(struct StationItem) * station.size());

        Sregs.resize(regs.size());
        memcpy((void*)(&Sregs), (void*)(&regs), sizeof(struct RegItem) * regs.size());

        Scycle = cycle;
        Sinst2issue = inst2issue;
    }
    void restore() {
        insts.resize(Sinsts.size());
        memcpy((void*)(&insts), (void*)(&Sinsts), sizeof(struct Instruction) * Sinsts.size());

        board.resize(Sboard.size());
        memcpy((void*)(&board), (void*)(&Sboard), sizeof(struct BoardItem) * Sboard.size());

        station.resize(Sstation.size());
        memcpy((void*)(&station), (void*)(&Sstation), sizeof(struct StationItem) * Sstation.size());

        regs.resize(Sregs.size());
        memcpy((void*)(&regs), (void*)(&Sregs), sizeof(struct RegItem) * Sregs.size());
    }

    ~StatusRecord() {
        Sinsts.clear();
        Sboard.clear();
        Sstation.clear();
        Sregs.clear();
    }
};

vector<StatusRecord*> status;

void displayAll() {
    displayBoard();
    displayStation();
    displayRegister();
}

void displayBoard() {
    cout << "========BOARD========" << endl;
    BoardItem::showTitle();
    for(int i = 0; i < board.size(); i++) {
        board[i].show();
        cout << endl;
    }
    cout << endl;
}
void displayStation() {
    cout << "========STATION========" << endl;
    StationItem::showTitle();
    for(int i = 0; i < station.size(); i++) {
        if(!station[i].busy)continue;
        station[i].show();
        cout << endl;
    }
    cout << endl;
}
void displayRegister() {
    cout << "========REGISTER========" << endl;
    cout << "RegID\t";
    for(int i = 0; i < regs.size(); i++)
        cout << i << '\t';
    cout << endl;

    cout << "Addr\t";
    for(int i = 0; i < regs.size(); i++) {
        regs[i].show();
        cout << '\t';
    }
    cout << endl;

}
void displayInstruction(){
    cout << "========INSTRUCTION========" << endl;
    Instruction::showTitle();
    for(int i = 0; i < insts.size(); i++){
        insts[i].show();
        cout << endl;
    }
    cout << endl;
}

void parseInst(vector<string>& seg){
    insts.push_back(Instruction());
    Instruction& inst = insts[insts.size()-1];
    inst.id = insts.size() - 1;
    unsigned tmp;
    //Parse Type
    for(int i = 0; i < INSTTYPENUM; i++) {
        if(strcmp(seg[0].c_str(), INSTYPENAME[i]) == 0) {
            inst.type = (INSTYPE)i;
            break;
        }
    }
    //Paese dest
    if(seg[1][0] == 'F') {  //Register
        inst.dest.type = REG;
        stringstream ss(seg[1].substr(1));
        ss >> inst.dest.value;
    }
    else {  //Integer
        inst.dest.type = INT;
        stringstream ss(seg[1]);
        ss.setf(ios::showbase);
        ss >> hex >> tmp;
        inst.dest.value = (int)tmp;
    }

    //Paese VJ
    if(seg[2][0] == 'F') {  //Register
        inst.vj.type = REG;
        stringstream ss(seg[2].substr(1));
        ss >> inst.vj.value;
    }
    else {  //Integer
        inst.vj.type = INT;
        stringstream ss(seg[2]);
        ss.setf(ios::showbase);
        ss >> hex >> tmp;
        inst.vj.value = (int)tmp;
    }

    if(seg.size() > 3) {
        //Paese VK
        if(seg[3][0] == 'F') {  //Register
            inst.vk.type = REG;
            stringstream ss(seg[3].substr(1));
            ss >> inst.vk.value;
        }
        else {  //Integer
            inst.vk.type = INT;
            stringstream ss(seg[3]);
            ss.setf(ios::showbase);
            ss >> hex >> tmp;
            inst.vk.value = (int)tmp;
        }
    }
}

UserInst parseUser(char* c) {
    UserInst inst;
    inst.type = 5;
    int len = strlen(c);
    if(len == 0)
        inst.type = 0;
    else {
        switch(c[0]) {
            case 'n':
                inst.type = 1;
                break;
            case 'c':
                inst.type = 2;
                break;
            case 's':
                inst.type = 3;
                break;
            case 'q':
                inst.type = 4;
                break;
            case 'h':
            default:
                inst.type = 5;
                break;
        }
    }
    if(inst.type == 3) {
        if(len == 1)
            inst.disp = 0;
        else if(len != 3 && c[1] != ' ')
            inst.type = 5;
        else{
            switch(c[2]) {
                case 'a':
                    inst.disp = 0;
                    break;
                case 'i':
                    inst.disp = 1;
                    break;
                case 'b':
                    inst.disp = 2;
                    break;
                case 's':
                    inst.disp = 3;
                    break;
                case 'r':
                    inst.disp = 4;
                    break;
                default:
                    inst.type = 5;
                    break;
            }
        }
    }
    else if(len > 0 && len != 1)
        inst.type = 5;
    return inst;
}

void display(int disp) {
    switch(disp) {
        case 0:
            displayAll();
            break;
        case 1:
            displayInstruction();
            break;
        case 2:
            displayBoard();
            break;
        case 3:
            displayStation();
            break;
        case 4:
            displayRegister();
            break;
        case 5:
        default:
            showhelp();
            break;
    }
}

void showhelp() {
    cout << "Tomasulo Usage:" << endl;
    cout << "n\t\t\t-- take one step" << endl;
    cout << "c\t\t\t-- continue until program end" << endl;
    cout << "s [type]\t\t-- display" << endl;
    cout << "s a\t\t\t-- display All" << endl;
    cout << "s i\t\t\t-- display Instructions" << endl;
    cout << "s b\t\t\t-- display Board status" << endl;
    cout << "s s\t\t\t-- display Station status" << endl;
    cout << "s r\t\t\t-- display Register status" << endl;
    cout << "q\t\t\t-- quit" << endl;
    cout << "h\t\t\t-- show this help" << endl;
}


bool getComp(int i) {
    int* CompNum;
    switch(station[i].type) {
        case ADDER:
            CompNum = &AdderCompNum;
            break;
        case MULTER:
            CompNum = &MultCompNum;
            break;
        case LOADER:
            CompNum = &LoadCompNum;
            break;
    }
    if(*CompNum <= 0)
        return false;
    (*CompNum)--;
    return true;
}

void releaseComp(int i) {
    int* CompNum;
    switch(station[i].type) {
        case ADDER:
            CompNum = &AdderCompNum;
            break;
        case MULTER:
            CompNum = &MultCompNum;
            break;
        case LOADER:
            CompNum = &LoadCompNum;
            break;
    }
    (*CompNum)++;
}

bool checkover() {
    if(inst2issue < insts.size())
        return false;
    for(int i = 0; i < MSTA; i++)
        if(station[i].busy)
            return false;
    return true;
}

void updateBoard(int instid, int status, int val) {
    int boardid = insts[instid].boardid;
    if(boardid != -1) {
        board[boardid].update(status, val);
        return;
    }
    board.push_back(BoardItem());
    boardid = board.size() - 1;
    board[boardid].instid = instid;
    board[boardid].update(status, val);
    insts[instid].boardid = boardid;
}

void saveStatus() {
    StatusRecord* s = new StatusRecord();
    s->save();
    status.push_back(s);
}

void restoreStatus() {
    StatusRecord* s = status[status.size() - 1];
    status.pop_back();
    if(s) {
        s->restore();
        delete s;
    }
}

void popStatus() {
    StatusRecord* s = status[status.size() - 1];
    status.pop_back();
    if(s)
        delete s;
}