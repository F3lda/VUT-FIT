#include "van_done.h"

#include "common.h"



using namespace std;

VanDone::VanDone(
	Store *vans_working,
    int shiftCanceled,
    Stat *vanReturnRideTime
) :
	vans_working(vans_working),
	shiftCanceled(shiftCanceled),
    vanReturnRideTime(vanReturnRideTime)
{
}

void VanDone::Behavior()
{
    // návrat ze z normální nebo zrušené směny
    if(shiftCanceled == 0) {
        double rideTime = Uniform(CONST_VAN_RETURN_RIDE_TIME_MIN, CONST_VAN_RETURN_RIDE_TIME_MAX);
		(*vanReturnRideTime)(rideTime);
		Wait(rideTime);
    } else {
        double rideTime = Uniform(CONST_VAN_CANCELED_RIDE_TIME_MIN, CONST_VAN_CANCELED_RIDE_TIME_MAX);
		(*vanReturnRideTime)(rideTime);
		Wait(rideTime);
    }
}

VanDone::~VanDone() {
	//cout << "VAN AT DEPO! At time: " << Time  << endl;
	Leave(*vans_working, 1);
}
