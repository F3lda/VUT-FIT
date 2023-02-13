#ifndef DEPO_H
#define DEPO_H

#include <simlib.h>
#include "truck_work.h"



/*
* Třída reprezentující práci výkládání zásilek z dodávek v Depu
*/
class Depo : public Process {
    private:
        /*
        * Sklad dodávek
        */
        Store *vans;

        /*
        * Sklad zaměstnanců
        */
        Store *workers;

        /*
        * Sklad balíků
        */
        Store *trucks;

        TruckWork *truckWork;

        unsigned long *unloaded_packages;
        unsigned long *loaded_packages;

        double StartTime;
        
    public:
        Depo(TruckWork *truckWork);

        ~Depo();

        void Behavior() override;
};

#endif
