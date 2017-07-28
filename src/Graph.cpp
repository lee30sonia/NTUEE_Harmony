/****************************************************************************
 FileName     [ Graph.cpp ]
 Synopsis     [ Implementation of graph ]
 ****************************************************************************/

#include "Graph.h"
#include "Circuit.h"
#include <iostream>
#include <algorithm>
#include <climits>
#ifdef _OMP
#include <omp.h>
#endif

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

bool Edge::changeNode(Node* from, Node* to)
{
   if( _node[0] == from ) {
      _node[0] = to;    return true;
   }
   if( _node[1] == from) {
      _node[1] = to;    return true;
   }
   return false;
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
}

void Graph::addEdge(Shape* o1, Shape* o2, int& weight, Point c1, Point c2)
{
   Node *n1=0, *n2=0;
   for (int i=0; i<_nodes.size(); ++i)
   {
      if (_nodes.at(i)->_obj[0]==o1)
         n1=_nodes.at(i);
      else if (_nodes.at(i)->_obj[0]==o2)
         n2=_nodes.at(i);
   }
   if (!n1)
   {
      n1=new Node(o1);
      #ifdef _DEBUG_ON
      n1->setId(_nodes.size());
      #endif
      _nodes.push_back(n1);
      o1->_inGraph=true;
   }
   if (!n2)
   {
      n2=new Node(o2);
      #ifdef _DEBUG_ON
      n2->setId(_nodes.size());
      #endif
      _nodes.push_back(n2);
      o2->_inGraph=true;
   }
   addEdge(n1, n2, weight, c1, c2);
}

void Graph::addNode(Shape* o)
{
   Node* n = new Node(o);
   #ifdef _DEBUG_ON
   n->setId(_nodes.size());
   #endif
   _nodes.push_back(n);
}

void Graph::mergeNodes(Edge* e, const int num)
{
   // copy the shapes
   for (int i=0; i<e->_node[1]->_obj.size(); ++i)
      e->_node[0]->_obj.push_back(e->_node[1]->_obj[i]);
   // segmentation fault, waiting for debug
   
   // copy the edges
   Edge* tempEdge;
   for (int i=0; i<e->_node[1]->_edges.size(); ++i)
   {
      tempEdge = e->_node[1]->_edges[i];
      if (tempEdge != e)
      {
         tempEdge->changeNode(e->_node[1], e->_node[0]);
         e->_node[0]->_edges.push_back(tempEdge);
      }
   }
   // delete the node
   for (int i=0; i<_nodes.size(); i++) {
      if(_nodes[i] == e->_node[1]) {
         _nodes[i]=_nodes.back();
         _nodes.pop_back();
         delete e->_node[1];
         break;
      }
   }
   
   // erase the edge
   _edges[num]=_edges.back();
   _edges.pop_back();
   delete e;
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

bool compareByX_O(Obstacle *s1, Obstacle *s2)
{
   return s1->getLL().x()<s2->getLL().x();
}

bool compareByY_O(Obstacle *s1, Obstacle *s2)
{
   return s1->getUR().y()>s2->getUR().y();
}

Graph* CircuitMgr::buildGraph(int layer)
{
   vector<Shape*>& shapes = _shapes.at(layer); 
   vector<Obstacle*>& obstacles = _obstacles.at(layer);
   if (shapes.size()==0)
   {
      #ifdef _DEBUG_ON
      cout<<endl<<"Graph of layer "<<layer<<" does not build because no shape is in the layer."<<endl;
      #endif
      return 0;
   }
   Graph* g=new Graph;
   Point connect[2];
   #ifdef _DEBUG_ON
   cout << "scanning trivial connections for layer " << layer << endl;
   #endif
   sort(shapes.begin(), shapes.end(), compareByY);
   sort(obstacles.begin(), obstacles.end(), compareByX_O);
   #ifdef _DEBUG_ON
   cout << "shapes sorted by Y." << endl;
   #endif
#ifdef _OMP
   #pragma omp parallel for
#endif
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         // if(shapes.at(i)->getLL().disXY(shapes.at(j)->getLL()) > _UR.y()) continue;
         // this condition skip checking for shapes that are too far from each other
         // too far == more than the width of the whole boundary
         // thus not all the connections will be found, but the speed is accelerated
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
   cout << "Y trivial checked and built." << endl;
   #endif

   sort(shapes.begin(), shapes.end(), compareByX);
   sort(obstacles.begin(), obstacles.end(), compareByY_O);
   #ifdef _DEBUG_ON
   cout << "shapes sorted by X." << endl;
   #endif
#ifdef _OMP
   #pragma omp parallel for
#endif
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
   #ifdef _DEBUG_ON
   cout << "X trivial checked and built." << endl;
   #endif
   
   for (int i=0; i<shapes.size(); ++i)
   {
      if (!shapes.at(i)->_inGraph)
         g->addNode(shapes.at(i));
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
            //cout << "overlap detected." <<s1.getLL().str()<<s1.getUR().str()<<s2.getLL().str()<<s2.getUR().str()<< endl;
            #endif
            connect[0] = connect[1] = Point(x1,s1.getLL().y());
            return 0;
         }
         y1=s2.getUR().y();
         y2=s1.getLL().y();
      }
      else
      {
         if (s2.overlapY(s1)) {
            #ifdef _DEBUG_ON
            //cout << "overlap detected." <<s1.getLL().str()<<s1.getUR().str()<<s2.getLL().str()<<s2.getUR().str()<< endl;
            #endif
            connect[0] = connect[1] = Point(x1,s2.getLL().y());
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
#ifdef _DEBUG_ON
     // cout << "checking obstacle for potential area " 
     //    << '(' << x1 << ',' << y1 << ") (" << x2 << ',' << y2 << ')' << endl;
#endif
      thru = new bool[l]; // through or not
      for(int i=0; i<l; i++)  thru[i] = true;
      int block = 0;
      for(int i=0; i<obstacles.size(); i++) {
         if(obstacles[i]->getUR().y()+_spacing<y1) break;
         if(obstacles[i]->getLL().y()-_spacing<y2)
            if(obstacles[i]->getUR().x()+_spacing>x1 && obstacles[i]->getLL().x()-_spacing<x2) 
               for(int j=obstacles[i]->getLL().x()-_spacing; j<=obstacles[i]->getUR().x()+_spacing; j++) {
                  if(j>=x1 && j<=x2) {
                     if(thru[j-x1]) {
                        thru[j-x1] = false;
                        block++;
                     }
                  }
                  else if(j>x2)  break;
               }
         if(block == l) break;
      }
      if(block != l)
         for(int i=0; i<l; i++)
            if(thru[i]) {     // choosing the first met available point
               connect[0].move(false, i);
               connect[1].move(false, i);
               delete[] thru;
               return d;
            }
      delete[] thru;
      return -1; 
   }
   else //yType
   {
      y1=(s1.getLL().y()>s2.getLL().y())?s1.getLL().y():s2.getLL().y();
      y2=s2.getUR().y();
      if (compareByX(&s1,&s2))
      {
         if (s1.overlapX(s2)) {
            #ifdef _DEBUG_ON
            //cout << "overlap detected." <<s1.getLL().str()<<s1.getUR().str()<<s2.getLL().str()<<s2.getUR().str()<< endl;
            #endif
            connect[0] = connect[1] = Point(s2.getLL().x(),y1);
            return 0;
         }
         x1=s1.getUR().x();
         x2=s2.getLL().x();
      }
      else
      {
         if (s2.overlapX(s1)) {
            #ifdef _DEBUG_ON
            //cout << "overlap detected." <<s1.getLL().str()<<s1.getUR().str()<<s2.getLL().str()<<s2.getUR().str()<< endl;
            #endif
            connect[0] = connect[1] = Point(s1.getLL().x(),y1);
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
#ifdef _DEBUG_ON
    //  cout << "checking obstacle for potential area " 
    //     << '(' << x1 << ',' << y1 << ") (" << x2 << ',' << y2 << ')' << endl;
#endif
      thru = new bool[l];
      for(int i=0; i<l; i++)  thru[i] = true;
      int block = 0;
      for(int i=0; i<obstacles.size(); i++) {
         if(obstacles[i]->getLL().x()-_spacing>x2) break;
         if(obstacles[i]->getUR().x()+_spacing>x1)
            if(obstacles[i]->getUR().y()+_spacing>y1 && obstacles[i]->getLL().y()-_spacing<y2) 
               for(int j=obstacles[i]->getLL().y()-_spacing; j<=obstacles[i]->getUR().y()+_spacing; j++) {
                  if(j>=y1 && j<=y2) {
                     if(thru[j-y1]) {
                        thru[j-y1] = false;
                        block++;
                     }
                  }
                  else if(j>y2)  break;
               }
         if(block == l) break;
      }
      if(block != l)
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

void CircuitMgr::connectLayer(int layer)
{
#ifdef _DEBUG_ON
   cout<<"connecting layer "<<layer<<" and "<<layer+1<<" with via."<<endl;
#endif
   vector<Shape*> shapes = _shapes.at(layer);
   for (int i=0; i<_shapes.at(layer+1).size(); ++i)
      shapes.push_back(_shapes.at(layer+1).at(i));
   
   Point connect[2];
   int dMin=INT_MAX; Point c[2]; int l;
   sort(shapes.begin(), shapes.end(), compareByY);
   for (int i=0; i<shapes.size()-1; ++i)
   {
      for (int j=i+1; j<shapes.size(); ++j)
      {
         if (shapes.at(i)->overlapY(*shapes.at(j)))
         {
            if (shapes.at(i)->layer()!=shapes.at(j)->layer())
            {
               int d = dist(*shapes.at(i),*shapes.at(j),false, connect);
               if (d==0)
               {
                  addVia(connect[0].x(),connect[0].y(),layer);
                  return;
               }
               else if (d>0)
               {
                  if (d<dMin)
                  {
                     Shape& s1=*shapes.at(i);
                     Shape& s2=*shapes.at(j);
                     dMin=d;
                     c[0]=(connect[0].inside(s2.getLL(),s2.getUR()))?connect[0]:connect[1]; //the one that is in s2. to add Via here
                     c[1]=(c[0]!=connect[0])?c[0]:c[1]; //the other one. to addLine on s1.layer()
                     l=s1.layer();
                  }
               }
            }
         }
         else
            break;
      }
   }
   // if the program reaches here, means there is not any overlap between the two layers
   if (dMin<INT_MAX)
   {
      if (!addVia(connect[0].x(),connect[0].y(),layer))
         cout<<"addVia ERROR in connectLayer()!!"<<endl;
      if (!addLine(connect[0],connect[1],l))
         cout<<"addLine ERROR in connectLayer()!!"<<endl;
   }
   else
   {
      #ifdef _BEDUG_ON
      cout<<"No trivial place to add Via..."<<endl;
      #endif
   }
}
