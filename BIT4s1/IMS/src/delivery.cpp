#include "delivery.h"

#include "delivery_timer.h"
#include "common.h"



Delivery::Delivery(Depo *depo) {

    this->depo = depo;

    this->shiftState = new int();
    this->vans_working = new Store("Van store", CONST_VANS);


    vanBoxRideTime = new Stat("Van ride distance to Box");
    vanNonBoxRideTime = new Stat("Van ride distance to Deli point");
    vanCanceledRemainingTime = new Stat("Remaining time when shift canceled");
    vanReturnRideTime = new Stat("Return to depot");
    // Počet van který to stihly a nestihly (s boxem a bez boxu)
    // zbývající čas pokud to nestihly (+ průměrný zbývající čas)
    // průměrný čas návratu


    cout << "=======================================================================\n"
		<< "Delivery shift started.\n"
		<< "\tStart time: " << Time << ".\n"
		<< "\tNumber of vans: " << this->vans_working->Capacity() << ".\n"
    << "-----------------------------------------------------------------------\n"
		<< endl;
}

void Delivery::Behavior() {

    DeliveryTimer *deliveryTimer = new DeliveryTimer(shiftState);
   
    // Next event
    int vans_started = vans_working->Capacity();
    while(vans_started > 0) {
        vans_started--;
        
        Enter(*vans_working, 1);
        //cout << "Van Start" << endl;

        (new VanWorking(vans_working, shiftState, deliveryTimer, vanBoxRideTime, vanNonBoxRideTime, vanCanceledRemainingTime, vanReturnRideTime))->Activate();
        
    }

    Enter(*vans_working, vans_working->Capacity());
    Leave(*vans_working, vans_working->Capacity());
    // Konec směny dodávek
    // Začátek směny u depa
    delete deliveryTimer;
}

Delivery::~Delivery() {

    delete shiftState;
    delete vans_working;


    cout << "-----------------------------------------------------------------------\n"
		<< "Delivery shift end.\n"
		<< "\tEnd Time: " << Time << ".\n"
		<< "\tTotal van ride distance to Box: " << vanBoxRideTime->Sum() << " minutes.\n"
    << "\tTotal van ride distance to Deli Point: " << vanNonBoxRideTime->Sum() << " minutes.\n"
    << "======================================================================="
		<< endl;
    
    //cout << "=======================================================================\n" << endl;
    vanBoxRideTime->Output();
    //cout << "=======================================================================\n" << endl;
    vanNonBoxRideTime->Output();
    //cout << "=======================================================================\n" << endl;
    vanCanceledRemainingTime->Output();
    //cout << "=======================================================================\n" << endl;
    vanReturnRideTime->Output();
    //cout << "=======================================================================" << endl;
    
    delete vanBoxRideTime;
    delete vanNonBoxRideTime;
    delete vanCanceledRemainingTime;
    delete vanReturnRideTime;
    


    // Start Depo Shift
    depo->Activate();
}
