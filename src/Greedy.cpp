/****************************************************************************
  FileName     [ Greedy.cpp ]
  Synopsis     [ Implement main algorithm ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
using namespace std;

void CircuitMgr::greedy()
{
   for (int layer=0; layer<_layernum; ++layer)
   {
      Graph* g = buildGraph(layer);
      mst(g);
      // run other methods to make single set
      delete g;
   }
   // deal with connection between different layers(via)
   // final optimization (ex. remove redundant lines; replace a long line with two vias)
}
