/****************************************************************************
  FileName     [ Graph.h ]
  Synopsis     [ Define graph data structures ]
****************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>
#include <list>
#include "Circuit.h"
using namespace std;

class Node;
class Obj;
class Point;
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

class Edge
{
   
public:
   Edge(Node *a, Node *b, const int& w, Point c1, Point c2);
   ~Edge();

   Node* _node[2];
   int _weight;
   Point* _connect;  // _connect[2]
   
   Node* getNeighbor(Node* n);
   
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
   Node(Obj* obj): _obj(obj) { }
   void addEdge(Edge *e) { _edges.push_back(e); }
   //bool operator==(const Obj* o) { return _obj==o; }
   //void addEdge(Node *n);
   //Edge* getEdgeByNeighbor(Node*);
   
   void setId(int i) { _id=i; }
   
   Obj* _obj;
   vector<Edge *> _edges;

   Node* _pi; //parent
   int _key; //key for priority queue
   bool _inMST;
   int _rank;
   
   // saving the adjacency list as pairs
   //list<pair<Node*, int>> _adj;
   vector<Edge*> _adj;
   bool operator>(const Node &rhs) { return this->_key> rhs._key; }

   int _id; //id to store adjacency list (list<pair<Node*, int>>) in Graph, starting from 0, usage: g->_adj[n->_id]

};

class Graph
{
   
public:
   Graph() { }
   ~Graph();
   
   void addEdge(Node* n1, Node* n2, int& weight, Point c1, Point c2); //n1 n2 already in graph
   void addEdge(Obj* n1, Obj* n2, int& weight, Point c1, Point c2);
   
   //Node * getNodeById(const int& id);
   
   //map<int, Node *> nodesMap;
   vector<Node *> _nodes;
   vector<Edge *> _edges;
   

};
#endif
