#include "pvm3.h"
//#include <pvm3pp>
#include <iostream>
#include <string>

using namespace std;


class Lol2 {

private:
    string lol3 = "pokaz lol 4";

public:
    string getlol() {
        return this->lol3;
    }


};


int main()
{

        Lol2 * lol2 = new Lol2();

        cout <<   lol2->getlol();

        int tids[4];/*tablica z identyfikatorami procesów slave*/
        int nproc;
            nproc=pvm_spawn("slave", NULL, PvmTaskDefault, \
                                "", 8, tids);

        pvm_exit();

        return 0;

//        int mytid;
//    int tids[4];/*tablica z identyfikatorami procesów slave*/
//    int nproc;
//        nproc=pvm_spawn("slave", NULL, PvmTaskDefault, \
//                                "", 8, tids);
//        pvm_exit();
}
