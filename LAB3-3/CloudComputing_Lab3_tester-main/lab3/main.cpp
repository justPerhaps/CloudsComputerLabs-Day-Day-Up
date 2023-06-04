#include "kvstore2pcsystem.h"
#include "coordinator.h"
#include "participant.h"
#include <iostream>
using namespace std;
int main(int argc, char * argv[])
{
    kvstore2pcsystem KV(argc, argv);
    cout << KV.getfilename() << endl; 
    cout << "mode = " << KV.getmode() << endl;
    if(KV.getmode() == 0) {
        coordinator COOR(KV.getfilename());
        COOR.start();
    }
    else if(KV.getmode() == 1) {
        participant PART(KV.getfilename());
        PART.start();
    }
    else {
        perror("mode error!!!");
        exit(0);
    }
}
