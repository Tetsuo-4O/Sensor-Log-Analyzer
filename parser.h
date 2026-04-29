#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stddef.h>

typedef struct {
	FILE *file_p;
	char path[256];
} LogFile;

typedef struct {
	double vBatt;
	double iAmp;
	double sagVbatt;
	double imuTemp;
	double accVib;
} HardwareStat;

typedef struct {
	double axisRoll;
	double axisPitch;
	double baroAlt;
	double attitude;
	double healthStat;
} TripReport;

typedef struct {
	double latitude;
	double longitude;
	double gpsAlt;
} GpsData;

typedef struct {
	const char *header;
	size_t      offset;
} ColumnMap;

int  openLog(LogFile *log, const char *prompt);
void parseCsv(LogFile *log, const ColumnMap *maps[], void *targets[], size_t mapCounts[], size_t numStructs);
void parseFlightLog(LogFile *log, HardwareStat *hw, TripReport *tr);
void parseGpsLog(LogFile *log, GpsData *gps);

#endif
