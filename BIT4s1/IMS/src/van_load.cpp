#include "van_load.h"

#include "common.h"



using namespace std;

VanLoad::VanLoad() {
    this->vans = new Store("Sklad dodávek", CONST_VANS);
    this->loadTime = new Stat("Čas nakládání");
}

void VanLoad::Behavior() {
    cout << "\nSimulating package loading at depo.\n" 
        << "=======================================================================\n"
		<< "Van load at depo shift started.\n"
		<< "\tStart time: " << Time/60 << " hours\n"
		<< "\tNumber of vans: " << this->vans->Capacity() << ".\n"
		<< endl;
    int vans_count = vans->Capacity();
    while(vans_count > 0) {
        Enter(*vans, 1);
        vans_count--;
        
        (new Van(vans))->Activate();
    }

    Enter(*vans, vans->Capacity());
    Leave(*vans, vans->Capacity());
}

VanLoad::~VanLoad() {
    cout << "Van load at depo shift ended.\n"
		<< "\tEnd time: " << Time/60 << " hours.\n" 
        << "=======================================================================\n"
		<< endl;



    if (Time < CONST_WORKING_SHIFT_LENGTH) {
        cout << "WORKING SHIFT FINISHED IN TIME \n(" << Time/60 << " h < " << CONST_WORKING_SHIFT_LENGTH/60 << " h) [" << CONST_WORKING_SHIFT_LENGTH - Time << " minutes before end]\n" << endl;
    } else {
        cout << "WORKING SHIFT NOT FINISHED ON TIME \n(" << Time/60 << " h > " << CONST_WORKING_SHIFT_LENGTH/60 << " h) [" << Time - CONST_WORKING_SHIFT_LENGTH << " minutes after end]\n" << endl;
    }
}
