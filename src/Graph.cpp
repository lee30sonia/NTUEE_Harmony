/****************************************************************************
  FileName     [ Graph.cpp ]
  Synopsis     [ Implementation of graph ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <algorithm>

using namespace std;

/********************CircuitMgr*********************/
void CircuitMgr::buildGraph(int layer)
{
    Graph g;
    vector<Shape>* shapes = getShapesByLayer(layer);
    sort(shapes->begin(), shapes->end(), compareByX);
    for (int i=0; i<shapes.size(); ++i)
    {
        for (int j=i; j<shapes.size(); ++j)
        {
            if (shapes.at(j).LL)
        }
    }
}
