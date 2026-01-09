#ifndef ArrayList_h//A super basic arraylist implementation for integers
#define ArrayList_h

#include "Arduino.h"
#include "ScreenControl.h"

#define MAX_SIZE 4500  //the amount of seconds that we can save, 60 x 60 = 1800  for every hour
#define STR_BUFFER_SIZE 4500*4  //How long can the string realistically be, assume 3 digits per point, then plus comma is x4 the MAX_SIZE

static char strHRbuffer[STR_BUFFER_SIZE] = "";
static char strPbuffer[STR_BUFFER_SIZE] = "";


class ArrayList {

  private:
    int size;

  public:
    ArrayList();
    bool add(int value,bool HR);
    char* toStr(bool HR);
    void clear();

};


#endif
