#include "gps.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M_PI 3.14159265358979323846
//haversine formula for calculating difference/distance between degree coordinates
//requires some definition of PI and math.h
static double GPShaversine(double lat1, double lon1, double lat2, double lon2)
{
	double earthRadius = 6371000.0;
	double diffLat = (lat2 - lat1) * (M_PI / 180.0);
	double diffLon = (lon2 - lon1) * (M_PI / 180.0);
	double constA = sin(diffLat/2) * sin(diffLat/2)
		+ cos(lat1 * (M_PI/180.0)) * cos(lat2 * (M_PI/180.0))
		* sin(diffLon/2) * sin(diffLon/2);
	return 2.0 * earthRadius * atan2(sqrt(constA), sqrt(1.0 - constA));
}
//timestamp conversion for alerts, warnings, and critical fault reporting
static void timeConversion(double timeElapsed, int *fMin, int *fSec)
{
	int fDur = (int)timeElapsed;
	*fMin = fDur / 60;
	*fSec = fDur % 60;
}

GPSstats process_stats(GPSmainRecord *pointerLog) //function type GPSstats, called process_stats, takes a pointer TO GPSmainRecord. It RETURNS a populated GPSstats variable/value to MAIN, where it is called. 
{
	GPSstats stats; //stats variable declared here as type "GPSstats".
	strncpy(stats.source_file, pointerLog->source_file, 255); //255 to leave room for NULL terminator; source_file is [256] long. 
	double speedSum = 0;
	double hdopSum = 0;
	int satSum = 0;
	//double maxSpeed = 0; These are "REDUNDANT" because we can use stats.field "DIRECTLY"
	//double totalDist = 0; "^"
	double flightDur = (pointerLog->pointerRecord[pointerLog->count - 1].timeStamp - pointerLog->pointerRecord[0].timeStamp) / 1000000;
	int baseAlt = pointerLog->pointerRecord[0].GPS_altitude;
	//baselines for first min and max comparisons
	stats.warnCount = 0;
	stats.critCount = 0;
	stats.maxAlt = 0;
	stats.maxhDist = 0;
	stats.maxSpeed = pointerLog->pointerRecord[0].GPS_speed;
	stats.maxClimb = pointerLog->pointerRecord[0].GPS_velNED2;
	stats.maxDescent = pointerLog->pointerRecord[0].GPS_velNED2;
	stats.maxSat = pointerLog->pointerRecord[0].GPS_numSat;
	stats.minSat = 99999999;
	//Boolean flags for in-flight failure conditions, addl anomaly logic
	int conlowSat = 0;
	int conwarnHDOP = 0;
	int concritHDOP = 0;
	int confixLoss = 0;
	stats.anomCount = 0;
	stats.crashDet = 0;
	stats.susendPos = 0;
	//1 = lowsat begin
	//2 = lowsat recover
	//3 = hdop warn begin
	//4 = hdop warn recover
	//5 = hdop crit begin
	//6 = hdop crit recover
	//7 = fix loss begin
	//8 = fix loss recover
	//anomaly/event-counter//minmax calc loop
	for(size_t i = 0; i < pointerLog->count; i++)
	{
		double timeElapsed = (pointerLog->pointerRecord[i].timeStamp - pointerLog->pointerRecord[0].timeStamp) / 1000000;
		
		if(pointerLog->pointerRecord[i].GPS_numSat < 6 && conlowSat == 0)
		{
			stats.anom[stats.anomCount].anomType = 1;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			stats.warnCount++;
			conlowSat = 1;
		}
		else if(pointerLog->pointerRecord[i].GPS_numSat >= 6 && conlowSat == 1)
		{
			stats.anom[stats.anomCount].anomType = 2;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			conlowSat = 0;
		}
		if(pointerLog->pointerRecord[i].GPS_hdop > 250 && pointerLog->pointerRecord[i].GPS_hdop < 500 && conwarnHDOP == 0)
		{
			stats.anom[stats.anomCount].anomType = 3;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			stats.warnCount++;
			conwarnHDOP = 1;
		}
		else if(pointerLog->pointerRecord[i].GPS_hdop >= 500 && conwarnHDOP == 1)
		{
			conwarnHDOP = 0; //"warning" status cleared upon entry into "critical" status, no "recovery" status yet
			stats.warnCount++;
			stats.critCount++;
		}
		else if(pointerLog->pointerRecord[i].GPS_hdop <= 250 && conwarnHDOP == 1)
		{
			stats.anom[stats.anomCount].anomType = 4;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			conwarnHDOP = 0;
		}
		if(pointerLog->pointerRecord[i].GPS_hdop >= 500 && concritHDOP == 0)
		{
			stats.anom[stats.anomCount].anomType = 5;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			stats.warnCount++;
			stats.critCount++;
			concritHDOP = 1;
		}
		else if(pointerLog->pointerRecord[i].GPS_hdop < 500 && pointerLog->pointerRecord[i].GPS_hdop > 250 && concritHDOP == 1)
		{
			stats.anom[stats.anomCount].anomType = 6;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			concritHDOP = 0;
			stats.anom[stats.anomCount].anomType = 3;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			stats.warnCount++;
			conwarnHDOP = 1;
		}
		else if(pointerLog->pointerRecord[i].GPS_hdop <= 250 && concritHDOP == 1)
		{
			stats.anom[stats.anomCount].anomType = 6;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			concritHDOP = 0;
		}
		if(pointerLog->pointerRecord[i].GPS_fixType < 2 && confixLoss == 0)
		{
			stats.anom[stats.anomCount].anomType = 7;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			stats.warnCount++;
			stats.critCount++;
			confixLoss = 1;
		}
		else if(pointerLog->pointerRecord[i].GPS_fixType >= 2 && confixLoss == 1)
		{
			stats.anom[stats.anomCount].anomType = 8;
			stats.anom[stats.anomCount].anomTime = timeElapsed;
			stats.anomCount++;
			confixLoss = 0;
		}
		
		if(pointerLog->pointerRecord[i].GPS_speed > stats.maxSpeed)
			stats.maxSpeed = pointerLog->pointerRecord[i].GPS_speed;
		if(pointerLog->pointerRecord[i].GPS_velNED2 < stats.maxClimb)
			stats.maxClimb = pointerLog->pointerRecord[i].GPS_velNED2;
		if(pointerLog->pointerRecord[i].GPS_velNED2 > stats.maxDescent)
			stats.maxDescent = pointerLog->pointerRecord[i].GPS_velNED2;
		if(pointerLog->pointerRecord[i].GPS_numSat > stats.maxSat)
			stats.maxSat = pointerLog->pointerRecord[i].GPS_numSat;
		if(pointerLog->pointerRecord[i].GPS_numSat < stats.minSat)
			stats.minSat = pointerLog->pointerRecord[i].GPS_numSat;

		double hDist = GPShaversine(pointerLog->pointerRecord[i].GPS_home_lat, pointerLog->pointerRecord[i].GPS_home_lon, 
				pointerLog->pointerRecord[i].GPS_coord0, pointerLog->pointerRecord[i].GPS_coord1);
			if(hDist > stats.maxhDist)
				stats.maxhDist = hDist;
		int aglAlt = pointerLog->pointerRecord[i].GPS_altitude - baseAlt;
			if(aglAlt > stats.maxAlt)
				stats.maxAlt = aglAlt;
		//summation of variables for average calc using pointer[index] to records struct
		speedSum += pointerLog->pointerRecord[i].GPS_speed;
		hdopSum += pointerLog->pointerRecord[i].GPS_hdop;
		satSum += pointerLog->pointerRecord[i].GPS_numSat;
		//calculates running trip distance (total)
		if(i > 0)
		{
			stats.totalDist += GPShaversine(pointerLog->pointerRecord[i - 1].GPS_coord0, pointerLog->pointerRecord[i - 1].GPS_coord1,
			pointerLog->pointerRecord[i].GPS_coord0, pointerLog->pointerRecord[i].GPS_coord1);
		}
	}
	//average and static value assignments
	stats.avgSpeed = speedSum/pointerLog->count;
	stats.avgHDOP = hdopSum/pointerLog->count;
	stats.avgSat = satSum/pointerLog->count;
	//stats.totalDist = totalDist; "ALSO REDUNDANT"
	stats.flightDur = flightDur;

	double endWindow = pointerLog->pointerRecord[pointerLog->count - 1].timeStamp;
	double startWindow = endWindow - 1000000;
	//downward velocity check at last ~1 second of log
	for(size_t i = pointerLog->count - 1; i > 0; i--)
	{
		if(pointerLog->pointerRecord[i].timeStamp < startWindow)
			break;
		if(pointerLog->pointerRecord[i].GPS_velNED2 > 500)
		{
			stats.crashDet = 1;
			stats.warnCount++;
			stats.critCount++;
			break;
		}
	}
	
	double lastPos = GPShaversine(pointerLog->pointerRecord[0].GPS_home_lat, pointerLog->pointerRecord[0].GPS_home_lon,
	pointerLog->pointerRecord[pointerLog->count - 1].GPS_coord0, pointerLog->pointerRecord[pointerLog->count - 1].GPS_coord1);
	if(lastPos > 50)
	{
		stats.susendPos = 1;
		stats.warnCount++;
		stats.critCount++;
	}

	return stats;	
}

void trip_report(GPSstats stats)
{
	int totalDur = (int)stats.flightDur, fMin = totalDur /60, fSec = totalDur % 60; //cast to int and convert sec into min:sec
	printf("================================================\n");
	printf("             IN-FLIGHT TRIP REPORT              \n");
	printf("================================================\n");
	printf("Log-file: %s\n", stats.source_file);
	printf("\n--------------Flight Overview-----------------\n");
	printf("%d Events recorded; see EVENT LOG for more details!\n", stats.warnCount);
	printf("Total flight duration: %dm %02ds\n", fMin, fSec);
	printf("Total distance flown: %.2f m\n", stats.totalDist);
	printf("Greatest distance from homepoint: %.2f m\n", stats.maxhDist);
	printf("Maximum recorded altitude: %d m\n", stats.maxAlt);
	printf("Maximum rate of climb: %d cm/s\n", stats.maxClimb * -1);
	printf("Maximum rate of descent: %d cm/s\n", stats.maxDescent * -1);
	printf("Maximum rate of speed: %.2f m/s\n", stats.maxSpeed);
	printf("Average flight speed: %.2f m/s\n", stats.avgSpeed);
	printf("Maximum acquired satellites: %d\n", stats.maxSat);
	printf("Minimum acquired satellites: %d\n", stats.minSat);
	printf("Average satellite count: %d\n", stats.avgSat);
	printf("Average HDOP(satellite signal quality): %.2f\n\n", stats.avgHDOP);
}
	
		
	


