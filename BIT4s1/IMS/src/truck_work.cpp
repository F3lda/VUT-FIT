#include "truck_work.h"

#include "common.h"



using namespace std;

TruckWork::TruckWork(VanLoad *vanLoad) {
    this->trucks = new Store("Sklad kamionů", CONST_TRUCKS);
	this->vanLoad = vanLoad;
	trucksRideLength = new Stat("Total trucks length during shift");
}

void TruckWork::Behavior() {
    cout << "\nSimulating work of trucks.\n" 
		<< "=======================================================================\n"
		<< "Truck shift started.\n"
		<< "\tStart time: " << Time/60 << " hours.\n"
		<< "\tNumber of trucks: " << this->trucks->Capacity() << ".\n"
		<< endl;
	
	int truck_started = trucks->Capacity();
	while(truck_started) {
		truck_started--;

		Enter(*trucks, 1);
		//cout << "Truck start" << endl;

    	(new Truck(trucks, trucksRideLength))->Activate();
	}
    
    Enter(*trucks, trucks->Capacity());
    Leave(*trucks, trucks->Capacity());
}

TruckWork::~TruckWork() {

    cout << "\n"
		<< "Truck shift ended.\n"
		<< "\tEnd time: " << Time/60 << " hours.\n"
		<< "======================================================================="
		<< endl;
	trucksRideLength->Output();

	delete trucks;
	delete trucksRideLength;


	
	vanLoad->Activate();
}
