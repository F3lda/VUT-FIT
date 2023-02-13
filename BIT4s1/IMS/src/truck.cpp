#include "truck.h"

#include "common.h"



using namespace std;

Truck::Truck(Store *trucks, Stat *trucksRideLength) {
    this->trucks = trucks;
    this->trucksRideLength = trucksRideLength;
    TRUCK_TRAVEL_BETWEEN_DEPOS_TIME = new Stat("Cesta kamionů přes další depa");
    TRUCK_WORK_BETWEEN_DEPOS_TIME = new Stat("Překlad na depech na trase");
    TRUCK_WORK_AT_LAST_DEPO_TIME = new Stat("Vykládání na posledním Depu");
}

void Truck::Behavior() {
    double startTime = Time;
    double truckTravelTime = Uniform(CONST_TRUCK_TRAVEL_TIME_MIN, CONST_TRUCK_TRAVEL_TIME_MAX);
    (*TRUCK_TRAVEL_BETWEEN_DEPOS_TIME)(truckTravelTime);
    Wait(truckTravelTime);
    double truckWorkBetween = Uniform(CONST_TRUCK_WORK_ON_WAY_TIME_MIN, CONST_TRUCK_WORK_ON_WAY_TIME_MAX);
    (*TRUCK_WORK_BETWEEN_DEPOS_TIME)(truckWorkBetween);
    Wait(truckWorkBetween);
    double truckWorkLast = Uniform(CONST_TRUCK_UNLOAD_TIME_MIN, CONST_TRUCK_UNLOAD_TIME_MAX);
    (*TRUCK_WORK_AT_LAST_DEPO_TIME)(truckWorkLast);
    Wait(truckWorkLast);

    (*trucksRideLength)(Time-startTime);

    Leave(*trucks, 1);
}
