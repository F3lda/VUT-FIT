#ifndef VAN_WORKING_H
#define VAN_WORKING_H

#include <iostream>
#include <simlib.h>


/*
* Třída reprezentující práci jedné dodávky při vykládání zásilek z dodávky
*/
class VanWorking : public Process {
    private:
        Store *vans_working;

        int *state;

        void *delivery_timer;

        /*
        * Statistika určující čas cesty na box
        */
        Stat *vanBoxRideTime;

        /*
        * Statistika určující čas cesty na kamenné výdejní místo
        */
        Stat *vanNonBoxRideTime;

        /*
        * Statistika určující čas při návratu do dopa po konce směny
        */
        Stat *vanCanceledRemainingTime;

        /*
        * Statistika určující čas při návratu do depa
        */
        Stat *vanReturnRideTime;

        double rideTime = 0;

    public:
        VanWorking(
            Store *vans_working,
            int *state,
            void *delivery_timer,
            Stat *vanBoxRideTime,
            Stat *vanNonBoxRideTime,
            Stat *vanCanceledRemainingTime,
            Stat *vanReturnRideTime
        );

        void Behavior() override;

        ~VanWorking();
};

#endif
