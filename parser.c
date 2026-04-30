#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "parser.h"

#define HW_COL_COUNT  (sizeof(hardwareColumns)/sizeof(hardwareColumns[0]))
#define TR_COL_COUNT  (sizeof(tripColumns)/sizeof(tripColumns[0]))
#define GPS_COL_COUNT (sizeof(gpsColumns)/sizeof(gpsColumns[0]))

static const ColumnMap hardwareColumns[] = {
	{"vbat (V)",           offsetof(HardwareStat, vBatt)},
	{"amperage (A)",       offsetof(HardwareStat, iAmp)},
	{"sagCompensatedVBat", offsetof(HardwareStat, sagVbatt)},
	{"IMUTemperature",     offsetof(HardwareStat, imuTemp)},
	{"accVib",             offsetof(HardwareStat, accVib)},
};

static const ColumnMap tripColumns[] = {
	{"attitude[0]",   offsetof(TripReport, axisRoll)},
	{"attitude[1]",   offsetof(TripReport, axisPitch)},
	{"BaroAlt (cm)",  offsetof(TripReport, baroAlt)},
	{"attitude[0]",   offsetof(TripReport, attitude)},
	{"hwHealthStatus",offsetof(TripReport, healthStat)},
};

static const ColumnMap gpsColumns[] = {
	{"GPS_coord[0]", offsetof(GpsData, latitude)},
	{"GPS_coord[1]", offsetof(GpsData, longitude)},
	{"GPS_altitude",  offsetof(GpsData, gpsAlt)},
};

int openLog(LogFile *log, const char *prompt)
{
	do {
		printf("%s", prompt);
		if(!fgets(log->path, sizeof(log->path), stdin))
			return 0;
		log->path[strcspn(log->path, "\r\n")] = '\0';
		log->file_p = fopen(log->path, "r");
		if(!log->file_p)
			printf("----------- could not open %s -----------\n", log->path);
	} while(log->file_p == NULL);
	return 1;
}

void parseCsv(LogFile *log, const ColumnMap *maps[], void *targets[], size_t mapCounts[], size_t numStructs)
{
	char line[4096];

	if(!fgets(line, sizeof(line), log->file_p)) return;
	line[strcspn(line, "\r\n")] = '\0';

	int *colIndex[16];
	for(size_t s = 0; s < numStructs; s++) {
		colIndex[s] = malloc(mapCounts[s] * sizeof(int));
		for(size_t i = 0; i < mapCounts[s]; i++) colIndex[s][i] = -1;
	}

	int idx = 0;
	char *tok = strtok(line, ",");
	while(tok) {
		for(size_t s = 0; s < numStructs; s++) {
			for(size_t i = 0; i < mapCounts[s]; i++) {
				if(strcmp(tok, maps[s][i].header) == 0)
					colIndex[s][i] = idx;
			}
		}
		idx++;
		tok = strtok(NULL, ",");
	}

	while(fgets(line, sizeof(line), log->file_p)) {
		line[strcspn(line, "\r\n")] = '\0';
		char *tokens[512];
		int n = 0;
		char *p = strtok(line, ",");
		while(p && n < 512) { tokens[n++] = p; p = strtok(NULL, ","); }

		for(size_t s = 0; s < numStructs; s++) {
			for(size_t i = 0; i < mapCounts[s]; i++) {
				int c = colIndex[s][i];
				if(c >= 0 && c < n) {
					double val = atof(tokens[c]);
					*(double *)((char *)targets[s] + maps[s][i].offset) = val;
				}
			}
		}
	}

	for(size_t s = 0; s < numStructs; s++) free(colIndex[s]);
}

void parseFlightLog(LogFile *log, HardwareStat *hw, TripReport *tr)
{
	const ColumnMap *maps[]   = { hardwareColumns, tripColumns };
	void            *targets[]= { hw, tr };
	size_t           counts[] = { HW_COL_COUNT, TR_COL_COUNT };
	parseCsv(log, maps, targets, counts, 2);
}

void parseGpsLog(LogFile *log, GpsData *gps)
{
	const ColumnMap *maps[]   = { gpsColumns };
	void            *targets[]= { gps };
	size_t           counts[] = { GPS_COL_COUNT };
	parseCsv(log, maps, targets, counts, 1);
}
