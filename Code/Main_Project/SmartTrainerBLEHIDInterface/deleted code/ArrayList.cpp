#include <Arduino.h>
#include "ArrayList.h"


ArrayList::ArrayList()
{
    //strbuffer[0] = '\0';  // clear buffer
    snprintf(strHRbuffer, sizeof(strHRbuffer), "%s", "\0");
    snprintf(strPbuffer, sizeof(strPbuffer), "%s", "\0");
    size = 0;
}

bool ArrayList::add(int value, bool HR)
{
  if (HR)//this is probably the dumbest thing ive ever done or seen...but I am tired and gatvol. Il;l fix it tommorow...or something. Actually I just need the one text that already contaoins all
  {
    if (size>=MAX_SIZE)
      return false;

    char temp[12]; // enough for int (-2147483648)
    if (size != 0)
      strcat(strHRbuffer, ",");
    itoa(value, temp, 10);
    strcat(strHRbuffer, temp);
    size++;

    Serial.print("Elapsed minutes: ");
    Serial.println(size/60.0);
  }
  else
  {
    if (size>=MAX_SIZE)
      return false;

    char temp[12]; // enough for int (-2147483648)
    if (size != 0)
      strcat(strPbuffer, ",");
    itoa(value, temp, 10);
    strcat(strPbuffer, temp);
    size++;
  }
  

  return true;
}

void ArrayList::clear()
{
      size = 0;
}

char* ArrayList::toStr(bool HR)
{
  if (HR)
    return strHRbuffer;
  else
    return strPbuffer;
}

      
