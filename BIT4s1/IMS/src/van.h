#ifndef VAN_H
#define VAN_H

#include <iostream>
#include <simlib.h>


/*
* Třída reprezentující dodávku při nakládání
*/
class Van : public Process {
    private:
        Store *vans;

        /*
        * Statistika určující čas nakládání všech zásilek do dodávek
        */
        Stat *LOAD_TIME;

    public:
        Van(Store *vans);

        void Behavior();
};

#endif