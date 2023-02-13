#ifndef TRUCKWORK_H
#define TRUCKWORK_H

#include <iostream>
#include <simlib.h>
#include "truck.h"
#include "van_load.h"



/*
* Třída reprezentující práci kamionů po nakládání balíků do kamionů (depo.h/cpp)
*/
class TruckWork : public Process {
    private:
        Store *trucks;

        VanLoad *vanLoad;

        Stat *trucksRideLength;
    public:
        TruckWork(VanLoad *vanLoad);

        void Behavior();
        
        ~TruckWork();
};

#endif
