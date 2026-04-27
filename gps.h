#ifndef GPS_H
#define GPS_H
#include <stddef.h>
typedef struct
{
	double timeStamp; //time in MICROSECONDS. 
	int GPS_fixType; //1D, 2D and 3D fix. 
	int GPS_numSat; //number of satellites locked, <6 is more reliable
	double GPS_coord0; //first coordinate
	double GPS_coord1; //2nd coordinate
	int GPS_altitude; //altitude in meters; begins at baseline, NOT 0.
	double GPS_speed; //ground speed in m/s
	double GPS_hdop; //gps/satellite signal quality
	int GPS_eph; //horizontal error
	int GPS_epv; //vertical error
	int GPS_velNED0; //velocity NORTH
	int GPS_velNED1; //velocity EAST
	int GPS_velNED2; //velocity DOWN
	double GPS_home_lat; //home point latitude
	double GPS_home_lon; //home point longitude
}
	GPSelementRecord;

typedef struct
{
	GPSelementRecord *pointerRecord; //pointer of type GPSelementRecord. Thanks typedef. 
	size_t count;
	size_t capacity;
	char source_file[256];
}
	GPSmainRecord;
GPSmainRecord *extract_gps(const char *filename); //data extract function declaration

typedef struct
{
	int anomType;
	double anomTime;
}
	GPSanom;
typedef struct
{
	int minSat;
	int maxSat;
	int avgSat;
	double flightDur;
	double maxSpeed;
	double avgSpeed;
	int maxClimb;
	int maxDescent;
	double avgHDOP;
	int maxAlt;
	double maxhDist;
	double totalDist;

	char source_file[256];
	
	GPSanom anom[512];
	int anomCount;
	int warnCount;
	int critCount;
	int crashDet;
	int susendPos;
}
	GPSstats;
GPSstats process_stats(GPSmainRecord *pointerLog); //stat/metric computation function dec
void trip_report(GPSstats stats); //print report function dec

#endif

