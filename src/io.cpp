/****************************************************************************
  FileName     [ io.cpp ]
  Synopsis     [ Implement input and output parser ]
****************************************************************************/

#include "Circuit.h"
#include <iostream>
#include <fstream>
using namespace std;

bool CircuitMgr::readCircuit(char* filename)
{
   fstream fin(filename,fstream::in);
   if(!fin) {
      cout << "Error: can not open file " << filename << endl;
      return false;
   }

   int routedShapes, routedVias, obstacles;
   int x1, x2, y1, y2, layer;
   fin.ignore(10);   fin >> _viaCost;  fin.ignore();
   fin.ignore(10);   fin >> _spacing;  fin.ignore();
   fin.ignore(12);   fin >> x1;   fin.ignore();  fin >> y1;
   _LL = Point(x1,y1);
   fin.ignore(3);    fin >> x2;   fin.ignore();  fin >> y2;   fin.ignore(2);
   _UR = Point(x2,y2);
   fin.ignore(15);   fin >> _layernum;    fin.ignore();
   fin.ignore(16);   fin >> routedShapes; fin.ignore();
   fin.ignore(14);   fin >> routedVias;   fin.ignore();
   fin.ignore(13);   fin >> obstacles;   fin.ignore();
   #ifdef _DEBUG_ON
   cout << "reading in data..." << endl;
   #endif
   cout << "input size: shape num = " << routedShapes 
        << ", vias num = " << routedVias
        << ", obstacle num = " << obstacles 
        << ", layer num = " << _layernum
        << ", via cost = " << _viaCost << endl;
   
   for(int i=0; i<=_layernum; ++i){
      vector<Shape*> shapeVec;
      vector<Line*> lineVec;
      vector<Obstacle*> obstacleVec;
      _shapes.push_back(shapeVec);
      _lines.push_back(lineVec);
      _obstacles.push_back(obstacleVec);
   }
   for(int i=0; i<routedShapes; i++) {
      fin.ignore(13);   fin >> layer;
      fin.ignore(2);    fin >> x1;  fin.ignore();  fin >> y1;
      fin.ignore(3);    fin >> x2;  fin.ignore();  fin >> y2;
      fin.ignore(2);
      addShape(x1, y1, x2, y2, layer);
   }
   /*if(routedShapes != _shapes.size()) {
      cout << "Error: input RoutedShapes format error!" << endl;
      return false;
   }*/

   for(int i=0; i<routedVias; i++) {
      fin.ignore(11);   fin >> layer;
      fin.ignore(2);    fin >> x1;
      fin.ignore();     fin >> y1;
      fin.ignore(2);
      addVia(x1, y1, layer, true);
      // also add vias as shapes
      addShape(x1, y1, x1, y1, layer);
      addShape(x1, y1, x1, y1, layer+1);
   }
   if(routedVias != _vias.size()) {
      cout << "Error: input RoutedVias format error!" << endl;
      return false;
   }

   for(int i=0; i<obstacles; i++) {
      fin.ignore(10);   fin >> layer;
      fin.ignore(2);    fin >> x1;  fin.ignore();  fin >> y1;
      fin.ignore(3);    fin >> x2;  fin.ignore();  fin >> y2;
      fin.ignore(2);
      addObstacle(x1, y1, x2, y2, layer);
   }
   /*if(obstacles != _obstacles.size()) {
      cout << "Error: input RoutedShapes format error!" << endl;
      return false;
   }*/
   
   fin.close();
   #ifdef _DEBUG_ON
   cout << "Read in file successfully." << endl << endl;
   #endif
   return true; //return true if read in file succeed
}

void CircuitMgr::writeOutput(char* filename)
{
   fstream fout;
   fout.open(filename, ios::out);
   #ifdef _DEBUG_ON
   cout << "writing output file..." << endl;
   #endif
   
   for(unsigned layer=1; layer<_lines.size(); ++layer){
      for(unsigned i=0; i<_lines.at(layer).size(); i++) {
         fout << (_lines[layer][i]->vertical()? "V": "H") << "-line M" << layer << " "
         << _lines[layer][i]->startpoint().str() << " " << _lines[layer][i]->endpoint().str() << endl;
      }
   }
   
   for(unsigned i=0; i<_vias.size(); i++) {
      if(_vias[i]->given())   continue;
      fout << "Via V" << _vias[i]->layer() << " " << _vias[i]->pos().str() << endl;
   }

   #ifdef _DEBUG_ON
   cout << "file output successfully." << endl;
   #endif
   fout.close();
}
