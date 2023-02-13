#ifndef TRUCK_H
#define TRUCH_H

#include <iostream>
#include <simlib.h>



class Truck : public Process {
    private:
        Store *trucks;
        Stat *TRUCK_TRAVEL_BETWEEN_DEPOS_TIME;
        Stat *TRUCK_WORK_BETWEEN_DEPOS_TIME;
        Stat *TRUCK_WORK_AT_LAST_DEPO_TIME;
        Stat *trucksRideLength;

    public:
        Truck(Store *trucks, Stat *trucksRideLength);

        void Behavior();
};

#endif
