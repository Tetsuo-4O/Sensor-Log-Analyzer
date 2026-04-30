#ifndef FLIGHT_H
#define FLIGHT_H

#include <stddef.h>

typedef struct {
	double timeStamp;
	double voltBatt;
	double current;
	double sagComp;
	double imuTemp;
	double acsmX;
	double acsmY;
	double acsmZ;
	double accVib;
	double roll;
	double pitch;
	double baroAlt;
	int hwHealth;
} FlightelementRecord;

typedef struct {
	FlightelementRecord *fpointerRecord;
	size_t count;
	size_t capacity;
	char source_file[256];
} FlightmainRecord;

typedef struct {
	int fanomType;
	double fanomTime;
} Flightanom;

typedef struct {
	double flightDur;
	double vtakeOff;
	double vLanding;
	double vDelta;
	double minvoltBatt;
	double maxvoltSag;
	double maxCurrent;
	double avgCurrent;
	double maximuTemp;
	double avgimuTemp;
	double maxRoll;
	double maxPitch;
	double maxForce;
	double maxVib;
	double avgVib;
	double maxbarAlt;
	double battUsage;
	int hwHealth;

	char source_file[256];

	Flightanom fanom[1024];
	int fanomCount;
	int fwarnCount;
	int fcritCount;
} Flightstats;

FlightmainRecord *extract_flight(const char *filename);
Flightstats process_flightstats(FlightmainRecord *fpointerLog);
void inst_report(Flightstats stats);

#endif
