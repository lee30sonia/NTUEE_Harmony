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

void Graph::addEdge(Node* n1, Node* n2, int& weight)
{
   Edge* e = new Edge(n1,n2,weight);
   _edges.push_back(e);
}
void Graph::addEdge(Obj* o1, Obj* o2, int& weight)
{
   Node* n1=0,n2=0;
   for (int i=0; i<_nodes.size(); ++i)
   {
      if (_nodes.at(i)==o1)
         n1=_nodes.at(i);
      else if (_nodes.at(i)==o2)
         n2=_nodes.at(i);
   }
   if (!n1)
      n1=new Node(o1);
   if (!n2)
      n2=new Node(o2);
   addEdge(n1,n2,weight);
}

/********************CircuitMgr*********************/
void CircuitMgr::buildGraph(int layer)
{
   Graph g;
   vector<Shape>& shapes = _shapes.at(layer);
   sort(shapes.begin(), shapes.end(), compareByX);
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         if (shapes.at(i).overlapX(shapes.at(j)))
         {
            int d=dist(shapes.at(i),shapes.at(j),true);
            if (d>=0)
               g.addEdge(&(shapes.at(i)),&(shapes.at(j)),d);
         }
         else
            break;
      }
   }
   sort(shapes.begin(), shapes.end(), compareByY);
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         if (shapes.at(i).overlapY(shapes.at(j)))
         {
            int d=dist(shapes.at(i),shapes.at(j),false);
            if (d>=0)
               g.addEdge(&(shapes.at(i)),&(shapes.at(j)),d);
         }
         else
            break;
      }
   }
}

int CircuitMgr::dist(Shape& s1, Shape& s2, bool xType) 
{
   vector<Obstacle>& obstacles = _obstacles.at(s1.layer());
   int x1,x2,y1,y2;
   int d;
   if (xType)
   {
      x1=s2.getLL().x();
      x2=s1.getUR().x();
      if (compareByY(s1,s2))
      {
         if (s1.overlapY(s2))
            return 0;
         y1=s2.getUR().y();
         y2=s1.getLL().y();
      }
      else
      {
         if (s2.overlapY(s1))
            return 0;
         y1=s1.getUR().y();
         y2=s2.getLL().y();
      }
      d=y2-y1;
   }
   else
   {
      y1=s1.getLL().y();
      y2=s2.getUR().y();
      if (compareByX(s1,s2))
      {
         if (s1.overlapX(s2))
            return 0;
         x1=s1.getUR().x();
         x2=s2.getLL().x();
      }
      else
      {
         if (s2.overlapX(s1))
            return 0;
         x1=s2.getUR().x();
         x2=s1.getLL().x();
      }
      d=x2-x1;
   }
   Point ll(x1,y1);
   Point ur(x2,y2);
   for (int i=0; i<obstacles.size(); ++i)
   {
      if (obstacles.at(i).inside(ll,ur,xType,_spacing))
         return -1;
   }
   return d;
}


