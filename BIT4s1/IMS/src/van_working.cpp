#include "van_working.h"

#include "delivery_timer.h"
#include "van_done.h"
#include "common.h"



using namespace std;

VanWorking::VanWorking(
	Store *vans_working,
	int *state,
	void *delivery_timer,
    Stat *vanBoxRideTime,
    Stat *vanNonBoxRideTime,
    Stat *vanCanceledRemainingTime,
    Stat *vanReturnRideTime
) :
	vans_working(vans_working),
	state(state),
	delivery_timer(delivery_timer),
	vanBoxRideTime(vanBoxRideTime),
	vanNonBoxRideTime(vanNonBoxRideTime),
	vanCanceledRemainingTime(vanCanceledRemainingTime),
	vanReturnRideTime(vanReturnRideTime)
{
	((DeliveryTimer *)delivery_timer)->addVan(this);
}


void VanWorking::Behavior()
{
	// Box nebo ne
	if( Random() < CONST_VAN_RIDE_BOX_PROBABILITY ) {
		rideTime = Uniform(CONST_VAN_RIDE_NOBOX_MIN, CONST_VAN_RIDE_NOBOX_MAX);
		(*vanNonBoxRideTime)(rideTime);
		Wait(rideTime);
	} else {
		rideTime = Uniform(CONST_VAN_RIDE_BOX_MIN, CONST_VAN_RIDE_BOX_MAX);
		(*vanBoxRideTime)(rideTime);
		Wait(rideTime);
	}
}

VanWorking::~VanWorking() {
	// is shift done?
	if (*state == 0) {
		((DeliveryTimer *)delivery_timer)->deleteVan(this);
	}

	if (*state == 1) {
		(*vanCanceledRemainingTime)(rideTime-Time);

		//cout << "VAN SHIFT CANCELED! At time: " << Time  << endl;
		(new VanDone(vans_working, 1, vanReturnRideTime))->Activate();
	} else {
		//cout << "VAN DONE IN TIME! At time: " << Time  << endl;
		(new VanDone(vans_working, 0, vanReturnRideTime))->Activate();
	}
}
