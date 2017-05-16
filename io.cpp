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
   fstream fin("filename");
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
   fin.ignore(13);   fin >> obstacless;   fin.ignore();

   for(int i=0; i<routedShapes; i++) {
      fin.ignore(13);   fin >> layer;
      fin.ignore(2);    fin >> x1;  fin.ignore();  fin >> y1;
      fin.ignore(3);    fin >> x2;  fin.ignore();  fin >> y2;
      fin.ignore(2);
      addShape(x1, y1, x2, y2, layer);
   }
   if(routedShapes != _shapes.size()) {
      cout << "Error: input RoutedShapes format error!" << endl;
      return false;
   }

   for(int i=0; i<routedVias; i++) {
      fin.ignore(11);   fin >> layer;
      fin.ignore(2);    fin >> x1;
      fin.ignore();     fin >> y1;
      fin.ignore(2);
      addVia(x1, y1, layer, true);
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
      Obstacle(x1, y1, x2, y2, layer);
   }
   if(obstacles != _obstacles.size()) {
      cout << "Error: input RoutedShapes format error!" << endl;
      return false;
   }
   
   fin.close();
   return true; //return true if read in file succeed
}

void CircuitMgr::writeOutput(char* filename)
{
   fstream fout();
   fout.open(filename, ios::out)
   
   for(unsigned i=0; i<_lines.size(); i++) {
      fout << _lines[i].vertical()? 'V': 'H' << "-line M" << _lines[i].layer << " (" 
           << _lines[i].x1 << "," << _lines[i].y1 << ") ("
           << _lines[i].x2 << "," << _lines[i].y2 << ")" << endl;
   }
   for(unsigned i=0; i<_vias.size(); i++) {
      if(_vias[i].given)   continue;
      fout << "Via V" << _vias[i].layer << " ("
           << _vias[i].x1 << "," << _vias[i].y1 << ") ("
           << _vias[i].x2 << "," << _vias[i].y2 << ")" << endl;
   }

   fout.close();
}
