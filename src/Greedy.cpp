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
   }
   
   // final optimization (ex. remove redundant lines; replace a long line with two vias)
}

// assume that all the trivial connections have been found
// also assume we got one main set(roots[0]) and other small ones
bool CircuitMgr::collectRemains(vector<Node*>& roots)
{
   short mainSet = roots[0]->_obj->getsetNum();
   short x, y;
   bool joint = true;
   for(int i=1; i<roots.size(); i++) {
      Shape* connect = findNearest(roots[i]->_obj, mainSet, x, y);
      if(!L_connect(roots[i]->_obj, connect, x, y)) {
#ifdef _DEBUG_ON
         cout << "set " << i << " can't be joined." << endl;
#endif
         joint = false;
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

bool CircuitMgr::L_connect(Shape* root, Shape* connect, short& x, short& y)
{
   Point p1, p2, p3, p4;
   int layer = root->layer();
   
   // determine the connection point
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
   

   return false;
}


