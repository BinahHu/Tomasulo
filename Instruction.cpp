//
// Created by SeventhHeaven on 2019-06-05.
//

#include "Instruction.h"
#include "util.h"

void Instruction::showTitle() {
    cout << "InstID\tOP\tDest\tVj\tVk" << endl;
}

void Instruction::show() {
    cout << id << '\t';
    cout << INSTYPENAME[type] << '\t';
    dest.show();
    cout <<  '\t';
    vj.show();
    cout << '\t';
    vk.show();
}