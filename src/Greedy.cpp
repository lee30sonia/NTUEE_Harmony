/****************************************************************************
  FileName     [ Greedy.cpp ]
  Synopsis     [ Implement main algorithm ]
****************************************************************************/

#include "Circuit.h"
#include "Graph.h"
#include <iostream>
#include <vector>
#include <climits>

#ifdef _OMP
#include <omp.h>
#endif
using namespace std;

void CircuitMgr::greedy()
{
   // deal with connection between different layers(via)
   vector<bool> checkVia;
   checkVia.resize(_layernum, false);
   for (int i=0; i<_vias.size(); ++i)
      checkVia[_vias.at(i)->layer()]=true;
   for (int layer=1; layer<_layernum; ++layer)
   {
      if (!checkVia[layer])
         connectLayer(layer);
   }
   
#ifndef _DEBUG_ON
   #pragma omp parallel for
#endif
   for (int layer=1; layer<=_layernum; ++layer)
   {
      // scan for trivial connections and build as a graph
      Graph* g = buildGraph(layer);
      if (g==0) continue;
      if (g->_nodes.size()>0)
      {
         /*
         // merge 0 nodes
         // waiting for debug
#ifdef _DEBUG_ON
         cout << "merging nodes with 0 weight edges..." << endl;
#endif
         for (int i=0; i<g->_edges.size(); ++i)
         {
            if (g->_edges[i]->_weight==0)
            {
               g->mergeNodes(g->_edges[i], i);
               --i; //because this function will delete the current edge
            }
         }
         */
         // find the MST for the graph
         vector<unsigned> set_sizes;
         vector<Node*> roots = mstPrim(g, set_sizes);
         
         // add the MST as actual lines
         for (int i=0; i<g->_nodes.size(); ++i)
         {
            Node* n = g->_nodes.at(i);
            if (n->_pi==0) //root
               continue;
            if (n->_connectEdge->_weight==0) //already overlap
               continue;
            if (!addLine(n->_connectEdge->_connect[0],n->_connectEdge->_connect[1],layer, false))
               // set check to true if debug is needed
               cout<<"Error: trivial connection addLine failed!"<<n->_connectEdge->_connect[0].str()<<n->_connectEdge->_connect[1].str()<<endl;
         }
         
         #ifdef _DEBUG_ON
         cout<<"#disjoint set: "<<set_sizes.size()<<endl;
         for(int i=0; i<set_sizes.size(); i++)  cout << "set " << i << ": " << set_sizes[i] << endl;
         #endif
         // try to connect remaining shapes
         if(roots.size()>1) {
            // make sure the largest set is at roots[0]
            for(int i=1; i<roots.size(); i++)
               if(set_sizes[i]>set_sizes[0]) swap(roots[0], roots[i]);
            // join the remained sets using L_connect
            if(collectRemains(roots)) {
               #ifdef _DEBUG_ON
               cout << "Joint as one set!" << endl;
               #endif
            }
         }
         // run other methods to make single set
      }
      delete g;
      cout<<"layer "<<layer<<" finished."<<endl<<endl;
   }
   
   // final optimization (ex. remove redundant lines; replace a long line with two vias)
}

// assume that all the trivial connections have been found
// also assume we got one main set(roots[0]) and other small ones
bool CircuitMgr::collectRemains(vector<Node*>& roots)
{
   short mainSet = roots[0]->_obj[0]->getsetNum();
   short x, y;
   Point p1, p2;
   bool joint = true;
   for(int i=1; i<roots.size(); i++) {
      Shape* connect = findNearest(roots[i]->_obj[0], mainSet, x, y);
      if(!L_connect(roots[i]->_obj[0], connect, p1, p2, x, y)) {
#ifdef _DEBUG_ON
         cout << "set " << i << " can't be joined by L_connect." << endl;
#endif
         if (!routing(p1, p2, roots[0]->_obj[0]->layer()))
         {
#ifdef _DEBUG_ON
            cout << "set " << i << " can't either be joined by routing." << endl;
#endif
            joint = false;
         }
      }
   }
   return joint;
}

// use LL as reference points
Shape* CircuitMgr::findNearest(Shape* target, const short mainSet, short& x, short& y)
{
   vector<Shape*>& shapes = _shapes.at(target->layer());
   int distance = INT_MAX, temp;
   Shape* result;
   int bound = (target->getLL().y()<target->getUR().disY(_UR))?
               target->getUR().disY(_UR): target->getLL().y();
   // I still can't decide which is faster
   // scan through the whole thing
   for(int i=0; i<shapes.size(); i++) {
      if(shapes[i]->getsetNum() != mainSet)  continue;
      temp = target->getLL().disXY(shapes[i]->getLL());
      if(temp<distance) {
         distance = temp;
         result = shapes[i];
      }
   }

   // sort but scan through part of it
   /*
   sort(shapes.begin(), shapes.end(), compareByX);
   if(target->getLL().x() < target->getUR().disX(_UR)) {    // scan from the left
      for(int i=0; i<shapes.size(); i++) {
         if(shapes[i]->getsetNum() != mainSet)  continue;
         temp = target->getLL().disXY(shapes[i]->getLL());
         if(temp<distance) {
            distance = temp;
            result = shapes[i];
         }
         // stop scanning if too far
         if(temp>1.4*bound && shapes[i]->getLL()>target->getLL()) break;
      }
   }
   else {   // scan from the right
      for(int i=shapes.size()-1; i>=0; i--) {
         if(shapes[i]->getsetNum() != mainSet)  continue;
         temp = target->getLL().disXY(shapes[i]->getLL());
         if(temp<distance) {
            distance = temp;
            result = shapes[i];
         }
         if(temp>1.4*bound && shapes[i]->getLL()<target->getLL()) break;
      }
   }
   */

   x = result->getLL().x()<target->getLL().x()? -1: 1;
   y = result->getLL().y()<target->getLL().y()? -1: 1;

   return result;
}

// determine the connection point
void CircuitMgr::DeterminePoints(Point& p1, Point& p2, short& x, short& y, Shape* root, Shape* connect)
{
   if(x>0 && y>0) {
      p1 = root->getUR();  p2 = connect->getLL();
   }
   else if(x>0 && y<0) {
      p1 = Point(root->getUR().x(), root->getLL().y());
      p2 = Point(connect->getLL().x(), connect->getUR().y());
   }
   else if(x<0 && y>0) {
      p1 = Point(root->getLL().x(), root->getUR().y());
      p2 = Point(connect->getUR().x(), connect->getLL().y());
   }
   else if(x<0 && y<0) {
      p1 = root->getLL();  p2 = connect->getUR();
   }
}

bool CircuitMgr::L_connect(Shape* root, Shape* connect, Point& p1, Point& p2, short& x, short& y)
{
   Point p3, p4;
   int layer = root->layer();
   DeterminePoints(p1,p2,x,y,root,connect);
   
   p3 = Point(p1.x(), p2.y());   // V -> H
   p4 = Point(p2.x(), p1.y());   // H -> V

   Line lineV(p1.x(), p1.y(), p3.x(), p3.y(), layer);
   Line lineH(p3.x(), p3.y(), p2.x(), p2.y(), layer);
   if(valid(lineV) && valid(lineH)) {
      addLine(p1, p3, layer, false);
      addLine(p3, p2, layer, false);
      return true;
   }
   lineV = Line(p4.x(), p4.y(), p2.x(), p2.y(), layer);
   lineH = Line(p1.x(), p1.y(), p4.x(), p4.y(), layer);
   if(valid(lineV) && valid(lineH)) {
      addLine(p1, p4, layer, false);
      addLine(p4, p2, layer, false);
      return true;
   }
   
   // trivial L connect not found
   // look for extented L connect
   
   Point pp1, pp2, c;
   bool V_valid, H_valid;

   // V -> H
   pp1 = p1;   pp2 = p2;
   while(root->inside(pp1,'x') && connect->inside(pp2,'y')) {
      c = Point(pp1.x(), pp2.y());
      lineV = Line(pp1.x(), pp1.y(), c.x(), c.y(), layer);
      lineH = Line(c.x(), c.y(), pp2.x(), pp2.y(), layer);
      V_valid = valid(lineV);
      H_valid = valid(lineH);
      if(V_valid && H_valid) {
         addLine(pp1, c, layer, false);
         addLine(c, pp2, layer, false);
         return true;
      }
      if(!V_valid)   pp1.move(false, x*(-1));
      if(!H_valid)   pp2.move(true, y);
   }

   // H -> V
   pp1 = p1;   pp2 = p2;
   while(root->inside(pp1,'y') && connect->inside(pp2,'x')) {
      c = Point(pp2.x(), pp1.y());
      lineH = Line(pp1.x(), pp1.y(), c.x(), c.y(), layer);
      lineV = Line(c.x(), c.y(), pp2.x(), pp2.y(), layer);
      H_valid = valid(lineH);
      V_valid = valid(lineV);
      if(V_valid && H_valid) {
         addLine(pp1, c, layer, false);
         addLine(c, pp2, layer, false);
         return true;
      }
      if(!H_valid)   pp1.move(true, y*(-1));
      if(!V_valid)   pp2.move(false, x);
   }

   return false;
}

bool CircuitMgr::routing(Point& p1, Point& p2, int layer)
{
   #ifdef _DEBUG_ON
   cout<<"routing..."<<p1.str()<<p2.str()<<endl;
   #endif
   char target_dir[4];
   if(p2.x()>p1.x()) { target_dir[0]='r'; target_dir[2]='l'; }
   else { target_dir[0]='l'; target_dir[2]='r'; }
   if(p2.y()>p1.y()) { target_dir[1]='u'; target_dir[3]='d'; }
   else { target_dir[1]='d'; target_dir[3]='u'; }
   
   vector<char> dir;
   for (int i=0; i<4; ++i) dir.push_back(target_dir[i]);
   
   char goal='f'; // 'f'=fulfill dir[0]
   vector<Point*> visited;
   visited.push_back(new Point(p1));
   Point p(p1);
   bool found;
   int counter=0;
   int bound=100*p1.disXY(p2); // to avoid infinite-loop
   
   while (dir.size()>0 && counter<bound)
   {
      //cout<<p.str();for (int i=0; i<dir.size(); ++i)cout<<dir[i];cout<<" "<<goal<<endl;
      ++counter;
      p.move(dir[0]);
      if (p==p2)
      {
         found=true;
         visited.push_back(new Point(p2));
         break;
      }
      
      for(int i=0; i<visited.size(); ++i)
      {
         if (p==*(visited[i]))
         {
            found=false;
            break;
         }
      }
      
      bool enc=false;
      for (int i=0; i<_obstacles[layer].size(); ++i)
      {
         if (p.encounter(_obstacles[layer][i],dir[0],_spacing))
         {
            enc=true;
            break;
         }
      }
      if (enc)
      {
         Point* pp = new Point(p);
         visited.push_back(pp);
         if (dir.size()==1)
         {
            found=false;
            break;
         }
         
         goal=dir[0];
         
         bool enc2=false;
         for (int i=0; i<_obstacles[layer].size(); ++i)
         {
            if (p.encounter(_obstacles[layer][i],dir[1],_spacing))
            {
               enc2=true;
               break;
            }
         }
         if (enc2)
            dir.erase(dir.begin());
         
         dir.erase(dir.begin());
         continue;
      }
      
      if (goal=='f')
      {
         if (((dir[0]=='r'||dir[0]=='l')&&p.x()==p2.x())||((dir[0]=='u'||dir[0]=='d')&&p.y()==p2.y()))
         {
            Point* pp = new Point(p);
            visited.push_back(pp);
            swap(dir[0],dir[1]);
            swap(dir[2],dir[3]);
            
            bool enc4=false;
            for (int i=0; i<_obstacles[layer].size(); ++i)
            {
               if (p.encounter(_obstacles[layer][i],dir[0],_spacing))
               {
                  enc4=true;
                  break;
               }
            }
            if (enc4)
            {
               goal=dir[0];
               dir.erase(dir.begin());
            }
         }
      }
      else
      {
         Point tmp(p);
         tmp.move(goal);
         bool enc3=false;
         for (int i=0; i<_obstacles[layer].size(); ++i)
         {
            if (tmp.encounter(_obstacles[layer][i],goal,_spacing))
            {
               enc3=true;
               break;
            }
         }
         if (!enc3)
         {
            Point* pp = new Point(p);
            visited.push_back(pp);
            int i;
            for (i=0; i<4; ++i)
               if (target_dir[i]==goal) break;
            if (dir.size()==2-i)
               dir.insert(dir.begin(),target_dir[i+1]);
            dir.insert(dir.begin(),goal);
            
            if (dir.size()==4)
            {
               for (int j=0; j<2; ++j)
               {
                  if ((dir[j]=='r'&&p.x()>p2.x()) || (dir[j]=='l'&&p.x()<p2.x()) || (dir[j]=='u'&&p.y()>p2.y()) || (dir[j]=='d'&&p.y()<p2.y()))
                     swap(dir[j],dir[j+2]);
               }
               goal='f';
            }
            else
               goal=target_dir[i-1];
         }
      }
   }
   
   //for (int i=0; i<visited.size(); ++i)
     // cout<<visited[i]->str()<<endl;
   
   for (int i=0; i<visited.size()-1; ++i)
      addLine(*(visited[i]),*(visited[i+1]),layer);
   
   for (int i=0; i<visited.size(); ++i)
      delete visited[i];
   
   return found;
}


