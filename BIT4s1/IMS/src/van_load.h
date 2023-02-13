#ifndef VANLOAD_H
#define VANLOAD_H

#include <iostream>
#include <simlib.h>
#include "van.h"


/*
* Třída reprezentující práci kamionů po výkladu balíků v jednotlivých depech (od 16:00 do 8:00)
*/
class VanLoad : public Process {
    private:
        Store *vans;
        Stat *loadTime;

    public:
        VanLoad();
        void Behavior();
        ~VanLoad();
};

#endif
