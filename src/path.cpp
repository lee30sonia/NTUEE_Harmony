#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include "Circuit.h"
#include "Graph.h"

using namespace std;
class Node;

// Using Prims's algorithm to solve mst
vector<Node*> CircuitMgr::mstPrim(const Graph* g, vector<unsigned>& set_sizes)
{
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
         for (int i=0; i< u->_adj.size(); i++){
            Node* v= u->_adj[i]->getNeighbor(u);
            int weight= u->_adj[i]->_weight;
            
            if (v->_inMST==false && weight < v->_key) {
               v->_pi= u;
               v->_connectEdge = u->_adj[i];
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
      // checking pQ is the same. this way we don't need to go through all the nodes
      /*
      for (int i=0; i< g->_nodes.size(); ++i)
      {
         //find new root for tree if not connected
         if (!g->_nodes[i]->_inMST) {
            n=g->_nodes[i];         
            break;
         }
      }
      */
   }
   
   #ifdef _DEBUG_ON
   cout<<"MST completed."<<endl;
   for (int i=0; i<g->_nodes.size(); ++i)
   {
      if (!g->_nodes[i]->_inMST)
         cout<<"node "<<g->_nodes[i]->_id<<" not in MST.";
      //if (!g->_nodes[i]->_pi) {
         //cout<<"node "<<g->_nodes[i]->_id<<" ,pi = null"<<endl;
         //cout<<"node "<<g->_nodes[i]->_pi->_id<<endl; }
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
/*
bool CircuitMgr::shortestPath(const Point s, const Point t, const int layer) {

   #ifdef _DEBUG_ON
   cout << "Finding shortest path between " << s.str() << "and " << t.str() << endl;
   #endif

   int current = -1, dis2t, level;
   bool reach = false;
   Point p;
   setLevel(s,layer,0);
   setDir(s, layer, 's');
   (_Q[0]).push(s);

   // update all the needed level
   while(!reach) {
      current++;
      _Q.push_back(queue<Point>());
      while(!_Q[current].empty()) {
         p = _Q[current].front();
         _Q[current].pop();
         dis2t = p.disXY(t);
         level = getLevel(p, layer);
         p.move(false, -1);
         reach = check4short(p, t, layer, 'r', dis2t, level);
         p.move(false, 2);
         reach = check4short(p, t, layer, 'l', dis2t, level);
         p.move(false, -1);
         p.move(true, -1);
         reach = check4short(p, t, layer, 'u', dis2t, level);
         p.move(true, 2);
         reach = check4short(p, t, layer, 'd', dis2t, level);
      }
   }

   // collect the path
   Point start = p = t;
   char lastDir = getDir(t, layer);
   while(lastDir != 's') {
      if(!p.move(lastDir)) {
         cout << "Connection Error!" << endl;
         break;
      }
      if(getDir(p, layer) != lastDir) {
         addLine(start.x(), start.y(), p.x(), p.y(), layer);
         lastDir = getDir(p, layer);
      }
   }
   return true; //?
}

void CircuitMgr::init4short(int layer) {
   // the map of each level is no built until needed
   if(!_levelMap[layer])   _levelMap[layer] = new int*[_LL.disX(_UR)];
   if(!_dirMap[layer])     _dirMap[layer] = new char*[_LL.disX(_UR)];
   // initiate all the level cell to -1, all the dir to 0
   for(int i=0; i<_LL.disX(_UR); i++) {
      _levelMap[layer][i] = new int[_LL.disY(_UR)];
      _dirMap[layer][i] = new char[_LL.disY(_UR)];
   }
   for(int i=0; i<_LL.disX(_UR); i++)
      for(int j=0; j<_LL.disY(_UR); j++) {
         _levelMap[layer][i][j] = -1;
         _dirMap[layer][i][j] = 0;
      }

   // initiate obstables as INT_MAX 
   Obstacle* it;
   for(unsigned n=0; n<_obstacles[layer].size(); n++) {
      it = _obstacles[layer][0];
      for(int i=it->getLL().x(); i<=it->getUR().x(); i++)
         for(int j=it->getLL().y(); j<=it->getUR().y(); j++)
            _levelMap[layer][i][j] = INT_MAX;
   }
   
   // initialise the queue vector
   _Q.clear();
   _Q.push_back(queue<Point>());
}

bool CircuitMgr::check4short(const Point& p, const Point& t, const int& layer, const char& dir,
      const int& dis2t, const int& level) {
   if(p.x()<0 || p.x()>(_LL.disX(_UR))) return false; 
   if(p.y()<0 || p.y()>(_LL.disY(_UR))) return false; 
   if(getLevel(p, layer) >= 0)   return false;

   if(getDir(p,layer) == 0)  setDir(p,layer,dir);
   if(p.disXY(t) == 0) return true;
   int level2;
   if(p.disXY(t) > dis2t) level2 = level+1;
   else  level2 = level;
   setLevel(p, layer, level2);
   _Q[level2].push(p);
   return false;

   #ifdef _DEBUG_ON
   cout << "Maps initialized." << endl;
   #endif
}
*/
