//
// Created by SeventhHeaven on 2019-06-05.
//

#include "RegItem.h"
#include "util.h"

void RegItem::showTitle() {
    cout << "RegID\tAddr" << endl;
}

void RegItem::show() {
    //cout << id << '\t';
    if(srcid != -1) {
        cout << station[srcid].getCompName();
    }
    else {
        if(hexflag)
            cout << hex << value << dec;
        else
            cout << value;
    }
}