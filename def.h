//
// Created by SeventhHeaven on 2019-06-05.
//

#ifndef EXPERIMENT2_DEF_H
#define EXPERIMENT2_DEF_H

//Include files
#include <vector>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;


//enums
enum INSTYPE{
    LD = 0,
    JUMP,
    ADD,
    SUB,
    MUL,
    DIV,
    UNDEF
};
enum VARTYPE{
    REG = 0,
    INT,
    EMPTY
};
enum STATIONSTATUS{
    WAITTING = 0,
    RUNNING,
    READY,
    OVER
};
enum STATIONTYPE {
    ADDER,
    MULTER,
    LOADER
};

//global vars
const int INSTTYPENUM = 6;


const int MREG = 32;
//const int MREG = 16;

const int MAC = 3;
const int MMC = 2;
const int MLC = 2;

const int ASS = 0;  // Adder Station Start pos
const int ASE = 6;  // Adder Station End pos
const int MSS = 6;
const int MSE = 9;
const int LSS = 9;
const int LSE = 12;

const int MSTA = 12;

const bool debugflag = true;
const bool hexflag = false;

#define OK 0
#define RESTORE 1




#endif //EXPERIMENT2_DEF_H
