#include <string>
#include <simlib.h>
#include "delivery.h"
#include "depo.h"
#include "truck_work.h"
#include "van_load.h"
#include "common.h"


using namespace std;

int main() {         
  cout << "# HYPSOZ --- model of delivery service" << endl;

  for (int i = 1; i <= CONS_ATTEMPTS_COUNT; i++) {
      string filename = "data/hypsoz_" + to_string(i) + ".dat";
      SetOutput(filename.c_str());
      cout << "Trying simulation number " << i << endl;
      cout << "=======================================================================\n\n";
      cout << "Simulating delivery shift from 8 to 16" << endl;
      Init(0);

      VanLoad *vanLoad = new VanLoad();
      TruckWork *truckWork = new TruckWork(vanLoad);
      Depo *depo = new Depo(truckWork);
      Delivery *del = new Delivery(depo);
      del->Activate();
      
      Run();
      cout << "=======================================================================\n";
      cout << "Ending simulation number " << i << "\n\n" << endl;

      cout << "******************************************************************************\n"
          << "******************************************************************************\n\n";
      //SIMLIB_statistics.Output();
  }
  return 0;
}
