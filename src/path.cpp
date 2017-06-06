#include <vector>
#include <queue>
#include <climits>
#include "Circuit.h"

using namespace std;

bool CircuitMgr::shortestPath(Point s, Point t, int layer) {
   int current = -1, dis2t, level;
   bool reach = false;
   Point p;
   setLevel(s,layer,0);
   (_Q[0]).push(s);

   while(!reach) {
      current++;
      _Q.push_back(queue<Point>());
      while(!_Q[current].empty()) {
         p = _Q[current].front();
         _Q[current].pop();
         dis2t = p.disXY(t);
         level = level(p, layer);
         reach = check4short(Point(p.x()-1,p.y()), t, layer, 'r', dis2t, level);
         reach = check4short(Point(p.x()+1,p.y()), t, layer, 'l', dis2t, level);
         reach = check4short(Point(p.x(),p.y()-1), t, layer, 'u', dis2t, level);
         reach = check4short(Point(p.x(),p.y()+1), t, layer, 'd', dis2t, level);
      }
   }

   // not yet done
}

void CircuitMgr::init4short(int layer) {
   if(!_levelMap[layer])   _levelMap[layer] = new int*[_LL.disX(_UR)];
   if(!_dirMap[layer])     _dirMap[layer] = new char[_LL.disX(_UR)];
   for(int i=0; i<_LL.disX(_UR); i++) {
      _levelMap[layer][i] = new int[_LL.disY(_UR)];
      _dirMap[layer][i] = new int[_LL.disY(_UR)];
   }
   for(int i=0; i<_LL.disX(_UR); i++)
      for(int j=0; j<_LL.disY(_UR); j++) {
         _levelMap[layer][i][j] = -1;
         _dirMap[layer][i][j] = 0;
      }

   Obstacle* it;
   for(unsigned n=0; n<_obstacles[layer].size(); n++) {
      it = &_obstacles[layer][0];
      for(int i=it->getLL.x(); i<=it->getUR.x(); i++)
         for(int j=it->getLL.y(); j<=it->getUR.y(); j++)
            _levelMap[layer][i][j] = INT_MAX;
   }
   
   _Q.clear();
   _Q.push_back(queue<Point>());
}

bool CircuitMgr::check4short(Point p, const Point& t, const int& layer, const char& dir,
      const int& dis2t, const int& level) {
   if(p.x()<0 || p.x()>(_LL.disX(_UR))) return false; 
   if(p.y()<0 || p.y()>(_LL.disY(_UR))) return false; 
   if(level(p, layer) >= 0)   return false;

   if(dir(p,layer) == 0)  dir(p,layer) = dir;
   if(p.disXY(t) == 0) return true;
   int level2;
   if(p.disXY(t) > dis2t) level2 = level+1;
   else  level2 = level;
   level(p, layer) = level2;
   _Q[level2].push(p);
   return false;
}

