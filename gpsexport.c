#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpsexport.h"

int export_kml(GPSmainRecord *pointerLog, GPSstats stats)
{
	//dynamic file-naming
	char kmlTitle[256]; 
	strncpy(kmlTitle, stats.source_file, 255); //255, leaves room for null character
	char *fileExt = strrchr(kmlTitle, '.'); //searching for last . for file extension grab
	if(fileExt != NULL)
		strcpy(fileExt, ".kml"); //designate file kml
	
	FILE *fp = fopen(kmlTitle, "w"); //open file for writing, see "w" arg
	if(fp == NULL)
		return -1;

	//KML BOILERPLATE; tells google earth to expect a kml file
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"); //kml files use xml format
	fprintf(fp, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"); //standard kml format specified in 
									 //kml documentation
	fprintf(fp, "<Document>\n"); //container, holds contents of kml file
	fprintf(fp, "<name>Visual Flight Map</name>\n");

	fprintf(fp, "<Placemark>\n"); //visual element on map
	fprintf(fp, "<name>Visual Flight Map</name>\n");
	fprintf(fp, "<LineString>\n"); //specifies line, not pin/placemark
	fprintf(fp, "<coordinates>\n"); //coordinate data

	for(size_t i = 0; i < pointerLog->count; i++) //loops through coord struct and draws coordinate lines
	{
		int ret = fprintf(fp, "%.7f,%.7f,%d\n", pointerLog->pointerRecord[i].GPS_coord1, pointerLog->pointerRecord[i].GPS_coord0, pointerLog->pointerRecord[i].GPS_altitude); //TS START
		if(ret < 0)
		{
			printf("fprintf failed at record number %zu\n", i);
			break;
		} //TS END
		//fprintf(fp, "%.7f,%.7f,%d\n", pointerLog->pointerRecord[i].GPS_coord1, pointerLog->pointerRecord[i].GPS_coord0, pointerLog->pointerRecord[i].GPS_altitude); //kml expects LONGITUDE before LATITUDE, opposite of what program generally uses
	}

	fprintf(fp, "</coordinates>\n");
	fprintf(fp, "</LineString>\n");
	fprintf(fp, "</Placemark>\n");

	fprintf(fp, "<Placemark>\n");
	fprintf(fp, "<name>Home</name>\n");
	fprintf(fp, "<Point>\n");
	fprintf(fp, "<coordinates>%.7f,%.7f,0</coordinates>\n", pointerLog->pointerRecord[0].GPS_home_lon, pointerLog->pointerRecord[0].GPS_home_lat);
	fprintf(fp, "</Point>\n");
	fprintf(fp, "</Placemark>\n");
	
	for(int i = 0; i < stats.anomCount; i++)
	{
		if(stats.anom[i].anomType == 1 || stats.anom[i].anomType == 3 || stats.anom[i].anomType == 5 || stats.anom[i].anomType == 7)
		{
			size_t closestRec = 0;
			double closestDiff = 99999999;

			for(size_t j = 0; j < pointerLog->count; j++)
			{
				double timeElapsed = (pointerLog->pointerRecord[j].timeStamp - pointerLog->pointerRecord[0].timeStamp) / 1000000;
				double timeDiff = timeElapsed - stats.anom[i].anomTime;
				if(timeDiff < 0)
					timeDiff = -timeDiff;
				if(timeDiff < closestDiff)
				{
					closestDiff = timeDiff;
					closestRec = j;
				}
			}
			
			fprintf(fp, "<Placemark>\n");

			const char *anomString;
			if(stats.anom[i].anomType == 1)
				anomString = "Low satellite count";
			else if(stats.anom[i].anomType == 3)
				anomString = "HDOP warning";
			else if(stats.anom[i].anomType == 5)
				anomString = "HDOP critical";
			else if(stats.anom[i].anomType == 7)
				anomString = "Satellite fix lost";
			else
				anomString = "N/A";

			fprintf(fp, "<name>%s</name>\n", anomString);
			fprintf(fp, "<Point>\n");
			fprintf(fp, "<coordinates>%.7f,%.7f,0</coordinates>\n", pointerLog->pointerRecord[closestRec].GPS_coord1, pointerLog->pointerRecord[closestRec].GPS_coord0);
			fprintf(fp, "</Point>\n");
			fprintf(fp, "</Placemark>\n");
		}
	}
	
	if(stats.crashDet == 1)
	{
		fprintf(fp, "<Placemark>\n");
		fprintf(fp, "<name>CRASH DETECTED</name>\n");
		fprintf(fp, "<Point>\n");
		fprintf(fp, "<coordinates>%.7f,%.7f,0</coordinates>\n", pointerLog->pointerRecord[pointerLog->count - 1].GPS_coord1, pointerLog->pointerRecord[pointerLog->count - 1].GPS_coord0);
		fprintf(fp, "</Point>\n");
		fprintf(fp, "</Placemark>\n");
	}

	fprintf(fp, "</Document>\n");
	fprintf(fp, "</kml>\n");
	fclose(fp);
	
	printf("Flight path KML generated at: %s\n", kmlTitle);
	printf("Drag and drop file into Google Earth for real-world flight demonstration.\n");
	
	return 0;
}





