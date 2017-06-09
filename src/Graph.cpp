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
   #ifdef _DEBUG_ON
   cout<<"Graph: add edge from node "<<n1->_id<<" to node "<<n2->_id<<" with weight "<<weight<<endl;
   #endif
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
      #ifdef _DEBUG_ON
      n1->setId(_nodes.size());
      #endif
      _nodes.push_back(n1);
   }
   if (!n2)
   {
      n2=new Node(o2);
      #ifdef _DEBUG_ON
      n2->setId(_nodes.size());
      #endif
      _nodes.push_back(n2);
   }
   addEdge(n1,n2,weight);
}

/********************CircuitMgr*********************/


bool compareByX(Shape& s1, Shape& s2)
{
   return s1.getLL().x()<s2.getLL().x();
}

bool compareByY(Shape& s1, Shape& s2)
{
   return s1.getUR().y()>s2.getUR().y();
}

Graph* CircuitMgr::buildGraph(int layer)
{
   vector<Shape*>& shapes = _shapes.at(layer);
   if (shapes.size()==0)
   {
      #ifdef _DEBUG_ON
      cout<<"Graph of layer "<<layer<<" does not build because no shape is in the layer."<<endl;
      #endif
      return 0;
   }
   
   Graph* g=new Graph;
   sort(shapes.begin(), shapes.end(), compareByX);
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         if (shapes.at(i)->overlapX(*shapes.at(j)))
         {
            int d=dist(*shapes.at(i),*shapes.at(j),true);
            if (d>=0)
               g->addEdge(shapes.at(i),shapes.at(j),d);
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
         if (shapes.at(i)->overlapY(*shapes.at(j)))
         {
            int d=dist(*shapes.at(i),*shapes.at(j),false);
            if (d>=0)
               g->addEdge(shapes.at(i),shapes.at(j),d);
         }
         else
            break;
      }
   }
   #ifdef _DEBUG_ON
   cout<<"Graph of layer "<<layer<<" built, edge num = "<<g->_edges.size()<<", node num = "<<g->_nodes.size()<<endl;
   #endif
   return g;
}

int CircuitMgr::dist(Shape& s1, Shape& s2, bool xType)
{
   vector<Obstacle*>& obstacles = _obstacles.at(s1.layer());
   int x1,x2,y1,y2;
   int d;
   if (xType)
   {
      x1=s2.getLL().x();
      x2=(s1.getUR().x()<s2.getUR().x())?s1.getUR().x():s2.getUR().x();
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
      y1=(s1.getLL().y()>s2.getLL().y())?s1.getLL().y():s2.getLL().y();
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
   cout<<ll.str()<<" "<<ur.str()<<endl;
   for (int i=0; i<obstacles.size(); ++i)
   {
      if (obstacles.at(i)->inside(ll,ur,xType,_spacing))
         return -1;
   }
   return d;
}



