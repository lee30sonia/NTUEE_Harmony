/*******************************************************************************
  FileName     [ Circuit.h ]
  Synopsis     [ Define data structures ]
  Discription  [ Define basic classes to build the circuit,
                 including Point, Shape, Via, Obstacle, Line, and CircuitMgr]
*******************************************************************************/

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>
#include <queue>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "Graph.h"

using namespace std;

class Shape;
class Via;
class Obstacle;
class Line;
class Graph;
class CircuitMgr;

class Node;

enum Type{
   empty, shape, via, obstacle, line
};

#ifndef POINT
#define POINT
class Point
{
public:
   Point(){ }
   Point(int x, int y);
   Point(const Point& p){ _x=p.x(); _y=p.y(); }

   int x() const { return _x; }
   int y() const { return _y; }
   string str() const;
   int disX(const Point& p) const { return abs(_x-p.x()); }
   int disY(const Point& p) const { return abs(_y-p.y()); }
   int disXY(const Point& p) const { return disX(p)+disY(p); }
   //int& level(const int& layer) { return _map[layer][_x][_y]; }
   
   bool inside(Point LL, Point UR, int spacing = 0); //whether this point is inside the rectangle given by LL and UR or its margin of width "spacing"

   //for line iteration
   void move(bool vertical, int dis = 1); //increase y by dis if vertical, x otherwise
   bool move(char dir, Point& ll, Point& ur, int spacing); // u, d, l, r;  check inside boundary
   bool encounter(Obstacle* ob, char dir, int spacing); //for routing
   
   bool operator!=(const Point& p);
   bool operator==(const Point& p);

private:
   int _x;
   int _y;
};
#endif

class Obj
{
public:
   virtual Type type() = 0;  // return the type by an enum. overloaded by each class
   bool _inGraph = false;
};

class Shape :public Obj
{
public:
   Shape(int x1, int y1, int x2, int y2, int layer);

   Type type() { return shape; }
   Point& getLL() { return _LL; }
   Point& getUR() { return _UR; }
   int layer() { return _layer; }
   int dis(const Shape* target) const;
   void setsetNum(short s ) { setNum = s; }
   short getsetNum() const { return setNum; }

   bool connected(Line l); //whether the line is connected to the shape
   bool connected(Point p); //return true if the point is inside the shape, assume the same layer
   bool inside(const Point p, const char d = 'b');
   // d = 'x' only check for x;    d = 'y' only check for y;  d = 'b' check for both

   bool overlapX(const Shape& s); //known: s is more right than this
   bool overlapY(const Shape& s); //known: s is lower than this
   
private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
   short setNum;
};

class Via :public Obj
{
public:
   Via(int x, int y, int layer, bool given=false);

   Type type() { return via; }
   bool given() { return _given; }
   int layer() { return _layer; }
   Point pos() { return _pos; }

private:
   Point _pos;
   int _layer; //at which layer. Vn connects Mn and Mn+1
   int _set; //belongs to which set
   bool _given; //given by problem(no cost)
};

class Obstacle :public Obj
{
public:
   Obstacle(int x1, int y1, int x2, int y2, int layer);

   Type type() { return obstacle; }
   Point& getLL() { return _LL; }
   Point& getUR() { return _UR; }
   int layer() { return _layer; }
   
   bool inside(Point& ll, Point& ur, bool xType, int& spacing); //for buildGraph
   
private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
};

class Line :public Obj
{
public:
   Line() {};
   Line(int x1, int y1, int x2, int y2, int layer);

   Type type() { return line; }
   int layer() { return _layer; }
   bool vertical(); //return true if it is vertical, false if horizontal
   int length();
   Point startpoint() { return _endpoints.at(0); }
   Point endpoint() { return _endpoints.at(1); }

private:
   vector<Point> _endpoints;
   int _layer; //at which layer
   int _set; //belongs to which set
   
};

class Cell
{
   Cell():_object(NULL) {}
   
   void set(Obj* ptr) { _object = ptr; }
   Type check() {
      if(_object) return _object->type();
      return empty;
   }

private:
   Obj* _object;
};

class CircuitMgr
{
public:
   CircuitMgr(){}
   ~CircuitMgr();
   
   // io.cpp
   bool readCircuit(char* filename);
   void writeOutput(char* filename);
   
   // Greedy.cpp
   void greedy();

   //Circuit.cpp
   ////circuit manipulation methods
   void addShape(int x1, int y1, int x2, int y2, int layer);
   bool addLine(int x1, int y1, int x2, int y2, int layer, bool check=true); // check valid or not
   bool addLine(Point p1, Point p2, int layer, bool check=true);
   bool addVia(int x, int y, int layer, bool given=false);
   void addObstacle(int x1, int y1, int x2, int y2, int layer);
   ////checking methods
   bool valid(Point& p, int layer); //return false if not in boundary or inside an obstacle
   bool valid(Line& l); //return false if (part of the line) not in boundary or inside an obstacle
   ////calculation methods
   int  cost(); //return total cost of lines and vias
   
   //Graph.cpp
   Graph* buildGraph(int layer); //build graph for a selected layer
   int dist(Shape& s1, Shape& s2, bool xType, Point* connect); //for buildGraph. return -1 if there is obstacle between them
   bool L_edge(Graph* g, Shape* root, int layer);
   void connectLayer(int l); //add via on layer l to connect l and l+1
   bool routing(Point& p1, Point& p2, int layer);
   
   // path.cpp

   // shortest path
   bool shortestPath(const Point s, const Point t, const int layer);
   void init4short(const Point s, const Point t, int layer);
   bool check4short(const Point& p, const Point& t, const char& dir,
      const int& dis2t, const int& level);
   int& getLevel(const Point& p);
   char& getDir(const Point& p);
   void setLevel(const Point& p, int level); 
   void setDir(const Point& p, char dir);
   

   // Minimun spanning tree
   vector<Node*> mstPrim(const Graph* g, vector<unsigned>& set_sizes); //solve minimum spanning tree 
   void enqueue(vector<Node *>& pQ, Node* n);
   Node* dequeue(vector<Node *>& pQ);
   void decrease_key(vector<Node *>& pQ, int i);
   void increase_key(vector<Node *>& pQ, int i);
   void mstKruskal(const Graph* g);
   void makeSet(Node* a);
   void linkSet(Node* a, Node* b);
   Node* findSet(Node* a);
   void unionSet(Node* a, Node* b);

   // Vias
   void reduce3d(vector<Node*> nodes); //use 3D reduction to connect nodes from different layers
   bool check43d(Node* a, const int layer=1);
   void findSteiner(Node* a); //use Kruskal based algorithm to find the Steiner point from a staring node(?)
   bool sameSet(Node* a, Node* b); //determine if two nodes belong to the same set
   
   // Greedy.cpp
   // other connection
   bool collectRemains(vector<Node*>& roots);
   Shape* findNearest(Shape* target, const short mainSet, short& x, short& y, bool checkset);
   void DeterminePoints(Point& p1, Point& p2, short& x, short& y, Shape* root, Shape* connect);
   bool L_connect(Shape* root, Shape* connect, Point& p1, Point& p2, short& x, short& y);
   
private:
   int _viaCost;
   int _spacing;
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layernum; //how many layers

   //can get by _xxx.at(layer), .at(0) is empty
   vector< vector<Shape*> > _shapes;
   vector< vector<Line*> > _lines;
   vector<Via*> _vias;
   vector< vector<Obstacle*> > _obstacles;

   // these are for function shortestPath
   int**                     _levelMap;
   char**                    _dirMap;
   vector< queue<Point> >     _Q;
   Point                      _mapLL;
   Point                      _mapUR;
};
#endif
