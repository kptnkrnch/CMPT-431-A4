/**
 * Lane class. Implements a shooting gallery lane for 2 users.
 * Not thread safe and requires external synchronization to manipulate lanes.
 * If you want add more than 2 users. Add to the Colors and
 */

#ifndef LANES_H
#define LANES_H

#include <iostream>
#include <iomanip>
#include <ostream>
#include <assert.h>
#include "helper.h"

#define LANE_COUNT 8


/**
 * Thanks to Skurmedel for this code.
 */

enum Color
{
  white = 0,
  red,
  blue,
  violet
};

namespace Colors {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}


class Lanes
{
public:
  // Constructors
  Lanes(int nlanes, int rounds);
  Lanes(const Lanes &from);
  ~Lanes();

  // Obtain color in specific lane
  Color Set(int index,Color c);

  // Set color in specific lane
  Color Get(int index);

  // Return number of lanes
  int Count();
  // print color of lane.
  void Print();

  //make sure we still have rounds
  bool hasRounds();

  //cleaner specific member functions
  void Clear();
  bool allDirty();
  bool hasViolet();

private:
  Color* lanes;
  int nlanes;
  int dirty_lanes;
  bool has_violet;
  int rounds;
  /* data */
};

Lanes::Lanes(int nlanes, int rounds) : nlanes(nlanes), rounds(rounds)
{
  lanes = new Color[nlanes];
  dirty_lanes = 0;
  has_violet = false;
}

Lanes::~Lanes()
{
  delete[] lanes;
}

Lanes::Lanes(const Lanes& from)
{
  nlanes = from.nlanes;
  has_violet = from.has_violet;
  dirty_lanes = from.dirty_lanes;

  lanes = new Color[nlanes];
  for (int i = 0; i < nlanes; ++i)
  {
    lanes[i] = from.lanes[i];
    /* code */
  }
}

Color Lanes::Set(int index, Color c)
{
  // You should be only trying to set red or blue.
  assert ((c == red) || (c == blue));

  // If violet you are already in trouble
  if (lanes[index] == violet) {
    has_violet = true;
    return violet;
  } 

  // If another color then a thread beat you. Figure out why you got here
  // in the first place. You shouldn't be trying to shoot a lane that has already
  // been shot by someone else OR shot by yourself earlier.
  if (lanes[index] != white)
  {
    Color OldColor = lanes[index];
    lanes[index] = (lanes[index] == c)? c : violet ;

    if(lanes[index] == violet) {
      has_violet = true;
    }

    return OldColor;
  }

  // If I got here then I am sure my lane is white.

  lanes[index] = c;
  dirty_lanes++;

  return white;
}

Color Lanes::Get(int index)
{

  return lanes[index];
}

bool Lanes::hasRounds() {
  return rounds > 0;
}

bool Lanes::hasViolet() {
  return has_violet;
}

bool Lanes::allDirty() {
  return dirty_lanes >= nlanes;
}

int Lanes::Count()
{

  return nlanes;
}


void Lanes::Clear()
{
  for (int i = 0; i < nlanes; ++i)
  {
    lanes[i] = white;
  }
  has_violet = false;
  dirty_lanes = 0;
  rounds--;
}


void Lanes::Print()
{
  Colors::Modifier reds(Colors::BG_RED);
    Colors::Modifier def(Colors::BG_DEFAULT);
    Colors::Modifier blues(Colors::BG_BLUE);
    Colors::Modifier greens(Colors::BG_GREEN);

    for (int i = 0; i < nlanes; i++)
      std::cout << "|------";

    std::cout<<std::endl;
    std::cout << "|" ;



    for (int i = 0; i < nlanes; ++i)
    {


      switch (lanes[i])
      {
       case red: std::cout << reds << "##RD##";break;
       case blue: std::cout << blues << "##BL##";break;
       case violet:std::cout << greens << "##VI##";break;
       case white: std::cout << def << "##WH##";break;
       default:
        std::cout<<"Error";
      }
      std::cout << def << "|" ;
    }
    std::cout<<std::endl;
    for (int i = 0; i < nlanes; i++)
      std::cout << "|------";

    std::cout<<std::endl;


}



#endif
