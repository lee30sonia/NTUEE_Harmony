#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include "Circuit.h"

using namespace std;

bool CircuitMgr::shortestPath(const Point s, const Point t, const int layer) {
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
   while(p != s) {
      if(!p.move(lastDir)) {
         cout << "Connection Error!" << endl;
         break;
      }
      if(getDir(p, layer) != lastDir) {
         addLine(start.x(), start.y(), p.x(), p.y(), layer);
         lastDir = getDir(p, layer);
      }
   }
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
      it = &_obstacles[layer][0];
      for(int i=it->getLL().x(); i<=it->getUR().x(); i++)
         for(int j=it->getLL().y(); j<=it->getUR().y(); j++)
            _levelMap[layer][i][j] = INT_MAX;
   }
   
   // initiate the queue vector
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
}

