#include "worker.h"
#include "common.h"

using namespace std;



Worker::Worker(Store *vehicles, Store *workers, long unsigned int *packages, int unloading) {
    this->vehicles = vehicles;
    this->workers = workers;
    this->packages = packages;
    this->unloading = unloading;
}

void Worker::Behavior() {
    if (unloading == 1) {
        double unloadTime = Uniform(CONST_EMPLOYEE_PACKET_UNLOAD_TIME_MIN, CONST_EMPLOYEE_PACKET_UNLOAD_TIME_MAX);
        Wait(unloadTime);

        (*packages)++;

        // právě je jedna dodávka úplně vyložena
        if ((*packages) == CONST_VAN_CAPACITY) {
            (*packages) = 0;
            Leave(*vehicles, 1);

            //cout << "Van fully unloaded" << endl;
        }
    } else {
        double loadTime = Uniform(CONST_EMPLOYEE_PACKET_LOAD_TIME_MIN, CONST_EMPLOYEE_PACKET_LOAD_TIME_MAX);
        Wait(loadTime);

        (*packages)++;

        // právě je jedna dodávka úplně vyložena
        if ((*packages) == CONST_TRUCK_CAPACITY) {
            (*packages) = 0;
            Leave(*vehicles, 1);

            //cout << "Truck fully loaded" << endl;
        }
    }


    Leave(*workers, 1);
}
