/****************************************************************************
  FileName     [ Circuit.cpp ]
  Synopsis     [ Implement basic member functions of circuit classes ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

/********************CircuitMgr*********************/
CircuitMgr::~CircuitMgr()
{
   for (int i=0; i<_shapes.size(); ++i)
   {
      for (int j=0; j<_shapes[i].size(); ++j)
         delete _shapes[i][j];
   }
   for (int i=0; i<_lines.size(); ++i)
   {
      for (int j=0; j<_lines[i].size(); ++j)
         delete _lines[i][j];
   }
   for (int i=0; i<_obstacles.size(); ++i)
   {
      for (int j=0; j<_obstacles[i].size(); ++j)
         delete _obstacles[i][j];
   }
   for (int i=0; i<_vias.size(); ++i)
      delete _vias[i];
}

void CircuitMgr::addShape(int x1, int y1, int x2, int y2, int layer)
{
   Shape* s = new Shape(x1, y1, x2, y2, layer);
   _shapes.at(layer).push_back(s);
}

bool CircuitMgr::addLine(int x1, int y1, int x2, int y2, int layer, bool check)
{
   if(x1==x2 && y1==y2) return false;
   Line* l = new Line(x1, y1, x2, y2, layer);
   if(check)
      if (!valid(*l))
      {
         #ifdef _DEBUG_ON
         cout<<"Line added is invalid! "<<x1<<","<<y1<<" "<<x2<<","<<y2<<endl;
         #endif
         return false;
      }
   _lines.at(layer).push_back(l);

   #ifdef _DEBUG_ON
   //cout << "Line" << l->startpoint().str() << l->endpoint().str()<< "on layer" << layer << " added." << endl;
   #endif

   return true;
}

bool CircuitMgr::addLine(Point p1, Point p2, int layer, bool check)
{
   return addLine(p1.x(), p1.y(), p2.x(), p2.y(), layer, check);
}


bool CircuitMgr::addVia(int x, int y, int layer, bool given)
{
   Point p(x,y);
   if (!valid(p, layer)||!valid(p, layer+1))
      return false;

   Via* v = new Via(x, y, layer, given);
   _vias.push_back(v);
   return true;
}

void CircuitMgr::addObstacle(int x1, int y1, int x2, int y2, int layer)
{
   Obstacle* o = new Obstacle(x1, y1, x2, y2, layer);
   _obstacles.at(layer).push_back(o);
}

bool CircuitMgr::valid(Point& p, int layer)
{
   if (!p.inside(_LL,_UR,-1*_spacing))
      return false; //not in boundary
   vector<Obstacle*>& obstacles = _obstacles.at(layer);
   for (int i=0; i<obstacles.size(); ++i)
   {
      if (p.inside(obstacles.at(i)->getLL(), obstacles.at(i)->getUR(), _spacing))
         return false;
   }
  return true;
}

bool compareByX_O(Obstacle *s1, Obstacle *s2);
 //  return s1->getLL().x()<s2->getLL().x();

bool CircuitMgr::valid(Line& l)
{
   Point p=l.startpoint();
   // bool vertical = l.vertical();
   vector<Obstacle*>& obstacles = _obstacles.at(l.layer());

   // sort and check through all the obstacles once
   // might be faster
   sort(obstacles.begin(), obstacles.end(), compareByX_O);
   for(int i=0; i<obstacles.size(); i++) {
      if(obstacles[i]->getUR().x()+_spacing<=l.startpoint().x()) continue;
      if(obstacles[i]->getLL().x()-_spacing>=l.endpoint().x()) break;
      if(obstacles[i]->getLL().y()-_spacing<l.endpoint().y() &&
         obstacles[i]->getUR().y()+_spacing>l.startpoint().y())  return false;
   }

   // check through all the obstacles for every point on the line
  /* 
   while(p!=l.endpoint())
   {
      if (!valid(p, l.layer()))
         return false;
      p.move(vertical);
   }
   if (!valid(p, l.layer())) //check endpoint
      return false;
    */  
   return true;
}

int CircuitMgr::cost()
{
   int c=0;
   for (int layer=1; layer<_lines.size(); ++layer)
   {
      for (int i=0; i<_lines.at(layer).size(); ++i)
         c+=_lines.at(layer).at(i)->length();
   }
   
   for (int i=0; i<_vias.size(); ++i)
   {
      if (!_vias.at(i)->given())
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
   setNum = -1;
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

bool Shape::overlapX(const Shape& s)
{
   return (s._LL.x()<=_UR.x());
}

bool Shape::inside(const Point p, const char d)
{
   if(d != 'y')  // check for x
      if(p.x()<_LL.x() || p.x()>_UR.x())  return false;
   if(d != 'x')  // check for y
      if(p.y()<_LL.y() || p.y()>_UR.y())  return false;
   return true;
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

bool Obstacle::inside(Point& ll, Point& ur, bool xType, int& spacing)
{
   if (xType)
      return (_LL.x()<ll.x()+spacing && _UR.x()>ur.x()-spacing && _UR.y()<=ur.y() && _LL.y()>=ll.y());
   else
      return (_LL.y()<ll.y()+spacing && _UR.y()>ur.y()-spacing && _UR.x()<=ur.x() && _LL.x()>=ll.x());
}

/********************Point*********************/
Point::Point(int x, int y)
{
   _x=x;
   _y=y;
}

string Point::str() const
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

void Point::move(bool vertical, int dis)
{
   if (vertical)
      _y+=dis;
   else
      _x+=dis;
}

bool Point::move(char dir)
{
   if(dir == 'u') {
      _y++; return true;
   }
   if(dir == 'd') {
      _y--; return true;
   }
   if(dir == 'r') {
      _x++; return true;
   }
   if(dir == 'l') {
      _x--; return true;
   }
   return false;
}

bool Point::encounter(Obstacle* ob, char dir, int spacing)
{
   if (dir=='u')
      return (ob->getLL().x()-spacing<_x && ob->getUR().x()+spacing>_x && ob->getLL().y()-spacing<=_y);
   if (dir=='d')
      return (ob->getLL().x()-spacing<_x && ob->getUR().x()+spacing>_x && ob->getUR().y()+spacing>=_y);
   if (dir=='r')
      return (ob->getLL().y()-spacing<_y && ob->getUR().y()+spacing>_y && ob->getLL().x()-spacing<=_x);
   if (dir=='l')
      return (ob->getLL().y()-spacing<_y && ob->getUR().y()+spacing>_y && ob->getUR().x()+spacing>=_x);
   cout<<"Error: encounter() dir error"<<endl;
   return false;
}

bool Point::operator!=(const Point& p)
{
   return !(_x==p._x && _y==p._y);
}

bool Point::operator==(const Point& p)
{
   return (_x==p._x && _y==p._y);
}
