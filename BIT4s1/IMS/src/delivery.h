#ifndef DELIVERY_H
#define DELIVERY_H

#include <simlib.h>
#include <iostream>
#include <vector>

#include "van_working.h"
#include "depo.h"



using namespace std;

/*
* Třída reprezentující směnu práci dodávek od 8 do 16
*/
class Delivery : public Process
{
    private:
        Depo *depo;

        int *shiftState;

        /*
        * Sklad dodávek
        */
        Store *vans_working;

        /*
        * Statistika určující čas cesty na box
        */
        Stat *vanBoxRideTime;

        /*
        * Statistika určující čas cesty na klasickou výdejnu
        */
        Stat *vanNonBoxRideTime;

        /*
        * Statistika určující čas výkládání zásilky na výdejně/boxu
        */
        Stat *vanCanceledRemainingTime;


        Stat *vanReturnRideTime;

        
    public:
        /*
        * Kontruktor
        */
        Delivery(Depo *depo);

        /*
        * Desktruktor
        */
        ~Delivery();

        /*
        * behavior metoda
        */
        void Behavior() override;
};

#endif