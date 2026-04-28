#include <stdio.h>
#include <string.h>
#include <stdlib.h>   
#include <math.h>
#include <stddef.h>
#include "gps.h" //CHANGE 1(Aedan)
#include "gpsexport.h" //CHANGE 9(Aedan)

//struct that defines the 
typedef struct{
FILE *file_p;
char path[256];
		} LogFile; // struct name 

// Hardware status struct, add more members if you want
typedef struct{
    double vBatt;
    double iAmp;
    double sagVbatt;
    double imuTemp;
    double accVib;
		} HardwareStat; //struct name 
//Trip report struct, add more members if needed
typedef struct{
    double axisRoll;
    double axisPitch;
    double baroAlt;
    double attitude;
    double healthStat;
		} TripReport; //struct name
// GPS struct also add data if needed
typedef struct {
    double latitude;
    double longitude;
    double gpsAlt;
		} GpsData; // struct name
//event log 
typedef struct {
    int    time;
    char   type[32];
    char   data[64];
} Event;

typedef struct {
    Event *events;     // dynamically allocated array
    size_t count;      // how many events recorded
    size_t capacity;   // how much room allocated
} EventLog;
//----------------------------------------------------------------------------------------------------------->
//CHANGE 11(Aedan) - NEW STRUCTS
//individual flight/sensor elements
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
//flight data array, pointer type FlightelementRecord
typedef struct {
	FlightelementRecord *fpointerRecord;
	size_t count;
	size_t capacity;
	char source_file[256];
} FlightmainRecord;
//stores anomaly/event types and timestamp for each
typedef struct {
	int fanomType;
	double fanomTime;
} Flightanom;
//stores converted metrics; referenced later in report
typedef struct { //struct for storing converted metrics
	double flightDur;
	double vtakeOff; //volt at takeoff
	double vLanding; //volt at end of log
	double vDelta; //diff used to calculate batt used
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
	double maxbarAlt; //maybe redundant because GPS already provides mostly accurate alt
	double battUsage;
	int hwHealth; //really not sure about this one, follow TBA
	
	char source_file[256]; //reads each line from main log

	Flightanom fanom[1024]; //for now, might need larger buffer
	int fanomCount; //flightanomaly count
	int fwarnCount; //designate WARNING/ADVISE
	int fcritCount; //designate CRITICAL WARN
} Flightstats;
//extract function; stores flight data in appropriate structs
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

	char line[4096]; //covers long row cases
	char *token;
	int index = 0;
	int col_tStamp = -1, col_vBatt = -1, col_curr = -1, col_sag = -1, col_temp = -1, col_accX = -1, col_accY = -1, col_accZ = -1, col_vib = -1, col_roll = -1, col_pitch = -1, col_bAlt = -1, col_hwh = -1;
	fgets(line, sizeof(line), fp);
	line[strcspn(line, "\r\n")] = '\0';
	token = strtok(line, ",");
	//process .csv header
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
	//process .csv data content
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
	
	fclose(fp); //close file
	return fpointerLog;

}
//process function; converts and manipulates flight data
Flightstats process_flightstats(FlightmainRecord *fpointerLog)
{
	Flightstats stats;
	//accumulators
	double currSum = 0;
	double tempSum = 0;
	double vibSum = 0;
	double rollSum = 0;
	double pitchSum = 0;
	//stat fields
	stats.maxbarAlt = 0;
	stats.maxForce = 0;
	stats.minvoltBatt = 9999;
	stats.maxCurrent = 0;
	stats.maxvoltSag = 0;
	stats.maximuTemp = 0;
	stats.maxRoll = 0;
	stats.maxPitch = 0;
	stats.maxVib = 0;
	////fanom desig
	stats.fanomCount = 0;
	stats.fwarnCount = 0;
	stats.fcritCount = 0;
	//anom bool flags
	int conlowVolt = 0;
	int conhiVib = 0;
	int conhiTemp = 0;
	int conhiSag = 0;
	int concritSag = 0;
	int concritVolt = 0;

	//main process loop
	for (size_t i = 0; i < fpointerLog->count; i++)
	{
		double timeElapsed = (fpointerLog->fpointerRecord[i].timeStamp - fpointerLog->fpointerRecord[0].timeStamp) / 1000000;;
		double sagDelta = fpointerLog->fpointerRecord[i].sagComp - fpointerLog->fpointerRecord[i].voltBatt;

		double forceGen = sqrt(fpointerLog->fpointerRecord[i].acsmX * fpointerLog->fpointerRecord[i].acsmX + fpointerLog->fpointerRecord[i].acsmY * fpointerLog->fpointerRecord[i].acsmY + fpointerLog->fpointerRecord[i].acsmZ * fpointerLog->fpointerRecord[i].acsmZ) / 1024; //INav uses 1024 to represent 1G. 

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
		//absolute value; we just want maximum rate
		double absvalR = fpointerLog->fpointerRecord[i].roll < 0 ?
			-fpointerLog->fpointerRecord[i].roll : fpointerLog->fpointerRecord[i].roll;
		double absvalP = fpointerLog->fpointerRecord[i].pitch < 0?
			-fpointerLog->fpointerRecord[i].pitch: fpointerLog->fpointerRecord[i].pitch;

		if(absvalR > stats.maxRoll)
			stats.maxRoll = absvalR;
		if(absvalP > stats.maxPitch)
			stats.maxPitch = absvalP;
		//accumulator math
		currSum += fpointerLog->fpointerRecord[i].current;
		tempSum += fpointerLog->fpointerRecord[i].imuTemp;
		vibSum += fpointerLog->fpointerRecord[i].accVib;
		rollSum += absvalR;
		pitchSum += absvalP;

		//anomaly/event logic
		//1 = low volt WARNING
		//2 = low volt resolved
		//3 = low volt CRITICAL
		//4 = low volt critical condition resolved
		//5 = high vibration condition
		//6 = high vibration resolved
		//7 = high temp warning
		//8 = high temp resolved
		//9 = voltage sag warning
		//10 = voltage sag resolved
		//11 = voltage sag critical
		//12 = voltage sag critical resolved
		////low batt/battery damage risk
		if(fpointerLog->fpointerRecord[i].voltBatt > 0 && fpointerLog->fpointerRecord[i].voltBatt < 19.8 && fpointerLog->fpointerRecord[i].voltBatt >= 18.0 && conlowVolt == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 1;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conlowVolt = 1;
		}
		////drop from warning to critical low volt
		else if(fpointerLog->fpointerRecord[i].voltBatt < 18.0 && conlowVolt == 1)
		{
			conlowVolt = 0;
		}
		////clear volt warning, signal recovery
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 19.8 && conlowVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 2;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conlowVolt = 0;
		}
		////critical onset
		if(fpointerLog->fpointerRecord[i].voltBatt > 0 && fpointerLog->fpointerRecord[i].voltBatt < 18.0 && concritVolt == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 3;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			stats.fcritCount++;
			concritVolt = 1;
		}
		////critical transition to warning
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 18.0 && fpointerLog->fpointerRecord[i].voltBatt < 19.8 && concritVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 4;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritVolt = 0;

			stats.fanom[stats.fanomCount].fanomType = 1;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conlowVolt = 1;
		}
		////critical to no warning
		else if(fpointerLog->fpointerRecord[i].voltBatt >= 19.8 && concritVolt == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 4;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritVolt = 0;
		}
		////high vibration warning
		if(fpointerLog->fpointerRecord[i].accVib >= 10000 && conhiVib == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 5;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conhiVib = 1;
		}
		////high vib resolved
		else if(fpointerLog->fpointerRecord[i].accVib < 10000 && conhiVib == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 6;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiVib = 0;
		}
		////high imu temp warning
		if(fpointerLog->fpointerRecord[i].imuTemp >= 52 && conhiTemp == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 7;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conhiTemp = 1;
		}
		////high temp resolved
		else if(fpointerLog->fpointerRecord[i].imuTemp < 52 && conhiTemp == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 8;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiTemp = 0;
		}
		////high sag warning
		if(sagDelta > 1 && sagDelta < 2 && conhiSag == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 9;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conhiSag = 1;
		}
		////sag warning to sag critical
		else if(sagDelta >= 2 && conhiSag == 1)
		{
			conhiSag = 0;
		}
		////sag warning resolved
		else if(sagDelta <= 1 && conhiSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 10;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			conhiSag = 0;
		}
		////critical sag onset
		if(sagDelta >= 2.0 && concritSag == 0)
		{
			stats.fanom[stats.fanomCount].fanomType = 11;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			stats.fcritCount++;
			concritSag = 1;
		}
		////critical sag to sag warning
		else if(sagDelta < 2 && sagDelta > 1 && concritSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 12;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritSag = 0;

			stats.fanom[stats.fanomCount].fanomType = 9;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			stats.fwarnCount++;
			conhiSag = 1;
		}
		////critical sag to no sag
		else if(sagDelta <= 1 && concritSag == 1)
		{
			stats.fanom[stats.fanomCount].fanomType = 12;
			stats.fanom[stats.fanomCount].fanomTime = timeElapsed;
			stats.fanomCount++;
			concritSag = 0;
		}
	}
		//flight duration calc
		stats.flightDur = (fpointerLog->fpointerRecord[fpointerLog->count - 1].timeStamp - fpointerLog->fpointerRecord[0].timeStamp) / 1000000;
		//average calcs
		stats.avgCurrent = currSum / fpointerLog->count;
		stats.avgimuTemp = tempSum / fpointerLog->count;
		stats.avgVib = vibSum / fpointerLog->count;
		//takeoff v landing voltage delta
		stats.vtakeOff = fpointerLog->fpointerRecord[0].voltBatt;
		stats.vLanding = fpointerLog->fpointerRecord[fpointerLog->count - 1].voltBatt;
		stats.vDelta = stats.vtakeOff - stats.vLanding;
		////using hardcoded 6 because battery is 6-cell
		stats.battUsage = (stats.vDelta / (stats.vtakeOff - 18.3)) * 100;
		//source
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
//---------------------------------------------------------------------------------------------------------->
// name and offset position, lets code locate offset members
typedef struct {
    const char *header;
    size_t      offset;
		} ColumnMap; // struct name

// assings each struct member a reference for when the parsing needs to look for something
const ColumnMap hardwareColumns[] = 
					{
    {"vbat (V)",         offsetof(HardwareStat, vBatt)},
    {"amperage",     offsetof(HardwareStat, iAmp)},
    {"sagCompensatedVBat",   offsetof(HardwareStat, sagVbatt)},
    {"IMUTemperature",      offsetof(HardwareStat, imuTemp)},
    {"accVib", offsetof(HardwareStat, accVib)},
					 };
const ColumnMap tripColumns[] = 
				{
    {"axisRate[0]",   offsetof(TripReport, axisRoll)},
    {"axisRate[1]",  offsetof(TripReport, axisPitch)},
    {"BaroAlt (cm)",offsetof(TripReport, baroAlt)},
    {"attitude[0]",    offsetof(TripReport, attitude)},
    {"hwHealthStatus", offsetof(TripReport, healthStat)},
				};

const ColumnMap gpsColumns[] = {
    {"GPS_coord[0]", offsetof(GpsData, latitude)},
    {"GPS_coord[1]", offsetof(GpsData, longitude)},
    {"GPS_altitude", offsetof(GpsData, gpsAlt)},
				};


// naming variables so we can insert that into our many functions
#define HW_COL_COUNT (sizeof(hardwareColumns)/sizeof(hardwareColumns[0]))
#define TR_COL_COUNT  (sizeof(tripColumns)/sizeof(tripColumns[0]))
#define GPS_COL_COUNT (sizeof(gpsColumns)/sizeof(gpsColumns[0]))
//prototypes
void menuDisplay();
void customMenuDisplay(HardwareStat *hw, TripReport *tr);
void handleCustomMenu(HardwareStat *hw, TripReport *tr);
void parseCsv(LogFile *log, const ColumnMap *maps[], void *targets[],size_t mapCounts[], size_t numStructs);
void parseFlightLog(LogFile *log, HardwareStat *hw, TripReport *tr);
//void parseGpsLog(LogFile *log, GpsData *gps); //CHANGE 12(Aedan) -stub function temporarily removed
int openLog(LogFile *log, const char *prompt);

//void hardwareStat(HardwareStat *hw);
void event_log(Flightstats fStats, GPSstats gStats); //CHANGE (Aedan)
//void report(TripReport *tr);

//repeats
//void report(); //CHANGE 7(Aedan)
void hardwareStat(HardwareStat *hw);
void eventLog();
int main() //MAIN START
{
LogFile      flightLog = {0};
LogFile      gpsLog    = {0};
HardwareStat hw  = {0};
TripReport   tr  = {0};
GpsData      gps = {0};
EventLog     ev  = {0};   // need to desing still Aedans job I think

GPSmainRecord *gpsData = NULL; //CHANGE 2(Aedan)
GPSstats results; //CHANGE 3(Aedan)

FlightmainRecord *flightData = NULL; //CHANGE 13(Aedan)
Flightstats flightResults;

int choice = 0;
int choiceDat = 0;

                printf("Please enter file address\n");
             	if (!openLog(&flightLog, "Enter flight log path:\n")) return 1;
		if (!openLog(&gpsLog,    "Enter GPS log path:\n"))    return 1;

		parseFlightLog(&flightLog, &hw, &tr);
		
		flightData = extract_flight(flightLog.path); //CHANGE 14(Aedan)
		
		if(flightData == NULL)
		{
			printf("Failed to parse flight log.\n");
			return 1;
		} //END CHANGE 14
		
		gpsData = extract_gps(gpsLog.path); //CHANGE 4(Aedan)
		
		if(gpsData == NULL) //CHANGE 5(Aedan)
		{
			printf("Failed to parse GPS log.\n");
			return 1; 
		} //END CHANGE 5
                //computing before menu loop
		flightResults = process_flightstats(flightData);
		results = process_stats(gpsData);

		do{
                menuDisplay();

                        if(scanf("%d", &choice) != 1) //CHANGE 10(Aedan)
			{
				printf("Invalid choice; please choose an option from the menu:\n");
				while(getchar() != '\n'); //clear input buffer
				choice = 0;			  //reassign choice
			} //END CHANGE 10
                switch(choice)
                {
                case 1:

                        hardwareStat(&hw);
                break;
                case 2:
                        event_log(flightResults, results);
		break;

                case 3: //CHANGE 6(Aedan)
			trip_report(results);
			export_kml(gpsData, results);
		break; //END CHANGE 6

                case 4:
                        customMenuDisplay(&hw, &tr);
                break;

                case 5:
                return 0;
		break;
		
		case 6: //CHANGE 15(Aedan)
			inst_report(flightResults);
		break;

			
		
        }}
               while (choice!=5);
                fclose(flightLog.file_p);
		fclose(gpsLog.file_p);




return 0;}
        void menuDisplay()
                {
                printf("-Flight Log analyzer ready\n");
                printf("-Please choose one of the options:\n");
                printf("1. HardwareStat\n");
                printf("2. EventLog\n");
                printf("3. Trip Report\n");
                printf("4. Custom Data\n");
                printf("5. Exit\n");
		printf("6. Instrument report\n");

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
			printf("1. Axis Roll 2. Axis Pitch 3. Battery Voltage 4. Current\n");
			printf("5. Voltage Sag 6. Baro Altitude 7. Accel Vibration 8. Attitude\n");
			printf("9. Health Status 10. IMU Temperature\n");
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

		printf("--- Selected Data ---\n");

		for(int i = 0; i < count; i++)
		{
			switch(selected[i])
			{
				case 1:
					printf("\n-- Axis Roll = %.2f\n", tr->axisRoll);
					break;

				case 2:
					printf("\n-- Axis Pitch = %.2f\n", tr->axisPitch);
					break;

				case 3:
					printf("\n-- Battery Voltage = %.2f\n", hw->vBatt);
					break;
				case 4:
					printf("\n-- Current = %.2f A\n", hw->iAmp);
					break;
				case 5:
					printf("\n-- Voltage Sag = %.2f\n", hw->sagVbatt);
					break;
				case 6:
					printf("\n-- Baro Altitude = %.2f\n", tr->baroAlt);
					break;
				case 7:
					printf("\n-- Accel Vibration = %.2f\n", hw->accVib);
					break;
				case 8:
					printf("\n-- Attitude = %.2f\n", tr->attitude);
					break;
				case 9:
					printf("\n-- Health = %.2f\n", tr->healthStat);
					break;
				case 10:
					printf("\n-- IMU Temp = %.2f\n", hw->imuTemp);
					break;
			}
		}
		printf("--------------------\n");
	}



        void hardwareStat(HardwareStat *hw)
        {
        	printf("\n--- Hardware Stats --- \n");
		printf("Battery Voltage : %.2f V\n", hw->vBatt);
		printf("Current Draw    : %.2f A\n", hw->iAmp);
		printf("Voltage Sag     : %.2f V\n", hw->sagVbatt / 100.0);
		printf("IMU Temperature : %.1f C\n", hw->imuTemp / 10.0);
		printf("Accel Vibration : %.2f\n", hw->accVib);
		printf("-------------------------\n");
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
				fMin = fdur /60;
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

        void report()
        {
        printf("report");
        }

// parser function

 void parseCsv(LogFile *log, const ColumnMap *maps[], void *targets[],size_t mapCounts[], size_t numStructs)
{
    char line[4096];

// checks if file can be read, creates an array of names w ',' will help to tokenize later
    if (!fgets(line, sizeof(line), log->file_p)) return;
    line[strcspn(line, "\r\n")] = '\0';

// creates a cache to refer each member of struct and each struct
    int *colIndex[16];
    for (size_t s = 0; s < numStructs; s++) {
        colIndex[s] = malloc(mapCounts[s] * sizeof(int));
        for (size_t i = 0; i < mapCounts[s]; i++) colIndex[s][i] = -1;
    }

//this scans the array of names and tokenizes at each ','
    int idx = 0;
    char *tok = strtok(line, ",");
    while (tok) {
//goes thru structs
        for (size_t s = 0; s < numStructs; s++) {
//goes thru struct members 
            for (size_t i = 0; i < mapCounts[s]; i++) {
//compares diffs of struct member and token, if none, remebers location of member index 
                if (strcmp(tok, maps[s][i].header) == 0) {
                    colIndex[s][i] = idx;
                }
            }
        }
//will do it for the next token, and so on
        idx++;
        tok = strtok(NULL, ",");
    }

    while (fgets(line, sizeof(line), log->file_p)) {

//cleans up tokens 
	line[strcspn(line, "\r\n")] = '\0';
//creates token array to hold them
	char *tokens[512];
        int n = 0;
        char *lineToken_p = strtok(line, ",");
        while (lineToken_p && n < 512) { 
		tokens[n++] = lineToken_p; 
		lineToken_p = strtok(NULL, ","); 
					}

        for (size_t s = 0; s < numStructs; s++) {
            for (size_t i = 0; i < mapCounts[s]; i++) {
                int c = colIndex[s][i];
                if (c >= 0 && c < n) {
                    double val = atof(tokens[c]);
                    *(double *)((char *)targets[s] + maps[s][i].offset) = val;
                }
            }
        }
    }

    for (size_t s = 0; s < numStructs; s++) free(colIndex[s]);
}

//flight log parser, acts from OG parser
void parseFlightLog(LogFile *log, HardwareStat *hw, TripReport *tr) {
    const ColumnMap *maps[]    = { hardwareColumns, tripColumns };
    void            *targets[] = { hw, tr };
    size_t           counts[]  = { HW_COL_COUNT, TR_COL_COUNT };
    parseCsv(log, maps, targets, counts, 2);
}

//GPS log parser, acts from OG parser
void parseGpsLog(LogFile *log, GpsData *gps) {
    const ColumnMap *maps[]    = { gpsColumns };
    void            *targets[] = { gps };
    size_t           counts[]  = { GPS_COL_COUNT };
    parseCsv(log, maps, targets, counts, 1);
}

//open log funtion
int openLog(LogFile *log, const char *prompt) {
	do {
		printf("%s", prompt);
		if (!fgets(log->path, sizeof(log->path), stdin))
			return 0;
		log->path[strcspn(log->path, "\r\n")] = '\0';
		log->file_p = fopen(log->path, "r");
		if (!log->file_p)
			printf("---------- could not open %s ----------\n", log->path);
	}
	while (log->file_p == NULL);
	return 1;
}


