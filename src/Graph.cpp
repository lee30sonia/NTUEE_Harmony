/****************************************************************************
 FileName     [ Graph.cpp ]
 Synopsis     [ Implementation of graph ]
 ****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <algorithm>

using namespace std;

/********************Edge*********************/

Edge::Edge(Node *a, Node *b, const int& w)
{
   _node[0]=a;
   _node[1]=b;
   _weight=w;
}

/********************Node*********************/

/********************Graph*********************/
Graph::~Graph()
{
   for (int i=0; i<_nodes.size(); ++i)
      delete _nodes.at(i);
   for (int i=0; i<_edges.size(); ++i)
      delete _edges.at(i);
}

void Graph::addEdge(Node* n1, Node* n2, int& weight)
{
   Edge* e = new Edge(n1,n2,weight);
   _edges.push_back(e);
}
void Graph::addEdge(Obj* o1, Obj* o2, int& weight)
{
   Node *n1=0, *n2=0;
   for (int i=0; i<_nodes.size(); ++i)
   {
      if (_nodes.at(i)->_obj==o1)
         n1=_nodes.at(i);
      else if (_nodes.at(i)->_obj==o2)
         n2=_nodes.at(i);
   }
   if (!n1)
   {
      n1=new Node(o1);
      _nodes.push_back(n1);
   }
   if (!n2)
   {
      n2=new Node(o2);
      _nodes.push_back(n2);
   }
   addEdge(n1,n2,weight);
}

/********************CircuitMgr*********************/




