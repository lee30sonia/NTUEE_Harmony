/****************************************************************************
  FileName     [ Graph.h ]
  Synopsis     [ Define graph data structures ]
  Discription  [ Define classes to build the graph,
                 including Point, Node, Edge, and Graph]
****************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>
#include <list>
#include "Circuit.h"
using namespace std;

class Node;
class Shape;
class Obstacle;
bool compareByX_O(Obstacle *s1, Obstacle *s2);
/*class DSet;

class DSetObj
{
   friend DSet;
public:
   DSetObj(Obj*);
   
private:
   Obj* _obj; //allowed types: shape via line
   DSetObj* _next;
   DSetObj* _set;
};

class DSet
{
public:
   DSet(DSetObj); //make-set with an initial obj
   
   DSetObj* _head;
   DSetObj* _tail;
};*/

#ifndef POINT
#define POINT
class Point
{
public:
   Point(){ }
   Point(int x, int y);

   int x() const { return _x; }
   int y() const { return _y; }
   string str() const;
   int disX(const Point& p) const { return abs(_x-p.x()); }
   int disY(const Point& p) const { return abs(_y-p.y()); }
   int disXY(const Point& p) const { return disX(p)+disY(p); }
   //int& level(const int& layer) { return _map[layer][_x][_y]; }
   
   bool inside(Point LL, Point UR, int spacing=0); //whether this point is inside the rectangle given by LL and UR or its margin of width "spacing"

   //for line iteration
   void move(bool vertical, int dis = 1); //increase y by dis if vertical, x otherwise
   bool move(char dir);    // u, d, l, r
   bool move(char dir, Point& ll, Point& ur, int spacing);
   bool encounter(Obstacle* ob, char dir, int spacing); //for routing
   
   bool operator!=(const Point& p);
   bool operator==(const Point& p);

private:
   int _x;
   int _y;
};
#endif

//set
//member pointers: root, ulmost, llmost, ...

class Edge
{
   
public:
   Edge(Node *a, Node *b, const int& w, Point c1, Point c2);

   Node* _node[2];
   int _weight;
   Point _connect[2];  // _connect[2]
   
   Node* getNeighbor(Node* n);
   bool changeNode(Node* from, Node* to);
   
   //bool operator < (const Edge& rhs) const;
   
};

/*
class Pair
{
public:
   Pair(Node *a, const int& k);
   Node* _node;
   int _key;
   bool operator>(const Pair &rhs) {return this._key> rhs.key; }
};
*/

class Node
{   
public:
   Node(Shape* obj): _inMST(false) { _obj.push_back(obj); }
   void addEdge(Edge *e) { _edges.push_back(e); }
   //bool operator==(const Obj* o) { return _obj==o; }
   //void addEdge(Node *n);
   //Edge* getEdgeByNeighbor(Node*);
   
   void setId(int i) { _id=i; }
   
   vector<Shape*> _obj;
   vector<Edge *> _edges;

   Node* _pi; //parent
   Edge* _connectEdge; //the edge to _pi
   int _key; //key for priority queue
   bool _inMST;
   int _rank;
   int _layer;
   
   // saving the adjacency list as pairs
   // list<pair<Node*, int>> _adj;
   bool operator>(const Node &rhs) { return this->_key> rhs._key; }

   int _id; //id to store adjacency list (list<pair<Node*, int>>) in Graph, starting from 0, usage: g->_adj[n->_id]

};

class Graph
{
   
public:
   Graph() { }
   ~Graph();
   
   void addEdge(Node* n1, Node* n2, int& weight, Point c1, Point c2); //n1 n2 already in graph
   void addEdge(Shape* n1, Shape* n2, int& weight, Point c1, Point c2);
   void addNode(Shape* o);
   void mergeNodes(Edge* e, const int num);
   //Node * getNodeById(const int& id);
   
   //map<int, Node *> nodesMap;
   vector<Node *> _nodes;
   vector<Edge *> _edges;
   vector<vector<Node *>> _trees;
};
#endif
