#ifndef DISPLAY_H
#define DISPLAY_H

#include "parser.h"
#include "flight.h"
#include "gps.h"

void menuDisplay();
void customMenuDisplay(HardwareStat *hw, TripReport *tr);
void handleCustomMenu(HardwareStat *hw, TripReport *tr);
void hardwareStat(HardwareStat *hw);
void event_log(Flightstats fStats, GPSstats gStats);

#endif
