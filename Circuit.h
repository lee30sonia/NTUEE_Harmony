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

class CircuitMgr
{
   CircuitMgr();
   
   // io.cpp
   bool readCircuit(char* filename);
   void writeOutput(char* filename);
   
   // Greedy.cpp
   void greedy();

   
};

class Shape
{

};

class Line
{

};

class Via
{

};

class Obstacle
{

};

#endif