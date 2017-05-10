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
   Shape();

   bool connected(int x, int y);
   
private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
   int _set; //belongs to which set
};

class Line
{
public:
   Line();

   bool vertical(); //return true if it is vertical, false if horizontal
   int length();

private:
   vector<Point> _endpoints;
   int _layer; //at which layer
   int _set; //belongs to which set
   
};

class Via
{

private:
   int[2] _point; //{x,y}
   int _layer; //at which layer. Vn connects Mn and Mn+1
   int _set; //belongs to which set
};

class Obstacle
{

private:
   Point _LL; //lower left corner
   Point _UR; //upper right corner
   int _layer; //at which layer
};

class Point
{
public:
   Point(int x, int y);

private:
   int _x;
   int _y;
};

#endif
