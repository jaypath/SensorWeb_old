#include "timesetup.hpp"
#include <TimeLib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <server.hpp>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"time.nist.gov",GLOBAL_TIMEZONE_OFFSET,10800000); //3rd param is offset, 4th param is update frequency
int DSTOFFSET = 0;

char DATESTRING[20]="";



bool checkTime(void) {

  uint32_t td = now(); 
  
  if ( WifiStatus()  && (td>2208992400 || td<1704070800)) return false;
  return true;

}



//Time fcn

void checkDST(void) {

//check if time offset is EST (-5h) or EDT (-4h)
int m = month();
int d = day();
int dow = weekday(); //1 is sunday

  DSTOFFSET = 0;
  if (m > 3 && m < 11) DSTOFFSET = 3600;
  else {
    if (month() == 3) {
      //need to figure out if it is past the second sunday at 2 am
      if (d<8) DSTOFFSET = 0;
      else {
        if (d>13)  DSTOFFSET = 3600; //must be past second sunday... though technically could be the second sunday and before 2 am... not a big error though
        else {
          if (d-dow+1>7) DSTOFFSET = 3600; //d-dow+1 is the date of the most recently passed sunday. if it is >7 then it is past the second sunday
          else DSTOFFSET = 0;
        }
      }
    }

    if (month() == 11) {
      //need to figure out if it is past the first sunday at 2 am
      if (d>7)  DSTOFFSET = 3600; //must be past first sunday... though technically could be the second sunday and before 2 am... not a big error though
      else {
        if ((int) d-dow+1>1) DSTOFFSET = 3600; //d-dow+1 is the date of the most recently passed sunday. if it is >1 then it is past the first sunday
        else DSTOFFSET = 0;
      }
    }
  }

  setTime(timeClient.getEpochTime()+GLOBAL_TIMEZONE_OFFSET+DSTOFFSET);

}


String fcnDOW(time_t t) {
    if (weekday(t) == 1) return "Sun";
    if (weekday(t) == 2) return "Mon";
    if (weekday(t) == 3) return "Tue";
    if (weekday(t) == 4) return "Wed";
    if (weekday(t) == 5) return "Thu";
    if (weekday(t) == 6) return "Fri";
    if (weekday(t) == 7) return "Sat";

    return "???";
}

char* dateify(time_t t, String dateformat) {
  if (t==0) t = now();

  char holder[5] = "";

  snprintf(holder,4,"%02d",month(t));
  dateformat.replace("mm",holder);
  
  snprintf(holder,4,"%02d",day(t));
  dateformat.replace("dd",holder);
  
  snprintf(holder,4,"%02d",year(t));
  dateformat.replace("yyyy",holder);
  
  snprintf(holder,4,"%02d",year(t)-2000);
  dateformat.replace("yy",holder);
  
  snprintf(holder,4,"%02d",hour(t));
  dateformat.replace("hh",holder);

  snprintf(holder,4,"%02d",minute(t));
  dateformat.replace("nn",holder);

  snprintf(holder,4,"%02d",second(t));
  dateformat.replace("ss",holder);
  
  snprintf(DATESTRING,19,"%s",dateformat.c_str());
  
  return DATESTRING;  
}

