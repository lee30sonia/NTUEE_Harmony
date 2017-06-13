/****************************************************************************
  FileName     [ Steiner.cpp ]
  Synopsis     [ Implement main algorithm ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <vector>

#ifndef _DEBUG_ON
#include <omp.h>
#endif
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
         vector<Node*> roots = mstPrim(g);
         #ifdef _DEBUG_ON
         cout<<"#disjoint set: "<<roots.size()<<endl;
         #endif
         
         for (int i=0; i<g->_nodes.size(); ++i)
         {
            Node* n = g->_nodes.at(i);
            if (n->_pi==0) //root
               continue;
            if (n->_connectEdge->_weight==0) //already overlap
               continue;
            if (!addLine(n->_connectEdge->_connect[0],n->_connectEdge->_connect[1],layer))
               cout<<"Error: trivial connection addLine failed!"<<n->_connectEdge->_connect[0].str()<<n->_connectEdge->_connect[1].str()<<endl;
         }
         // run other methods to make single set
      }
      delete g;
   }
   // deal with connection between different layers(via)
   // final optimization (ex. remove redundant lines; replace a long line with two vias)
}
