#ifndef _sampleMenu_
#define _sampleMenu_
#include "MenuManager.h"
#include <avr/pgmspace.h>


enum sampleMenuCommandId
{
  mnuCmdBack = 0,
  mnuCmdItem1,
  mnuCmdItem2,
  mnuCmdItem3,
  mnuCmdItem4
};

PROGMEM const char sampleMenu_back[] = "Back";
PROGMEM const char sampleMenu_exit[] = "Exit";

PROGMEM const char sampleMenu_1[] = "Temperature";
PROGMEM const char sampleMenu_2[] = "pH";
PROGMEM const char sampleMenu_3[] = "Turbidity";
PROGMEM const char sampleMenu_4[] = "Diss. Oxygen";
PROGMEM const MenuItem sampleMenu_Root[] = {{mnuCmdItem1, sampleMenu_1}, {mnuCmdItem2, sampleMenu_2}, {mnuCmdItem3, sampleMenu_3}, {mnuCmdItem4, sampleMenu_4}, {mnuCmdBack, sampleMenu_exit}};

#endif
