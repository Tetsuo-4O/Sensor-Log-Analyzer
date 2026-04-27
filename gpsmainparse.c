#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gps.h"

GPSmainRecord *extract_gps(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if(fp == NULL)
		return NULL;
	GPSmainRecord *pointerLog = malloc(sizeof(GPSmainRecord));
	pointerLog->count = 0;
	pointerLog->capacity = 100000;
	pointerLog->pointerRecord = malloc(100000 * sizeof(GPSelementRecord));
	strncpy(pointerLog->source_file, filename, 255);

	char line[1024];
	char *token;
	int col_tStamp = -1, col_fixType = -1, col_numSat = -1, col_coord0 = -1, col_coord1 = -1, col_alt = -1, col_speed = -1, col_hdop = -1, col_eph = -1, col_epv = -1, col_ned0 = -1, col_ned1 = -1, col_ned2 = -1, col_hLat = -1, col_hLon = -1;
	int index = 0;
	/*Extracting header*/
	fgets(line, sizeof(line), fp);
	token = strtok(line, ",");
	while(token != NULL)
	{	
		token[strcspn(token, "\r\n")] = '\0'; //setting trailing characters equal to null.
		if(strcmp(token, "time (us)") == 0)
			col_tStamp = index;
		else if(strcmp(token, "GPS_fixType") == 0)
			col_fixType = index;
		else if(strcmp(token, "GPS_numSat") == 0)
			col_numSat = index;
		else if(strcmp(token, "GPS_coord[0]") == 0)
			col_coord0 = index;
		else if(strcmp(token, "GPS_coord[1]") == 0)
			col_coord1 = index;
		else if(strcmp(token, "GPS_altitude") == 0)
			col_alt = index;
		else if(strcmp(token, "GPS_speed (m/s)") == 0)
			col_speed = index;
		else if(strcmp(token, "GPS_hdop") == 0)
			col_hdop = index;
		else if(strcmp(token, "GPS_eph") == 0)
			col_eph = index;
		else if(strcmp(token, "GPS_epv") == 0)
			col_epv = index;
		else if(strcmp(token, "GPS_velned[0]") == 0)
			col_ned0 = index;
		else if(strcmp(token, "GPS_velned[1]") == 0)
			col_ned1 = index;
		else if(strcmp(token, "GPS_velned[2]") == 0)
			col_ned2 = index;
		else if(strcmp(token, "GPS_home_lat") == 0)
			col_hLat = index;
		else if(strcmp(token, "GPS_home_lon") == 0)
			col_hLon = index;
		index++;
		token = strtok(NULL, ",");
	}

	/*troubleshooting seg fault*/
	//printf("time = %d\n", col_tStamp);
	//printf("fix = %d\n", col_fixType);
	//printf("numsat = %d\n", col_numSat);
	//printf("coord0 = %d\n", col_coord0);
	//printf("coord1 = %d\n", col_coord1);
	//printf("alt = %d\n", col_alt);
	//printf("speed = %d\n", col_speed);
	//printf("hdop = %d\n", col_hdop);
	//printf("eph = %d", col_eph);
	//printf("epv = %d\n", col_epv);
	//printf("n0 = %d\n", col_ned0);
	//printf("n1 = %d\n", col_ned1);
	//printf("n2 = %d\n", col_ned2);
	//printf("hlat = %d\n", col_hLat);
	//printf("hLon = %d\n", col_hLon);

	/*Extracting data blocks*/
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		GPSelementRecord r;
		index = 0;
		token = strtok(line, ",");
		while(token != NULL)
		{
			if(index == col_tStamp)
				r.timeStamp = atof(token);
			else if(index == col_fixType)
				r.GPS_fixType = atoi(token);
			else if(index == col_numSat)
				r.GPS_numSat = atoi(token);
			else if(index == col_coord0)
				r.GPS_coord0 = atof(token);
			else if(index == col_coord1)
				r.GPS_coord1 = atof(token);
			else if(index == col_alt)
				r.GPS_altitude = atoi(token);
			else if(index == col_speed)
				r.GPS_speed = atof(token);
			else if(index == col_hdop)
				r.GPS_hdop = atof(token);
			else if(index == col_eph)
				r.GPS_eph = atoi(token);
			else if(index == col_epv)
				r.GPS_epv = atoi(token);
			else if(index == col_ned0)
				r.GPS_velNED0 = atoi(token);
			else if(index == col_ned1)
				r.GPS_velNED1 = atoi(token);
			else if(index == col_ned2)
				r.GPS_velNED2 = atoi(token);
			else if(index == col_hLat)
				r.GPS_home_lat = atof(token);
			else if(index == col_hLon)
				r.GPS_home_lon = atof(token);
			index++;
			token = strtok(NULL, ",");
		}
	pointerLog->pointerRecord[pointerLog->count++] = r;
	}
	fclose(fp);
	return pointerLog;
}		


