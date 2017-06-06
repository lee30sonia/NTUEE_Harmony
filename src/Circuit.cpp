/****************************************************************************
  FileName     [ Circuit.cpp ]
  Synopsis     [ Implement basic member functions of circuit classes ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

/********************CircuitMgr*********************/
void CircuitMgr::addShape(int x1, int y1, int x2, int y2, int layer)
{
   Shape s(x1, y1, x2, y2, layer);
   _shapes.at(layer).push_back(s);
}

bool CircuitMgr::addLine(int x1, int y1, int x2, int y2, int layer)
{
   Line l(x1, y1, x2, y2, layer);
   if (!valid(l))
      return false;
   _lines.at(layer).push_back(l);
   return true;
}

bool CircuitMgr::addVia(int x, int y, int layer, bool given)
{
   Point p(x,y);
   if (!valid(p, layer))
      return false;
   Via v(x, y, layer, given);
   _vias.push_back(v);
   return true;
}

void CircuitMgr::addObstacle(int x1, int y1, int x2, int y2, int layer)
{
   Obstacle o(x1, y1, x2, y2, layer);
   _obstacles.at(layer).push_back(o);
}

bool CircuitMgr::valid(Point& p, int layer)
{
   if (!p.inside(_LL,_UR,-1*_spacing))
      return false; //not in boundary
   for (int i=0; i<_obstacles.size(); ++i)
   {
      if (_obstacles.at(i).layer()!=layer)
         continue;
      if (p.inside(_obstacles.at(i).getLL(), _obstacles.at(i).getUR(), _spacing))
         return false;
   }
  return true;
}

bool CircuitMgr::valid(Line& l)
{
   Point p=l.startpoint();
   bool vertical=l.vertical();
   while(p!=l.endpoint())
   {
      if (!valid(p, l.layer()))
         return false;
      p.move(vertical);
   }
   if (!valid(p, l.layer())) //check endpoint
      return false;
   return true;
}

int CircuitMgr::cost()
{
   int c=0;
   for (int i=0; i<_lines.size(); ++i)
      c+=_lines.at(i).length();
   for (int i=0; i<_vias.size(); ++i)
   {
      if (!_vias.at(i).given())
         c+=_viaCost;
   }
   return c;
}

/********************Shape*********************/
Shape::Shape(int x1, int y1, int x2, int y2, int layer)
{
   _LL=Point(x1,y1);
   _UR=Point(x2,y2);
   _layer=layer;
}

bool Shape::connected(Line l)
{
   if (l.layer()!=_layer)
      return false;
   return (connected(l.startpoint()) || connected(l.endpoint()));
}

bool Shape::connected(Point p)
{
   return p.inside(_LL, _UR);
}

bool Shape::compareByX(const Shape& s1, const Shape& s2)
{
   return s1._LL.x()<s2._LL.x();
}

bool Shape::compareByY(const Shape& s1, const Shape& s2)
{
   return s1._UR.y()>s2._UR.y();
}

bool Shape::overlapX(const Shape& s)
{
   return (s._LL.x()<=_UR.x());
}

bool Shape::overlapY(const Shape& s)
{
   return s._UR.y()>=_LL.y();
}

/********************Line*********************/
Line::Line(int x1, int y1, int x2, int y2, int layer)
{
   if (x1!=x2 && y1!=y2)
      cout<<"error:creating a non-vertical and non-horizontal line."<<endl;
   else
   {
      Point p1, p2;
      if (x1<x2 || y1<y2)
      {
         p1 = Point(x1,y1);
         p2 = Point(x2,y2);
      }
      else
      {
         p2 = Point(x1,y1);
         p1 = Point(x2,y2);
      }
      _endpoints.push_back(p1);
      _endpoints.push_back(p2);
      _layer=layer;
   }
}

bool Line::vertical()
{
   if (_endpoints.at(1).x()==_endpoints.at(0).x())
      return true;
   else
      return false;
}

int Line::length()
{
   if (vertical())
      return _endpoints.at(1).y()-_endpoints.at(0).y();
   else
      return _endpoints.at(1).x()-_endpoints.at(0).x();
}

/********************Via*********************/
Via::Via(int x, int y, int layer, bool given)
{
   _pos=Point(x,y);
   _layer=layer;
   _given=given;
}

/********************Obstacle*********************/
Obstacle::Obstacle(int x1, int y1, int x2, int y2, int layer)
{
   _LL=Point(x1,y1);
   _UR=Point(x2,y2);
   _layer=layer;
}

bool inside(Point& ll, Point& ur, bool xType, int& spacing)
{
   if (xType)
      return (_LL.x()<ll.x()+spacing && _UR.x()>ur.x()-spacing && _UR.y()<=ur.y() && _LL.y()>=ll.y())
   else
      return (_LL.y()<ll.y()+spacing && _UR.y()>ur.y()-spacing && _UR.x()<=ur.x() && _LL.x()>=ll.x())
}

/********************Point*********************/
Point::Point(int x, int y)
{
   _x=x;
   _y=y;
}

string Point::str()
{
   stringstream ss;
   ss << "(" << _x << "," << _y << ")";
   return ss.str();
}

bool Point::inside(Point LL, Point UR, int spacing)
{
   if (spacing<=0) // for boundary and shape connection checking
   {
      if (_x<LL._x-spacing || _x>UR._x+spacing)
         return false;
      if (_y<LL._y-spacing || _y>UR._y+spacing)
         return false;
      return true;
   }
   else // for obstacle spacing checking
   {
      if (_x<=LL._x-spacing || _x>=UR._x+spacing)
         return false;
      if (_y<=LL._y-spacing || _y>=UR._y+spacing)
         return false;
      return true;
   }
}

void Point::move(bool vertical)
{
   if (vertical)
      ++_y;
   else
      ++_x;
}

bool Point::operator!=(const Point& p)
{
   return !(_x==p._x && _y==p._y);
}

