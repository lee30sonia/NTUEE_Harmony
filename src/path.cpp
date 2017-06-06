#include <vector>
#include <queue>
#include <climits>

bool shortestPath(Point s, Point t, int layer) {

   // initiate the map
   if(!_map[layer])  _map[layer] = new int[_LL.disX(_UR)][_LL.disY(_UR)];
   for(int i=0; i<_LL.disX(_UR)]; i++)
      for(int j=0; j<_LL.disY(_UR)]; j++)
         _map[layer][i][j] = -1;

   Obstacle* it;
   for(unsigned n=0; n<_obstacles[layer].size(); n++) {
      it = _obstacles[layer][0];
      for(int i=it.getLL.x(); i<=it.getUR.x(); i++)
         for(int j=it.getLL.y(); j<=it.getUR.y(); j++)
            _map[layer][i][j] = INT_MAX;
   }

   int current = -1, dis2t, level;
   bool reach = false;
   Point p;
   vector<queue<Point>> dist;
   dist.push_back(queue<Point>());
   s.level(layer) = 0;
   dis[0].push(s);

   while(!reach) {
      current++;
      dist.push_back(queue<Point>());
      while(!dist(current).empty()) {
         p = dist(current).front();
         dist(current).pop();
         dis2t = p.dis(t);
         level = p.level(layer);
         reach = check4short(Point(p.x()-1,p.y()), t, layer, dis2t, level, dist);
         reach = check4short(Point(p.x()+1,p.y()), t, layer, dis2t, level, dist);
         reach = check4short(Point(p.x(),p.y()-1), t, layer, dis2t, level, dist);
         reach = check4short(Point(p.x(),p.y()+1), t, layer, dis2t, level, dist);
      }
   }
   

}

bool check4short(Point p, const Point& t, const int& layer
      const int& dis2t, const int& level, const vector<queue<Point>>& dist) {
   if(p.x()<0 || p.x()>(_LL.disX(_UR))) return false; 
   if(p.y()<0 || p.y()>(_LL.disY(_UR))) return false; 
   if(p.level(layer) >= 0)   return false;
   if(p.dis(t) == 0) return true;
   int level2;
   if(p.dis(t) > dis2t) level2 = level+1;
   else  level2 = level;
   p.level(layer) = level2;
   dist[level2].push(p);
   return false;
}
