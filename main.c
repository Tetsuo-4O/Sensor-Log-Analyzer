#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "flight.h"
#include "display.h"
#include "gps.h"
#include "gpsexport.h"

typedef struct {
	int    time;
	char   type[32];
	char   data[64];
} Event;

typedef struct {
	Event *events;
	size_t count;
	size_t capacity;
} EventLog;

int main()
{
	LogFile      flightLog = {0};
	LogFile      gpsLog    = {0};
	HardwareStat hw  = {0};
	TripReport   tr  = {0};
	GpsData      gps = {0};
	EventLog     ev  = {0};

	GPSmainRecord    *gpsData    = NULL;
	GPSstats          results;
	FlightmainRecord *flightData = NULL;
	Flightstats       flightResults;

	int choice = 0;

	printf("\n======= Flight Analyzer Ready =======\n");
	if(!openLog(&flightLog, "--- Enter flight log path:\n")) return 1;
	if(!openLog(&gpsLog,    "--- Enter GPS log path:\n"))    return 1;
	printf("----------- Parsing Files.... -----------\n");

	parseFlightLog(&flightLog, &hw, &tr);

	flightData = extract_flight(flightLog.path);
	if(flightData == NULL)
	{
		printf("Failed to parse flight log.\n");
		return 1;
	}

	gpsData = extract_gps(gpsLog.path);
	if(gpsData == NULL)
	{
		printf("Failed to parse GPS log.\n");
		return 1;
	}

	flightResults = process_flightstats(flightData);
	results       = process_stats(gpsData);

	do {
		menuDisplay();

		if(scanf("%d", &choice) != 1)
		{
			printf("\n----------- Invalid choice -----------; ----------- please choose an option from the menu: \n");
			while(getchar() != '\n');
			continue;
		}

		if(choice < 1 || choice > 6)
		{
			printf("\n----------- Invalid choice -----------; ----------- please choose an option from the menu: \n");
			continue;
		}
		
		switch(choice)
		{
			case 1:
				hardwareStat(&hw);
			break;
			case 2:
				event_log(flightResults, results);
			break;
			case 3:
				trip_report(results);
				export_kml(gpsData, results);
			break;
			case 4:
				customMenuDisplay(&hw, &tr);
			break;
			case 5:
				inst_report(flightResults);
			break;
			case 6:
				return 0;
			break;
		}
	} while(choice != 6);

	fclose(flightLog.file_p);
	fclose(gpsLog.file_p);

	return 0;
}
