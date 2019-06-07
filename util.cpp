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

int cycle = 0;
int inst2issue = 0;
int savepointid = 0;

//const int INSTTIME[INSTTYPENUM] = {3, 1, 3, 3, 12, 40};
const int INSTTIME[INSTTYPENUM] = {3, 1, 3, 3, 4, 4};
const char* INSTYPENAME[INSTTYPENUM+1] = {"LD", "JUMP", "ADD", "SUB", "MUL", "DIV", "UNDEF"};
const char* VARTYPENUM[3] = {"REG", "INT", "EMPTY"};
const char* STATIONSTATUSNAME[4] = {"WAITTING", "RUNNING", "READY", "OVER"};

StatusRecord* head = NULL;
vector<StatusRecord*> savepoint;

struct StatusRecord{
    vector<Instruction> Sinsts;
    vector<BoardItem> Sboard;
    vector<StationItem> Sstation;
    vector<RegItem> Sregs;
    int Scycle;
    int Sinst2issue;
    int SAdderCompNum;
    int SMultCompNum;
    int SLoadCompNum;
	int Ssavepointid;
    StatusRecord* prev;
    StatusRecord* next;

    void copyfrom(StatusRecord* s) {
        memcpy((void*)this, (void*)s, sizeof(StatusRecord));

        Sinsts.resize(s->Sinsts.size());
        for(int i = 0; i < s->Sinsts.size(); i++)
            memcpy((void*)(&Sinsts[i]), (void*)(&s->Sinsts[i]), sizeof(Instruction));

        Sboard.resize(s->Sboard.size());
        for(int i = 0; i < s->Sboard.size(); i++)
            memcpy((void*)(&Sboard[i]), (void*)(&s->Sboard[i]), sizeof(struct BoardItem));

        Sstation.resize(s->Sstation.size());
        for(int i = 0; i < s->Sstation.size(); i++)
            memcpy((void*)(&Sstation[i]), (void*)(&s->Sstation[i]), sizeof(struct StationItem));

        Sregs.resize(s->Sregs.size());
        for(int i = 0; i < s->Sregs.size(); i++)
            memcpy((void*)(&Sregs[i]), (void*)(&s->Sregs[i]), sizeof(struct RegItem));
    }

    void save(int inst) {
        Sinsts.resize(insts.size());
        for(int i = 0; i < insts.size(); i++)
            memcpy((void*)(&Sinsts[i]), (void*)(&insts[i]), sizeof(Instruction));

        Sboard.resize(board.size());
        for(int i = 0; i < board.size(); i++)
            memcpy((void*)(&Sboard[i]), (void*)(&board[i]), sizeof(struct BoardItem));

        Sstation.resize(station.size());
        for(int i = 0; i < station.size(); i++)
            memcpy((void*)(&Sstation[i]), (void*)(&station[i]), sizeof(struct StationItem));

        Sregs.resize(regs.size());
        for(int i = 0; i < regs.size(); i++)
            memcpy((void*)(&Sregs[i]), (void*)(&regs[i]), sizeof(struct RegItem));

        Scycle = cycle;
        Sinst2issue = inst;
        SAdderCompNum = AdderCompNum;
        SMultCompNum = MultCompNum;
        SLoadCompNum = LoadCompNum;
		Ssavepointid = savepointid;
    }
    void restore() {
        insts.resize(Sinsts.size());
        for(int i = 0; i < Sinsts.size(); i++)
            memcpy((void*)(&insts[i]), (void*)(&Sinsts[i]), sizeof(struct Instruction));

        board.resize(Sboard.size());
        for(int i = 0; i < Sboard.size(); i++)
            memcpy((void*)(&board[i]), (void*)(&Sboard[i]), sizeof(struct BoardItem));

        station.resize(Sstation.size());
        for(int i = 0; i < Sstation.size(); i++)
            memcpy((void*)(&station[i]), (void*)(&Sstation[i]), sizeof(struct StationItem));

        regs.resize(Sregs.size());
        for(int i = 0; i < Sregs.size(); i++)
            memcpy((void*)(&regs[i]), (void*)(&Sregs[i]), sizeof(struct RegItem));

        inst2issue = Sinst2issue;
        AdderCompNum = SAdderCompNum;
        MultCompNum = SMultCompNum;
        LoadCompNum = SLoadCompNum;
		savepointid = Ssavepointid;
    }

    void remove() {
        StatusRecord* pre = prev;
        StatusRecord* net = next;
        prev->next = net;
        net->prev = pre;
    }

    void insert(StatusRecord* pre, StatusRecord* net) {
        pre->next = this;
        net->prev = this;
        this->next = net;
        this->prev = pre;
    }

    ~StatusRecord() {
        Sinsts.clear();
        Sboard.clear();
        Sstation.clear();
        Sregs.clear();
    }
};

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

void showlink()
{
    StatusRecord* p = head;
    cout << "Show Link" << endl;
    do{
        cout << p << endl;
        if(p == NULL)
            break;
        p = p->next;
    }while(p != head);
}

StatusRecord* saveStatus(int inst) {
    cout << "Save status at cycle " << cycle << endl;
    cout << "instid = " << inst << endl;
    showlink();
    //displayAll();
    StatusRecord* s = new StatusRecord();
    s->save(inst);
    if(head == NULL) {
        head = s;
        s->insert(head, head);
    }
    else
        s->insert(head->prev, head);
    showlink();
    saveAll();
    return s;
}

void restoreStatus(StatusRecord* jumpstatus) {
    cout << "Restore status at cycle " << cycle << endl;
    cout << "Before, inst = " << inst2issue << endl;
    showlink();
    restoreAll(jumpstatus->Ssavepointid);
    //displayAll();
    StatusRecord* savehead = head;
    StatusRecord* prev = jumpstatus->prev;
    prev->next = head;
    head->prev = prev;
    jumpstatus->restore();
    cout << "After, inst = " << inst2issue << endl;
    //displayAll();
    if(jumpstatus == head)
        head = NULL;
    StatusRecord* p = jumpstatus;
    StatusRecord* q = jumpstatus->next;
    while(q != savehead) {
        cout << "in restore, p = " << p << endl;
        delete p;
        cout << 66666666666666666 << endl;
        p = q;
        q = q->next;
    }
    showlink();
}

void removeStatus(StatusRecord* jumpstatus) {
    cout << "Remove status at cycle " << cycle << endl;
    cout << "instid = " << inst2issue << endl;
    showlink();
    //displayAll();
    if(jumpstatus == head){
        if(jumpstatus->next == head)
            head = NULL;
        else
            head = jumpstatus->next;
    }
    jumpstatus->remove();
    cout << jumpstatus << endl;
    delete jumpstatus;
    cout << 666666666 << endl;
    showlink();
}

void saveAll() {
    StatusRecord* p = head;
    StatusRecord* q;
    StatusRecord* shead = NULL;
    do{
        q = new StatusRecord();
        q->copyfrom(p);
        if(shead == NULL)
            shead = q;
        p = p->next;
    }while(p != head);
    savepoint.push_back(head);
    savepointid++;
    head = shead;
}

void restoreAll(int id) {
    StatusRecord* shead = savepoint[id];
    removeAll(head);
    head = shead;
    savepointid++;
    for(int i = id; i < savepoint.size(); i++)
        removeAll(savepoint[i]);
    savepoint.erase(savepoint.begin() + id, savepoint.end());
}

void removeAll(StatusRecord* hd) {
    StatusRecord* p = hd;
    StatusRecord* q = NULL;

}
