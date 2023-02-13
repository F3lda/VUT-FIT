#ifndef VAN_DONE_H
#define VAN_DONE_H

#include <iostream>
#include <simlib.h>


/*
* Třída reprezentující práci jedné dodávky při vykládání zásilek z dodávky
*/
class VanDone : public Process {
    private:
        Store *vans_working;
        int shiftCanceled;
        Stat *vanReturnRideTime;

    public:
        VanDone(
            Store *vans_working,
            int shiftCanceled,
            Stat *vanReturnRideTime
        );

        void Behavior() override;

        ~VanDone();
};

#endif
