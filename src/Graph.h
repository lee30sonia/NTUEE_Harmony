/****************************************************************************
  FileName     [ Graph.h ]
  Synopsis     [ Define graph data structures ]
****************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "Circuit.h"
using namespace std;

class Node;
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
    Edge(Node *a, Node *b, const int& w);
    Node* _node[2];
    int _weight;
    
    //Node *getNeighbor(Node *n);
    
    //bool operator < (const Edge& rhs) const;
    
};

class Node
{
    
public:
    Node(Obj*);
    void addEdge(Edge *e);
    void addEdge(Node *n);
    //Edge* getEdgeByNeighbor(Node*);
    
    Obj* _obj;
    vector<Edge *> _edges;
};

class Graph
{
    
public:
    Graph();
    ~Graph();
    
    void addEdge(Node* n1, Node* n2, int weight);
    
    
    //Node * getNodeById(const int& id);
    
    //map<int, Node *> nodesMap;
    vector<Node *> _nodes;
    vector<Edge *> _edges;
};
#endif
