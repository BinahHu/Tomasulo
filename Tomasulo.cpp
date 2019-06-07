#include "util.h"

bool init(char* inputfile) {
    cout.setf(ios::showbase);
    cin.setf(ios::showbase);
    ifstream fin(inputfile);
    if(!fin.is_open()) {
        cout << "Open  file error!" << endl;
        return false;
    }

    string line;
    const char* sep  = ",";
    while(getline(fin, line)) {
        char src[line.size() + 1];
        strcpy(src, line.c_str());
        char* p = strtok(src, sep);
        vector<string> seg;
        while(p) {
            seg.push_back(string(p));
            p = strtok(NULL, sep);
        }
        parseInst(seg);
    }

    regs.resize(MREG);
    for(int i = 0; i < regs.size(); i++)
        regs[i].id = i;

    station.resize(MSTA);
    for(int i = 0; i < station.size(); i++)
        station[i].id = i;

    return true;
}

bool save(char* outputfile) {
    ofstream fout(outputfile);
    if(!fout.is_open()) {
        cout << "Open output file error!" << endl;
        return false;
    }
    return true;
}

bool Issue(int id) {
    if(id >= insts.size())
        return false;
    int stationid, i;
    Instruction& inst = insts[id];
    switch(inst.type) {
        default:
        case UNDEF:
            cout << "ERROR! Undefined Inst!" << endl;
            return false;
        case ADD:
        case SUB:
        case JUMP:
            for(i = ASS; i < ASE; i++)
                if(!station[i].busy) {
                    stationid = i;
                    station[i].type = ADDER;
                    break;
                }
            if(i == ASE)
                return false;
            break;
        case MUL:
        case DIV:
            for(i = MSS; i < MSE; i++)
                if(!station[i].busy) {
                    stationid = i;
                    station[i].type = MULTER;
                    break;
                }
            if(i == MSE)
                return false;
            break;
        case LD:
            for(i = LSS; i < LSE; i++)
                if(!station[i].busy) {
                    stationid = i;
                    station[i].type = LOADER;
                    break;
                }
            if(i == LSE)
                return false;
            break;
    }
    StationItem& s = station[stationid];
    s.busy = true;
    s.status = WAITTING;
    s.instid = id;

    if(inst.vj.type == REG) {   //Register
        int regid = inst.vj.value;
        if(regs[regid].srcid == -1) {
            s.vj = regs[regid].value;
            s.qj = -1;
        }
        else {
            s.qj = regs[regid].srcid;
        }
    }
    else {  //Instant Number
        s.vj = inst.vj.value;
        s.qj = -1;
    }

    if(inst.vk.type == REG) {
        int regid = inst.vk.value;
        if(regs[regid].srcid == -1) {
            s.vk = regs[regid].value;
            s.qk = -1;
        }
        else {
            s.qk = regs[regid].srcid;
        }
    }
    else {
        s.vk = inst.vk.value;
        s.qk = -1;
    }

    s.dest = inst.dest.value;
    if(inst.dest.type == REG) { //Normal instruction
        int regid = inst.dest.value;
        regs[regid].srcid = stationid;
    }
    else {  //Jump instruction

    }

    if(insts[id].type == LD) {
        // assert: inst.vj.type must be INT!
        s.addr = inst.vj.value;
    }
    if(insts[id].type == JUMP) {
		int inst1, inst2;
		if(s.jumphis / 2 == 0) {    //Predict failed
            inst1 = inst2issue + s.vk - 1;
            inst2 = inst2issue;
		}
		else {  //Predict success
		    inst1 = inst2issue;
		    inst2 = inst2issue + s.vk - 1;
		}
		s.jumpstatus = saveStatus(inst1);
        inst2issue = inst2;
    }

    updateBoard(id, 0, cycle);

    if(debugflag)
        cout << "Issue Instruction "<< id << endl;
    return true;


}

int Writeback(int id) {
    int ret = OK;
    StationItem& s = station[id];
    if(debugflag)
        cout << "Station " << id << " Write back" << endl;
    s.exec();
    if(insts[s.instid].type == JUMP) {
        bool restore1 = (s.dest != s.vj && s.jumphis / 2 != 0); //Predict success but actually failed
        bool restore2 = (s.dest == s.vj && s.jumphis / 2 == 0); //Predict failed but acutally success
        if(restore1 || restore2) {    //Need to recover
            int boardid = insts[s.instid].boardid;
            int iTime = board[boardid].IssueTime;
            int eTime = board[boardid].ExecCompTime;
            restoreStatus(s.jumpstatus);
            updateBoard(s.instid, 0, iTime);
            updateBoard(s.instid, 1, eTime);
            inst2issue++;
            ret = RESTORE;

            //Update jump history
            if(s.jumphis / 2 == 0)
                s.jumphis = s.jumphis * 2 + 1;
            else
                s.jumphis = (s.jumphis) * 2 % 4;
        } else {
            removeStatus(s.jumpstatus);
            if(s.jumphis / 2 == 0)
                s.jumphis = 0;
            else
                s.jumphis = 3;
        }
    }
    else {
        for(int i = 0; i < regs.size(); i++) {
            if(regs[i].srcid == id) {
                regs[i].value = s.dest;
                regs[i].srcid = -1;
            }
        }
        for(int i = 0; i < MSTA; i++)
            station[i].WriteBackCheck(id);
    }
    updateBoard(s.instid, 2, cycle);

    station[id].busy = false;
    return ret;

}
void Exec(int index) {
    if(debugflag)
        cout << "Station " << index << " Exec" << endl;
    StationItem& s = station[index];
    s.status = RUNNING;
    s.remaincycle = INSTTIME[insts[s.instid].type];
    if(insts[s.instid].type == DIV && s.vk == 0) {
        s.vk = 1;
        s.remaincycle = 1;
    }
}

void tick() {
    cycle++;
    if(debugflag)
        cout << "cycle " << cycle << endl << endl;

    bool restoreflag = false;

    //Pharse 1: Write back
    for(int i = 0; i < MSTA; i++) {
        StationItem& s = station[i];
        if(s.busy && s.status == OVER)
            if(Writeback(i) == RESTORE) {
                restoreflag = true;
                break;
            }
    }

    //Phrase 2: Issue
    if(restoreflag)
        restoreflag = false;
    else if(Issue(inst2issue))
        inst2issue++;

    //Phrase 3: Update exec status, check over
    for(int i = 0; i < MSTA; i++)
        if(station[i].busy)
            station[i].update();


    //Phrase 4: Check for currently issued insts
    for(int i = 0; i < MSTA; i++) {
        StationItem& s = station[i];
        if(s.busy && s.ready() && getComp(i)) {
            Exec(i);
        }
    }

}

void Tomasulo() {
    displayInstruction();

    cycle = 0;
    inst2issue = 0;
    //displayAll();
    //tick();
    //displayAll();

    int flag = 1;
    bool breakflag = false;
    if(flag) {
        char c[1024];
        while(1) {
            if(breakflag)
                break;
            cout << "(Tomasulo)";
            cin.getline(c, 1024);
            UserInst inst = parseUser(c);
            switch(inst.type) {
                case 0:
                    break;
                case 1:
                    if(!checkover())
                        tick();
                    break;
                case 2:
                    while(!checkover())
                        tick();
                    break;
                case 3:
                    display(inst.disp);
                    break;
                case 4:
                    breakflag = true;
                    break;
                case 5:
                default:
                    showhelp();
                    break;
            }
        }
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "Usage: tomasulo inputfile" << endl;
        return 0;
    }

    if(!init(argv[1])) {
        cout << "Init error!" << endl;
        return 0;
    }

    Tomasulo();

    return 0;
}
