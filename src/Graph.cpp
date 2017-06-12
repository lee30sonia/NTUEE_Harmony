/****************************************************************************
 FileName     [ Graph.cpp ]
 Synopsis     [ Implementation of graph ]
 ****************************************************************************/

#include "Graph.h"
#include "Circuit.h"
#include <iostream>
#include <algorithm>
#include <omp.h>

using namespace std;
class Point;

/********************Edge*********************/

Edge::Edge(Node *a, Node *b, const int& w, Point c1, Point c2)
{
   _node[0]=a;
   _node[1]=b;
   _weight=w;
   _connect[0] = c1;
   _connect[1] = c2;
}

Node* Edge::getNeighbor(Node* a)
{
   if ( _node[0] == a ) return _node[1];
   if ( _node[1] == a ) return _node[0];

   return 0;
}

/********************Pair*********************/
/*
Pair::Pair(Node *a, const int &k)
{
   _node= a;
   _key= k;
}
*/

/********************Node*********************/

/********************Graph*********************/
Graph::~Graph()
{
   for (int i=0; i<_nodes.size(); ++i)
      delete _nodes.at(i);
   for (int i=0; i<_edges.size(); ++i)
      delete _edges.at(i);
}

void Graph::addEdge(Node* n1, Node* n2, int& weight, Point c1, Point c2)
{
   Edge* e = new Edge(n1,n2,weight, c1, c2);
   _edges.push_back(e);
   //_adj[n1->_id].push_back(make_pair<n2, weight>);
   //_adj[n2->_id].push_back(make_pair<n1, weight>);
   n1->_adj.push_back(e);
   n2->_adj.push_back(e);

   #ifdef _DEBUG_ON
   cout<<"Graph: add edge from node "<<n1->_id<<" to node "<<n2->_id<<" with weight "<<weight<<endl;
   if(weight!=c1.disXY(c2))
      cout << "Error: Edge weight and endpoints not consistent!" << endl;
   #endif
}
void Graph::addEdge(Obj* o1, Obj* o2, int& weight, Point c1, Point c2)
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
   addEdge(n1, n2, weight, c1, c2);
}

/********************CircuitMgr*********************/


bool compareByX(Shape *s1, Shape *s2)
{
   return s1->getLL().x()<s2->getLL().x();
}

bool compareByY(Shape *s1, Shape *s2)
{
   return s1->getUR().y()>s2->getUR().y();
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
   Point connect[2];
   #pragma omp parallel for
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         if (shapes.at(i)->overlapX(*shapes.at(j)))
         {
            int d = dist(*shapes.at(i),*shapes.at(j),true, connect);
            if (d>=0)
               g->addEdge(shapes.at(i),shapes.at(j),d, connect[0], connect[1]);
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
            int d = dist(*shapes.at(i),*shapes.at(j),false, connect);
            if (d>=0)
               g->addEdge(shapes.at(i),shapes.at(j),d, connect[0], connect[1]);
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

int CircuitMgr::dist(Shape& s1, Shape& s2, bool xType, Point* connect)
{
   vector<Obstacle*>& obstacles = _obstacles.at(s1.layer());
   int x1,x2,y1,y2;
   int d, l;
   bool* thru; // through

   // calculate the distance between two shapes 
   if (xType)
   {
      x1=s2.getLL().x();
      x2=(s1.getUR().x()<s2.getUR().x())?s1.getUR().x():s2.getUR().x();
      if (compareByY(&s1,&s2))
      {
         if (s1.overlapY(s2)) {
            #ifdef _DEBUG_ON
            cout << "overlap detected." << endl;
            #endif
            connect[0] = connect[1] = Point(0,0);
            return 0;
         }
         y1=s2.getUR().y();
         y2=s1.getLL().y();
      }
      else
      {
         if (s2.overlapY(s1)) {
            #ifdef _DEBUG_ON
            cout << "overlap detected." << endl;
            #endif
            connect[0] = connect[1] = Point(0,0);
            return 0;
         }
         y1=s1.getUR().y();
         y2=s2.getLL().y();
      }

      d=y2-y1;
      connect[0] = Point(x1,y1);
      connect[1] = Point(x1,y2);

      // check for obstacles
    /*  #ifdef _DEBUG_ON
      cout << "Edge: obstacles checking..." << endl;
      #endif */

      l = x2-x1+1;
      thru = new bool[l];
      for(int i=0; i<l; i++)  thru[i] = true;
      for(int i=0; i<obstacles.size(); i++)
         if(obstacles[i]->getUR().x()>x1 && obstacles[i]->getLL().x()<x2) 
            for(int j=obstacles[i]->getLL().x(); j<=obstacles[i]->getUR().x(); j++) {
               if(j>=x1 && j<=x2)   thru[j-x1] = false;
               else if(j>x2)  break;
            }
      for(int i=0; i<l; i++)
         if(thru[i]) {
            connect[0].move(false, i);
            connect[1].move(false, i);
            delete[] thru;
            return d;
         }
      delete[] thru;
      return -1; 
   }
   else
   {
      y1=(s1.getLL().y()>s2.getLL().y())?s1.getLL().y():s2.getLL().y();
      y2=s2.getUR().y();
      if (compareByX(&s1,&s2))
      {
         if (s1.overlapX(s2)) {
            #ifdef _DEBUG_ON
            cout << "overlap detected." << endl;
            #endif
            connect[0] = connect[1] = Point(0,0);
            return 0;
         }
         x1=s1.getUR().x();
         x2=s2.getLL().x();
      }
      else
      {
         if (s2.overlapX(s1)) {
            #ifdef _DEBUG_ON
            cout << "overlap detected." << endl;
            #endif
            connect[0] = connect[1] = Point(0,0);
            return 0;
         }
         x1=s2.getUR().x();
         x2=s1.getLL().x();
      }
      d=x2-x1;
      connect[0] = Point(x1,y1);
      connect[1] = Point(x2,y1);

      // check for obstacles
      /*#ifdef _DEBUG_ON
      cout << "Edge: obstacles checking..." << endl;
      #endif*/
      l = y2-y1+1;
      thru = new bool[l];
      for(int i=0; i<l; i++)  thru[i] = true;
      for(int i=0; i<obstacles.size(); i++)
         if(obstacles[i]->getUR().y()>y1 && obstacles[i]->getLL().y()<y2) 
            for(int j=obstacles[i]->getLL().y(); j<=obstacles[i]->getUR().y(); j++) {
               if(j>=y1 && j<=y2)   thru[j-y1] = false;
               else if(j>y2)  break;
            }
      for(int i=0; i<l; i++)
         if(thru[i]) {
            connect[0].move(true, i);
            connect[1].move(true, i);
            delete[] thru;
            return d;
         }
      delete[] thru;
      return -1; 
   }

   // check for obstacles
   /*
   Point ll(x1,y1);
   Point ur(x2,y2);
   //cout<<ll.str()<<" "<<ur.str()<<endl;
   for (int i=0; i<obstacles.size(); ++i)
   {
      if (obstacles.at(i)->inside(ll,ur,xType,_spacing))
         return -1;
   }
   return d;
   */
}


