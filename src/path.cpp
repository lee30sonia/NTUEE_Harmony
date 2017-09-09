/*******************************************************************************
  FileName     [ path.cpp ]
  Synopsis     [ Implement MST and Hadlock's algorithm ]
*******************************************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <cmath>
#include "Circuit.h"
#include "Graph.h"

using namespace std;
class Node;

// Using Prims's algorithm to solve mst
vector<Node*> CircuitMgr::mstPrim(const Graph* g, vector<unsigned>& set_sizes)
{
#ifdef _DEBUG_ON
   cout << "Building MST..." << endl;
#endif
   // initialize the nodes
   vector<Node*> pQ;
   for (int i=0; i< g->_nodes.size(); ++i) {
      g->_nodes[i]->_key= INT_MAX;
      g->_nodes[i]->_pi= NULL;
      enqueue(pQ,g->_nodes[i]);
   }
   
   vector<Node*> roots;
   Node* n = pQ[0];
   // Node *n = g->_nodes[0];
   short setNum = -1;
   unsigned size;

   while (n!=0)
   {
      //set key to 0 for root node n
      n->_key= 0;
      /*
      // since we always set pQ[0] as n, there is no more need to descrease_key it here
      for(int j=0; j<pQ.size(); j++){
         if(pQ[j] == n) {
            decrease_key(pQ, j);
            break;
         }
      }
      */
      
      roots.push_back(n);
      setNum++;
      size = 0;
      n=0;
      
      while(!pQ.empty()) {
         Node *u= dequeue(pQ);
         if (u->_key==INT_MAX) {
            enqueue(pQ, u);
            break;
         }
         
         u->_inMST = true;
         for (int i=0; i<u->_obj.size(); ++i)
            u->_obj[i]->setsetNum(setNum);
         size++;
         for (int i=0; i< u->_edges.size(); i++){
            Node* v= u->_edges[i]->getNeighbor(u);
            int weight= u->_edges[i]->_weight;
            
            if (v->_inMST==false && weight < v->_key) {
               v->_pi= u;
               v->_connectEdge = u->_edges[i];
               v->_key= weight;
               for(int j=0; j<pQ.size(); j++){
                  if(pQ[j] == v) {
                     decrease_key(pQ, j);
                     break;
                  }
               }
            }
         }
      }
   
      set_sizes.push_back(size);
      if(!pQ.empty()) n = pQ[0];
   }
   
   #ifdef _DEBUG_ON
   cout<<"MST completed."<<endl;
   for (int i=0; i<g->_nodes.size(); ++i)
   {
      if (!g->_nodes[i]->_inMST)
         cout<<"node "<<g->_nodes[i]->_id<<" not in MST.";
   }
   #endif
   return roots;
}

// insert as a min heap
void CircuitMgr::enqueue(vector<Node *>& pQ, Node* n)
{
   pQ.push_back(n);
   decrease_key(pQ, pQ.size()-1);
}

Node* CircuitMgr::dequeue(vector<Node *>& pQ)
{
   //extract min from priority queue
   Node* min= pQ[0];

   //delete element from queue
   pQ[0]= pQ[pQ.size()-1];
   pQ.pop_back();

   increase_key(pQ,0);

   return min;
}

void CircuitMgr::decrease_key(vector<Node *>& pQ, int i)
{
   Node* temp;
   while (i>0) {
      if (pQ[(i-1)/2]->_key > pQ[i]->_key) {
         temp= pQ[i];
         pQ[i]= pQ[(i-1)/2];
         pQ[(i-1)/2]= temp;

         i= (i-1)/2;
      }
      else
         break;
   }
}

// increase key
void CircuitMgr::increase_key(vector<Node *>& pQ, int i)
{
   Node* temp;
   //int i= index;

   while (i*2+1< pQ.size()) { //has child(ren)

      if (i*2+2>= pQ.size()) { //only left child exists
         if (pQ[i*2+1]->_key < pQ[i]->_key) {
            temp= pQ[i];
            pQ[i]= pQ[i*2+1];
            pQ[i*2+1]= temp;
         }
         break;
      }

      else { //both children exist
         if (pQ[i*2+1]->_key < pQ[i]->_key || pQ[i*2+2]->_key < pQ[i]->_key) { //either child is smaller than parent
            if (pQ[i*2+1]->_key< pQ[i*2+2]->_key) {
               temp= pQ[i];
               pQ[i]= pQ[i*2+1];
               pQ[i*2+1]= temp;
               i= i*2+1; //going down the heap after swap
            }
            else {
               temp= pQ[i];
               pQ[i]= pQ[i*2+2];
               pQ[i*2+2]= temp;
               i= i*2+2; //going down the heap after swap
            }
         }
         else
            break;
      }
   }
}

void CircuitMgr::makeSet(Node* a)
{
   a->_pi= a;
   a->_rank= 0;
}

void CircuitMgr::linkSet(Node* a, Node* b)
{
   if (a->_rank >= b->_rank) {
      b->_pi= a;
   }

   else {
      a->_pi= b;
      if (a->_rank == b->_rank) {
         b->_rank++;
      }
   }
}

void CircuitMgr::mstKruskal(const Graph* g)
{
   //using Kruskal's algorithm tp solve MST

   for (int i=0;i< g->_nodes.size();i++) {
      makeSet(g->_nodes[i]);
   }

   /*****TODO******/
   //sort the edges by nondecreasing order

   for (int i=0; i< g->_edges.size();i++) {
      if (findSet(g->_edges[i]->_node[0])!= findSet(g->_edges[i]->_node[1])) {
         unionSet(g->_edges[i]->_node[0], g->_edges[i]->_node[1]);
      }
   }
}

Node* CircuitMgr::findSet(Node* a)
{
   if (a->_pi!= a)
      a->_pi= findSet(a->_pi);
   return a->_pi;
}

void CircuitMgr::unionSet(Node* a, Node* b)
{
   linkSet(findSet(a), findSet(b));
}

void CircuitMgr::reduce3d(vector<Node*> nodes)
{
   vector<Node*> nodes_h;
   vector<Node*> nodes_l;
   vector<Node*> nodes_prime;
   //assume nodes vector has been sorted or initialised in order of layer

   int l_m= nodes[nodes.size()/2]->_layer;

   for (int i=0; i< nodes.size(); i++) {
      if (nodes[i]->_layer > l_m)
         nodes_h.push_back(nodes[i]);
      else if (nodes[i]->_layer < l_m)
         nodes_l.push_back(nodes[i]);
   }

   for (int i=0; i<nodes_h.size(); i++) {
      //if the node can connect a line segment to a node in the middle layer without intersecting any obstacles
      if (check43d(nodes_h[i])) {
         //create new nodes
         Node u= *nodes_h[i];
         u._layer= l_m;
         nodes_prime.push_back(&u);   
      }
   }

   for (int i=0; i<nodes_l.size(); i++) {
      if (check43d(nodes_h[i])) {
         Node u= *nodes_l[i];
         u._layer= l_m;
         nodes_prime.push_back(&u);
      }
   }
}

bool CircuitMgr::check43d(Node* a, const int layer)
{
   return false;
}

void CircuitMgr::findSteiner(Node* a)
{

}

bool CircuitMgr::sameSet(Node* a, Node* b)
{
   if (findSet(a)==findSet(b))
      return true;
   else
      return false;
}

/*
void CircuitMgr::init4op(const ) {

}
*/

// Finding shortest path using Hadlock's algorithm
// this function still need spacing and boundary checking
// and a way to find a smaller map to use

bool CircuitMgr::shortestPath(const Point s, const Point t, const int layer) {

   #ifdef _DEBUG_ON
   // cout << "Finding shortest path between " << s.str() << "and " << t.str() << endl;
   #endif

   init4short(s, t, layer);

   int current = -1, dis2t, level;
   bool reach = false;
   Point p;
   setLevel(s, 0);
   setDir(s, 's');
   (_Q[0]).push(s);

   // update all the needed level
   while(!reach) {
      current++;
      _Q.push_back(queue<Point>());
      while(!_Q[current].empty()) {
         p = _Q[current].front();
         _Q[current].pop();
         dis2t = p.disXY(t);
         level = getLevel(p);
         p.move(false, -1);
         reach = check4short(p, t, 'r', dis2t, level);
         p.move(false, 2);
         reach = check4short(p, t, 'l', dis2t, level);
         p.move(false, -1);
         p.move(true, -1);
         reach = check4short(p, t, 'u', dis2t, level);
         p.move(true, 2);
         reach = check4short(p, t, 'd', dis2t, level);
      }
   }

   // collect the path
   Point start = p = t;
   char lastDir = getDir(t);
   while(lastDir != 's') {
      if(!p.move(lastDir, _mapLL, _mapUR, 0)) {
         cout << "Connection Error!" << endl;
         return false;
         break;
      }
      if(getDir(p) != lastDir) {
         addLine(start.x(), start.y(), p.x(), p.y(), layer);
         lastDir = getDir(p);
      }
   }
   return true;
}

// assume the obstacles has been sorted by X
void CircuitMgr::init4short(const Point s, const Point t, int layer)
{
   // define the boundary of the map
   Point p1, p2;
   int length = max(s.disX(t), s.disY(t)), x1, x2, y1, y2;

   if(s.x() < t.x()) {
      p1 = s;  p2 = t;
   }
   else  {
      p2 = s;  p1 = t;
   }
   if((p1.disX(_LL)-_spacing) < 0.5*length)  {
      x1 = _spacing;    x2 = x1 + length;
   }
   else if((p2.disX(_UR)-_spacing) < 0.5*length) {
      x2 = _UR.x()-_spacing;  x1 = x2-length;
   }
   else {
      x1 = p1.x()-0.5*length;    x2 = p2.x()+0.5*length;
   }
   if(s.y() < t.y()) {
      p1 = s;  p2 = t;
   }
   else  {
      p2 = s;  p1 = t;
   }
   if((p1.disY(_LL)-_spacing) < 0.5*length)  {
      y1 = _spacing;    y2 = y1 + length;
   }
   else if((p2.disY(_UR)-_spacing) < 0.5*length) {
      y2 = _UR.y()-_spacing;  y1 = y2-length;
   }
   else {
      y1 = p1.y()-0.5*length;    y2 = p2.y()+0.5*length;
   }
   _mapLL = Point(x1, y1);
   _mapUR = Point(x2, y2);
   length = _mapLL.disX(_mapUR)+1;

   // initiate all the level cell to -1, all the dir to 'e'(empty)
   _levelMap = new int*[length];
   _dirMap = new char*[length];
   for(int i=0; i<length; i++) {
      _levelMap[i] = new int[length];
      _dirMap[i] = new char[length];
   }
   for(int i=0; i<length; i++)
      for(int j=0; j<length; j++) {
         _levelMap[i][j] = -1;
         _dirMap[i][j] = 'e';
      }

   // initiate obstables as INT_MAX 
   // assume the obstacels has been sorted by X
   Obstacle* it;
   for(unsigned n=0; n<_obstacles[layer].size(); n++) {
      it = _obstacles[layer][n];
      if(it->getLL().x()-_spacing > _mapUR.x()) break;
      if(it->getUR().x()+_spacing < _mapLL.x() 
            || it->getLL().y()-_spacing > _mapUR.y() 
            || it->getUR().y()+_spacing < _mapLL.y())
         continue;
      for(int i=it->getLL().x()-_spacing; i<=it->getUR().x()+_spacing; i++)
         for(int j=it->getLL().y()-_spacing; j<=it->getUR().y()+_spacing; j++)
            if(x1<=i && i<=x2 && y1<=j && j<=y2)
               _levelMap[i-x1][j-y1] = INT_MAX;
   }
   
   // initialise the queue vector
   _Q.clear();
   _Q.push_back(queue<Point>());

   #ifdef _DEBUG_ON
   // cout << "Map initialized." << endl;
   #endif
}

bool CircuitMgr::check4short(const Point& p, const Point& t, const char& dir,
      const int& dis2t, const int& level)
{
   if(p.x()<_spacing || p.x()>_LL.disX(_UR)-_spacing) return false; 
   if(p.y()<_spacing || p.y()>_LL.disY(_UR)-_spacing) return false; 
   if(getLevel(p) >= 0)   return false;

   if(getDir(p) == 'e')  setDir(p, dir);
   if(p.disXY(t) == 0) return true;
   int level2;
   if(p.disXY(t) > dis2t)  level2 = level+1;
   else level2 = level;
   setLevel(p, level2);
   _Q[level2].push(p);
   return false;
}

int int_max = INT_MAX;
char char0 = 0;

int& CircuitMgr::getLevel(const Point& p)
{
   if(_mapLL.x()<=p.x() && p.x()<=_mapUR.x() && _mapLL.y()<=p.y() && p.y()<=_mapUR.y())
      return _levelMap[p.x()-_mapLL.x()][p.y()-_mapLL.y()];
   else  return int_max;
}

char& CircuitMgr::getDir(const Point& p)
{
   if(_mapLL.x()<=p.x() && p.x()<=_mapUR.x() && _mapLL.y()<=p.y() && p.y()<=_mapUR.y())
      return _dirMap[p.x()-_mapLL.x()][p.y()-_mapLL.y()];
   else  return char0;
}

// make sure it's not out of the map!!
void CircuitMgr::setLevel(const Point& p, int level)
{
   _levelMap[p.x()-_mapLL.x()][p.y()-_mapLL.y()] = level;
}

// make sure it's not out of the map!!
void CircuitMgr::setDir(const Point& p, char dir)
{
   _dirMap[p.x()-_mapLL.x()][p.y()-_mapLL.y()] = dir;
}
