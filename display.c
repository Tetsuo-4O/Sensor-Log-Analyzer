#include <stdio.h>
#include "display.h"

void menuDisplay()
{
	printf(" Fight Log analyzer ready\n");
	printf(" please choose one of the options:\n");
	printf("1. HardwareStat\n");
	printf("2. EventLog\n");
	printf("3. Trip Report\n");
	printf("4. Custom Data\n");
	printf("5. Instrument Report\n");
	printf("6. Exit\n");
}

void customMenuDisplay(HardwareStat *hw, TripReport *tr)
{
	printf("\n==== Custom Data Menu ====\n");
	handleCustomMenu(hw, tr);
}

void handleCustomMenu(HardwareStat *hw, TripReport *tr)
{
	int selected[20];
	int count = 0;
	int choice;

	printf(" Enter choices (-1 to finish):\n");

	while(1)
	{
		printf("\n");
		printf("1. Axis Roll     2. Axis Pitch     3. Battery Voltage  4. Current\n");
		printf("\n5. Voltage Sag   6. Baro Altitude  7. Accel Vibration  8. Attitude\n");
		printf("\n9. Health Status 10. IMU Temperature\n");
		printf("Choice: ");

		scanf("%d", &choice);

		if(choice == -1)
			break;
		if(choice >= 1 && choice <= 10)
			selected[count++] = choice;
		else
			printf("Invalid choice\n");
	}

	if(count == 0)
	{
		printf("No selections made.\n");
		return;
	}

	printf("\n--- Selected Data ---\n");

	for(int i = 0; i < count; i++)
	{
		switch(selected[i])
		{
			case 1:  printf("\n-- Axis Roll = %.2f\n",        tr->axisRoll);   break;
			case 2:  printf("\n-- Axis Pitch = %.2f\n",       tr->axisPitch);  break;
			case 3:  printf("\n-- Battery Voltage = %.2f\n",  hw->vBatt);      break;
			case 4:  printf("\n-- Current = %.2f A\n",        hw->iAmp);       break;
			case 5:  printf("\n-- Voltage Sag = %.2f\n",      hw->sagVbatt);   break;
			case 6:  printf("\n-- Baro Altitude = %.2f\n",    tr->baroAlt);    break;
			case 7:  printf("\n-- Accel Vibration = %.2f\n",  hw->accVib);     break;
			case 8:  printf("\n-- Attitude = %.2f\n",         tr->attitude);   break;
			case 9:  printf("\n-- Health = %.2f\n",           tr->healthStat); break;
			case 10: printf("\n-- IMU Temp = %.2f\n",         hw->imuTemp);    break;
		}
	}
	printf("--------------------\n");
}

void hardwareStat(HardwareStat *hw)
{
	printf("\n--- Hardware Status ---\n");
	printf("Battery Voltage  : %.2f V\n", hw->vBatt);
	printf("Current Draw     : %.2f A\n", hw->iAmp);
	printf("Voltage Sag      : %.2f V\n", hw->sagVbatt / 100.0);
	printf("IMU Temperature  : %.1f C\n", hw->imuTemp  / 10.0);
	printf("Accel Vibration  : %.2f\n",   hw->accVib);
	printf("-----------------------\n");
}

void event_log(Flightstats fStats, GPSstats gStats)
{
	int fMin, fSec;

	printf("================================================\n");
	printf("              Instrument Event Log:             \n");
	printf("================================================\n");
	printf("Events recorded: %d warnings (%d critical)\n", fStats.fwarnCount, fStats.fcritCount);
	printf("\n---------Onboard Instrument Events-----------\n");
	if(fStats.fanomCount == 0)
		printf("No recorded events.\n");
	else
	{
		for(int i = 0; i < fStats.fanomCount; i++)
		{
			int fdur = (int)fStats.fanom[i].fanomTime;
			fMin = fdur / 60;
			fSec = fdur % 60;
			if(fStats.fanom[i].fanomType == 1)
				printf("[%d:%02d] Low battery voltage WARNING\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 2)
				printf("[%d:%02d] Low battery warning resolved\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 3)
				printf("[%d:%02d] Low battery voltage CRITICAL\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 4)
				printf("[%d:%02d] Low battery critical status resolved\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 5)
				printf("[%d:%02d] High vibration WARNING\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 6)
				printf("[%d:%02d] High vibration resolved\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 7)
				printf("[%d:%02d] High IMU temp WARNING\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 8)
				printf("[%d:%02d] High IMU temp resolved\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 9)
				printf("[%d:%02d] Voltage sag WARNING\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 10)
				printf("[%d:%02d] Voltage sag warning resolved\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 11)
				printf("[%d:%02d] Voltage sag CRITICAL\n", fMin, fSec);
			else if(fStats.fanom[i].fanomType == 12)
				printf("[%d:%02d] Voltage sag critical status resolved\n", fMin, fSec);
		}
	}

	printf("================================================\n");
	printf("                 GPS Event Log:                 \n");
	printf("================================================\n");
	printf("Events recorded: %d warnings (%d critical)\n", gStats.warnCount, gStats.critCount);
	printf("\n----------------GPS Events--------------------\n");
	if(gStats.anomCount == 0 && gStats.crashDet == 0 && gStats.susendPos == 0)
		printf("No recorded events.\n");
	if(gStats.crashDet == 1)
		printf("CRASH DETECTED: ABNORMAL TERMINAL RATE OF DESCENT!\n");
	if(gStats.susendPos == 1)
		printf("WARNING: Last known position =/= home position!\n");
	if(gStats.anomCount > 0)
	{
		for(int i = 0; i < gStats.anomCount; i++)
		{
			int fdur = (int)gStats.anom[i].anomTime;
			fMin = fdur / 60;
			fSec = fdur % 60;
			if(gStats.anom[i].anomType == 1)
				printf("[%d:%02d] Low satellite count WARNING\n", fMin, fSec);
			else if(gStats.anom[i].anomType == 2)
				printf("[%d:%02d] Low satellite count resolved\n", fMin, fSec);
			if(gStats.anom[i].anomType == 3)
				printf("[%d:%02d] HDOP degraded WARNING\n", fMin, fSec);
			else if(gStats.anom[i].anomType == 4)
				printf("[%d:%02d] HDOP warning resolved\n", fMin, fSec);
			if(gStats.anom[i].anomType == 5)
				printf("[%d:%02d] HDOP degraded CRITICAL\n", fMin, fSec);
			else if(gStats.anom[i].anomType == 6)
				printf("[%d:%02d] HDOP critical status resolved\n", fMin, fSec);
			if(gStats.anom[i].anomType == 7)
				printf("[%d:%02d] Satellite fix lost CRITICAL\n", fMin, fSec);
			else if(gStats.anom[i].anomType == 8)
				printf("[%d:%02d] Satellite fix regained\n", fMin, fSec);
		}
	}
	printf("================================================\n\n");
}
