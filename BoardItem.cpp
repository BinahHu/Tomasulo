//
// Created by SeventhHeaven on 2019-06-05.
//

#include "BoardItem.h"
#include "util.h"

void BoardItem::showTitle() {
    cout << "InstID\tIssueTime\tExecCompTime\tWriteResultTime" << endl;
}

void BoardItem::show() {
    cout << instid << "\t" << IssueTime << "\t\t" << ExecCompTime << "\t\t" << WriteResultTime;
}