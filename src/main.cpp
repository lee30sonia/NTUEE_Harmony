/****************************************************************************
  FileName     [ main.cpp ]
  Synopsis     [ main program ]
****************************************************************************/
#include "Circuit.h"
#include "../lib/tm_usage.h"
#include <iostream>

int main(int argc, char* argv[])
{
   if(argc != 3) {
      cout << "usage:net_open_finder <input.txt> <output.txt>" << endl;
      return 0;
   }
   CommonNs::TmUsage tmusg;
   CommonNs::TmStat stat;

   CircuitMgr ccMgr;
   tmusg.periodStart();
   ccMgr.readCircuit(argv[1]);
   ccMgr.greedy();

   tmusg.getPeriodUsage(stat);
   cout << "Total CPU time: " << (stat.uTime + stat.sTime) / 1000.0 << "ms" << endl;
   cout << "memory: " << stat.vmPeak / 1024.0 << "MB" << endl;
   ccMgr.writeOutput(argv[2]);

   return 0;
}
