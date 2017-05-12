/****************************************************************************
  FileName     [ Circuit.h ]
  Synopsis     [ Define data structures ]
****************************************************************************/

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>

class Shape;
class Line;
class Via;
class Obstacle;
class Point;

class CircuitMgr
{
   CircuitMgr();
   
   // io.cpp
   bool readCircuit(char* filename);
   void writeOutput(char* filename);
   
   // Greedy.cpp
   void greedy();

   //Circuit.cpp
   ////circuit manipulation methods
   void addShape(int x1, int y1, int x2, int y2, int layer);
   bool addLine(int x1, int y1, int x2, int y2, int layer);
   bool addVia(int x, int y, int layer, bool given=false);
   void addObstacle(int x1, int y1, int x2, int y2, int layer);
   ////checking methods
   bool valid(Point p); //return false if not in boundary or inside an obstacle
   bool valid(Line l); //return false if (part of the line) not in boundary or inside an obstacle
   ////calculation methods
   int cost(); //return total cost of lines and vias
   
private:
   int _viaCost;
   int _spacing;
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layernum; //how many layers

   vector<Shape> _shapes;
   vector<Line> _lines;
   vector<Via> _vias;
   vector<Obstacle> _obstacles;
};

class Shape
{
public:
   Shape(int x1, int y1, int x2, int y2, int layer);

   int layer() { return _layer; }

   bool connected(Line l); //whether the line is connected to the shape
   bool connected(Point p); //return true if the point is inside the shape, assume the same layer
   
private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
   int _set; //belongs to which set
};

class Line
{
public:
   Line(int x1, int y1, int x2, int y2, int layer);

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

class Via
{
public:
   Via(int x, int y, int layer, bool given=false);

   bool given() { return _given; }
   int layer() { return _layer; }

private:
   Point _point;
   int _layer; //at which layer. Vn connects Mn and Mn+1
   int _set; //belongs to which set
   bool _given; //given by problem(no cost)
};

class Obstacle
{
public:
   Obstacle(int x1, int y1, int x2, int y2, int layer);

   Point& getLL() { return _LL; }
   Point& getUR() { return _UR; }
   int layer() { return _layer; }

private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
};

class Point
{
public:
   Point(int x, int y);

   int x() { return _x; }
   int y() { return _y; }
   
   bool inside(Point LL, Point UR, int spacing=0); //wheter this point is inside the rectangle given by LL and UR or its margin of width "spacing"

   //for line iteration
   void move(bool vertical); //increase y by 1 if vertical, x otherwise
   bool operator!=(const Point p);

private:
   int _x;
   int _y;
};

#endif
