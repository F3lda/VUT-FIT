#include "van.h"
#include "common.h"

Van::Van(Store *vans) {
    this->vans = vans;
    this->LOAD_TIME = new Stat("Van load time");
}

void Van::Behavior() {
    double depoLoadTime = Uniform(CONST_VAN_LOAD_TIME_MIN, CONST_VAN_LOAD_TIME_MAX);
    (*LOAD_TIME)(depoLoadTime);
    Wait(depoLoadTime);

    Leave(*vans, 1);
} 