//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_UTIL_H
#define EXPERIMENT2_UTIL_H

#include "def.h"
#include "Var.h"
#include "BoardItem.h"
#include "StationItem.h"
#include "Instruction.h"
#include "RegItem.h"


extern vector<Instruction> insts;
extern vector<BoardItem> board;
extern vector<StationItem> station;
extern vector<RegItem> regs;

extern int AdderCompNum;
extern int MultCompNum;
extern int LoadCompNum;

extern int cycle;
extern int inst2issue;

extern const int INSTTIME[INSTTYPENUM];
extern const char* INSTYPENAME[INSTTYPENUM+1];
extern const char* VARTYPENUM[3];
extern const char* STATIONSTATUSNAME[4];


//UI struct
struct UserInst{
    int type;
    int disp;
    int n;
};
struct StatusRecord;
//type:
//0: skip
//1: one step
//2: continue
//3: show
//  disp:
//  0: All
//  1: Instruction
//  2: Board
//  3: Station
//  4: Regs
//4: quit
//5: help

//Function interface
void displayBoard();
void displayStation();
void displayRegister();
void displayInstruction();
void displayAll();
void showhelp();
void display(int disp);

void parseInst(vector<string>& seg);
UserInst parseUser(char* c);

bool checkover();
bool getComp(int id);
void releaseComp(int id);
void updateBoard(int instid, int status, int val);

StatusRecord* saveStatus(int inst);
void restoreStatus(StatusRecord* jumpstatus);
void removeStatus(StatusRecord* jumpstatus);
void saveAll();
void restoreAll(int id);
void removeAll(StatusRecord* hd);

#endif //EXPERIMENT2_UTIL_H
