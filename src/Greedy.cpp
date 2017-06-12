/****************************************************************************
  FileName     [ Steiner.cpp ]
  Synopsis     [ Implement main algorithm ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <omp.h>
using namespace std;

void CircuitMgr::greedy()
{
   #pragma omp parallel for
   for (int layer=1; layer<=_layernum; ++layer)
   {
      Graph* g = buildGraph(layer);
      if (g==0) continue;
      if (g->_nodes.size()>0)
      {
         mstPrim(g);
         // run other methods to make single set
      }
      delete g;
   }
   // deal with connection between different layers(via)
   // final optimization (ex. remove redundant lines; replace a long line with two vias)
}
