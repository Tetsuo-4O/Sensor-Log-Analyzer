#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "flight.h"

FlightmainRecord *extract_flight(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if(fp == NULL)
		return NULL;
	FlightmainRecord *fpointerLog = malloc(sizeof(FlightmainRecord));
	fpointerLog->count = 0;
	fpointerLog->capacity = 2000000;
	fpointerLog->fpointerRecord = malloc(2000000 * sizeof(FlightelementRecord));

	strncpy(fpointerLog->source_file, filename, 255);

	char line[4096];
	char *token;
	int index = 0;
	int col_tStamp = -1, col_vBatt = -1, col_curr = -1, col_sag = -1, col_temp = -1, col_accX = -1, col_accY = -1, col_accZ = -1, col_vib = -1, col_roll = -1, col_pitch = -1, col_bAlt = -1, col_hwh = -1;
	fgets(line, sizeof(line), fp);
	line[strcspn(line, "\r\n")] = '\0';
	token = strtok(line, ",");

	while(token != NULL)
	{
		token[strcspn(token, "\r\n")] = '\0';
		if(strcmp(token, "time (us)") == 0)
			col_tStamp = index;
		else if(strcmp(token, "vbat (V)") == 0)
			col_vBatt = index;
		else if(strcmp(token, "amperage (A)") == 0)
			col_curr = index;
		else if(strcmp(token, "sagCompensatedVBat") == 0)
			col_sag = index;
		else if(strcmp(token, "IMUTemperature") == 0)
			col_temp = index;
		else if(strcmp(token, "accSmooth[0]") == 0)
			col_accX = index;
		else if(strcmp(token, "accSmooth[1]") == 0)
			col_accY = index;
		else if(strcmp(token, "accSmooth[2]") == 0)
			col_accZ = index;
		else if(strcmp(token, "accVib") == 0)
			col_vib = index;
		else if(strcmp(token, "attitude[0]") == 0)
			col_roll = index;
		else if(strcmp(token, "attitude[1]") == 0)
			col_pitch = index;
		else if(strcmp(token, "BaroAlt (cm)") == 0)
			col_bAlt = index;
		else if(strcmp(token, "hwHealthStatus") == 0)
			col_hwh = index;
		index++;
		token = strtok(NULL, ",");
	}

	while(fgets(line, sizeof(line), fp) != NULL)
	{
		FlightelementRecord r;
		index = 0;
		token = strtok(line, ",");
		while(token != NULL)
		{
			if(index == col_tStamp)
				r.timeStamp = atof(token);
			else if(index == col_vBatt)
				r.voltBatt = atof(token);
			else if(index == col_curr)
				r.current = atof(token);
			else if(index == col_sag)
				r.sagComp = atof(token) / 100;
			else if(index == col_temp)
				r.imuTemp = atof(token) / 10;
			else if(index == col_accX)
				r.acsmX = atof(token);
			else if(index == col_accY)
				r.acsmY = atof(token);
			else if(index == col_accZ)
				r.acsmZ = atof(token);
			else if(index == col_vib)
				r.accVib = atof(token);
			else if(index == col_roll)
				r.roll = atof(token) / 10;
			else if(index == col_pitch)
				r.pitch = atof(token) / 10;
			else if(index == col_bAlt)
				r.baroAlt = atof(token) / 100;
			else if(index == col_hwh)
				r.hwHealth = atoi(token);
			index++;
			token = strtok(NULL, ",");
		}
		fpointerLog->fpointerRecord[fpointerLog->count++] = r;
	}

	fclose(fp);
	return fpointerLog;
}

Flightstats process_flightstats(FlightmainRecord *fpointerLog)
{
	Flightstats stats;
	double currSum = 0;
	double tempSum = 0;
	double vibSum  = 0;
	double rollSum = 0;
	double pitchSum = 0;

	stats.maxbarAlt   = 0;
	stats.maxForce    = 0;
	stats.minvoltBatt = 9999;
	stats.maxCurrent  = 0;
	stats.maxvoltSag  = 0;
	stats.maximuTemp  = 0;
	stats.maxRoll     = 0;
	stats.maxPitch    = 0;
	stats.maxVib      = 0;
	stats.fanomCount  = 0;
	stats.fwarnCount  = 0;
	stats.fcritCount  = 0;

	int conlowVolt = 0;
	int conhiVib   = 0;
	int conhiTemp  = 0;
	int conhiSag   = 0;
	int concritSag  = 0;
	int concritVolt = 0;

	for(size_t i = 0; i < fpointerLog->count; i++)
	{
		double timeElapsed = (fpointerLog->fpointerRecord[i].timeStamp - fpointerLog->fpointerRecord[0].timeStamp) / 1000000;
		double sagDelta = fpointerLog->fpointerRecord[i].sagComp - fpointerLog->fpointerRecord[i].voltBatt;
		double forceGen = sqrt(
			fpointerLog->fpointerRecord[i].acsmX * fpointerLog->fpointerRecord[i].acsmX +
			fpointerLog->fpointerRecord[i].acsmY * fpointerLog->fpointerRecord[i].acsmY +
			fpointerLog->fpointerRecord[i].acsmZ * fpointerLog->fpointerRecord[i].acsmZ) / 1024;

		if(fpointerLog->fpointerRecord[i].voltBatt > 0 && fpointerLog->fpointerRecord[i].voltBatt < stats.minvoltBatt)
			stats.minvoltBatt = fpointerLog->fpointerRecord[i].voltBatt;
		if(fpointerLog->fpointerRecord[i].current > stats.maxCurrent)
			stats.maxCurrent = fpointerLog->fpointerRecord[i].current;
		if(sagDelta > stats.maxvoltSag)
			stats.maxvoltSag = sagDelta;
		if(forceGen > stats.maxForce)
			stats.maxForce = forceGen;
		if(fpointerLog->fpointerRecord[i].imuTemp > stats.maximuTemp)
			stats.maximuTemp = fpointerLog->fpointerRecord[i].imuTemp;
		if(fpointerLog->fpointerRecord[i].accVib > stats.maxVib)
			stats.maxVib = fpointerLog->fpointerRecord[i].accVib;
		if(fpointerLog->fpointerRecord[i].baroAlt > stats.maxbarAlt)
			stats.maxbarAlt = fpointerLog->fpointerRecord[i].baroAlt;

		double absvalR = fpointerLog->fpointerRecord[i].roll < 0 ?
			-fpointerLog->fpointerRecord[i].roll : fpointerLog->fpointerRecord[i].roll;
		double absvalP = fpointerLog->fpointerRecord[i].pitch < 0 ?
			-fpointerLog->fpointerRecord[i].pitch : fpointerLog->fpointerRecord[i].pitch;

		if(absvalR > stats.maxRoll)  stats.maxRoll  = absvalR;
		if(absvalP > stats.maxPitch) stats.maxPitch = absvalP;

		currSum  += fpointerLog->fpointerRecord[i].current;
		tempSum  += fpointerLog->fpointerRecord[i].imuTemp;
		vibSum   += fpointerLog->fpointerRecord[i].accVib;
		rollSum  += absvalR;
		pitchSum += absvalP;

		// low volt warning
		if(fpointerLog->fpointerRecord[i].voltBatt > 0 && fpointerLog->fpointerRecord[i].voltBatt < 19.8 && fpointerLog->fpointerRecord[i].voltBatt >= 18.0 && conlowVolt == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 1;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conlowVolt = 1;
		}
		else if(fpointerLog->fpointerRecord[i].voltBatt < 18.0 && conlowVolt == 1)
			conlowVolt = 0;
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 19.8 && conlowVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 2;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conlowVolt = 0;
		}

		// critical volt
		if(fpointerLog->fpointerRecord[i].voltBatt > 0 && fpointerLog->fpointerRecord[i].voltBatt < 18.0 && concritVolt == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 3;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++; stats.fcritCount++;
			concritVolt = 1;
		}
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 18.0 && fpointerLog->fpointerRecord[i].voltBatt < 19.8 && concritVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 4;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritVolt = 0;
			stats.fanom[stats.fanomCount].fanomType = 1;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conlowVolt = 1;
		}
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 19.8 && concritVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 4;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritVolt = 0;
		}

		// high vibration
		if(fpointerLog->fpointerRecord[i].accVib >= 10000 && conhiVib == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 5;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conhiVib = 1;
		}
		else if(fpointerLog->fpointerRecord[i].accVib < 10000 && conhiVib == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 6;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiVib = 0;
		}

		// high imu temp
		if(fpointerLog->fpointerRecord[i].imuTemp >= 52 && conhiTemp == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 7;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conhiTemp = 1;
		}
		else if(fpointerLog->fpointerRecord[i].imuTemp < 52 && conhiTemp == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 8;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiTemp = 0;
		}

		// voltage sag warning
		if(sagDelta > 1 && sagDelta < 2 && conhiSag == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 9;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conhiSag = 1;
		}
		else if(sagDelta >= 2 && conhiSag == 1)
			conhiSag = 0;
		else if(sagDelta <= 1 && conhiSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 10;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiSag = 0;
		}

		// critical sag
		if(sagDelta >= 2.0 && concritSag == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 11;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++; stats.fcritCount++;
			concritSag = 1;
		}
		else if(sagDelta < 2 && sagDelta > 1 && concritSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 12;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritSag = 0;
			stats.fanom[stats.fanomCount].fanomType = 9;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++; stats.fwarnCount++;
			conhiSag = 1;
		}
		else if(sagDelta <= 1 && concritSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 12;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritSag = 0;
		}
	}

	stats.flightDur  = (fpointerLog->fpointerRecord[fpointerLog->count - 1].timeStamp - fpointerLog->fpointerRecord[0].timeStamp) / 1000000;
	stats.avgCurrent = currSum / fpointerLog->count;
	stats.avgimuTemp = tempSum / fpointerLog->count;
	stats.avgVib     = vibSum  / fpointerLog->count;
	stats.vtakeOff   = fpointerLog->fpointerRecord[0].voltBatt;
	stats.vLanding   = fpointerLog->fpointerRecord[fpointerLog->count - 1].voltBatt;
	stats.vDelta     = stats.vtakeOff - stats.vLanding;
	stats.battUsage  = (stats.vDelta / (stats.vtakeOff - 18.3)) * 100;
	strncpy(stats.source_file, fpointerLog->source_file, 255);

	return stats;
}

void inst_report(Flightstats stats)
{
	int fdur = (int)stats.flightDur;
	int fMin = fdur / 60;
	int fSec = fdur % 60;

	printf("================================================\n");
	printf("         Onboard instrumentation report         \n");
	printf("================================================\n");
	printf("Log-file: %s\n", stats.source_file);
	printf("\n---------------Flight Overview----------------\n");
	printf("%d Events recorded; see 'EVENT LOG' for more details!\n", stats.fwarnCount);
	printf("Total flight duration: %dm %02ds\n", fMin, fSec);
	printf("----------------Electrical Systems--------------\n");
	printf("Takeoff battery voltage: %.2fV\n", stats.vtakeOff);
	printf("Landing battery voltage: %.2fV\n", stats.vLanding);
	printf("Total battery depletion: %.2f percent\n", stats.battUsage);
	printf("Minimum battery voltage: %.2fV\n", stats.minvoltBatt);
	printf("Maximum voltage sag: %.2fV\n", stats.maxvoltSag);
	printf("Highest measured current: %.2fA\n", stats.maxCurrent);
	printf("Average measured current: %.2fA\n", stats.avgCurrent);
	printf("--------------Processor performance-------------\n");
	printf("Maximum processor temp: %.2fC\n", stats.maximuTemp);
	printf("Average processor temp: %.2fC\n", stats.avgimuTemp);
	printf("------------Environmental flight data-----------\n");
	printf("Maximum experienced G-FORCE: %.2fG\n", stats.maxForce);
	printf("Maximum roll rate: %.2f(degrees/s)\n", stats.maxRoll);
	printf("Maximum pitch rate: %.2f(degrees/s)\n", stats.maxPitch);
	printf("Maximum frame vibration: %.2f units/cm\n", stats.maxVib);
	printf("Average frame vibration: %.2f units/cm\n", stats.avgVib);
	printf("Maximum barometer read: %.2fm\n\n\n", stats.maxbarAlt);
}
