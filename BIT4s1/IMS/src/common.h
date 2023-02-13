#ifndef COMMON_H
#define COMMON_H

/**
 * Soubor obsahující konstanty pro simulaci
 */

// Počet pokusů simulace
#define CONS_ATTEMPTS_COUNT 5

// Počet dodávek v depu
#define CONST_VANS 13

// Pravděpodobnost jízdy dodávky na H-box
#define CONST_VAN_RIDE_BOX_PROBABILITY 0.695

// Minimální délka jízdy na kamenné výdejní místo
#define CONST_VAN_RIDE_NOBOX_MIN 452

// Maximální délka jízdy na kamenné výdejní místo
#define CONST_VAN_RIDE_NOBOX_MAX 537

// Minimální délka jízdy na H-box
#define CONST_VAN_RIDE_BOX_MIN 447

// Maximální délka jízdy na H-box
#define CONST_VAN_RIDE_BOX_MAX 532

//Minimální délka jízdy zpět na depo
#define CONST_VAN_RETURN_RIDE_TIME_MIN 10

// Maximální délka jízdy zpět na depo
#define CONST_VAN_RETURN_RIDE_TIME_MAX 15

// Minímální délka jízdy při nuceném návratu ze směny
#define CONST_VAN_CANCELED_RIDE_TIME_MIN 15

// Maximální délka jízdy při nuceném návratu ze směny
#define CONST_VAN_CANCELED_RIDE_TIME_MAX 45

// Délka směny řidičů dodávek
#define CONST_VANS_SHIFT_LENGTH (8.0 * 60.0)

// Kapacita dodávek pro balíky
#define CONST_VAN_CAPACITY 157

// Minimální čas nakládání balíku do všech dodávek v depu
#define CONST_VAN_LOAD_TIME_MIN 74

// Maximální čas nakládání balíku do všech dodávek v depu
#define CONST_VAN_LOAD_TIME_MAX 84

// Počet zaměstnanců v depu
#define CONST_EMPLOYEES 26

// Minimální čas nákladu jednoho balíku zaměstnancem
#define CONST_EMPLOYEE_PACKET_LOAD_TIME_MIN 0.30
// Maximální čas nákladu jednoho balíku zaměstnancem
#define CONST_EMPLOYEE_PACKET_LOAD_TIME_MAX 0.35
// Minimální čas výkladu jednoho balíku zaměstnancem
#define CONST_EMPLOYEE_PACKET_UNLOAD_TIME_MIN 0.45
// Maximální čas výkladu jednoho balíku zaměstnancem
#define CONST_EMPLOYEE_PACKET_UNLOAD_TIME_MAX 0.50

// Počet kamiónů v depu
#define CONST_TRUCKS 4
// Kapacita kamiónu
#define CONST_TRUCK_CAPACITY (CONST_VAN_CAPACITY*6)
// Minimální cesta kamiónu mezi depy
#define CONST_TRUCK_TRAVEL_TIME_MIN 45
// Maximální cesta kamiónu mezi depy
#define CONST_TRUCK_TRAVEL_TIME_MAX 480

// Minimální délka jízdy kamionu zpět na depo
#define CONST_TRUCK_WORK_ON_WAY_TIME_MIN 0
// Maximální délka jízdy kamionu zpět na depo
#define CONST_TRUCK_WORK_ON_WAY_TIME_MAX 562
// Minimální délka výkladu kamionu
#define CONST_TRUCK_UNLOAD_TIME_MIN 107
// Maximální délka výkladu kamionu
#define CONST_TRUCK_UNLOAD_TIME_MAX 117



#define CONST_WORKING_SHIFT_LENGTH (24*60)



#endif
