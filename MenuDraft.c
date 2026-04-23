#include <stdio.h>
#include <string.h>

void menuDisplay();
void customMenuDisplay();

void eventLog();
void report();
void hardwareStat();
int main()
{




char line[4096];
FILE *fp;
char csvFile[256];

double axisRoll = 0.0;
        double *axisRoll_p = &axisRoll;
double axisPitch = 0.0;
        double *axisPitch_p = &axisPitch;
double vBatt = 0.0;
        double *vBatt_p = &vBatt;
double iAmp = 0.0;
        double *iAmp_p = &iAmp;
double sagVbatt = 0.0;
        double *sagVbatt_p = &sagVbatt;
double baroAlt = 0.0;
        double *baroAlt_p = &baroAlt;
double accVib = 0.0;
        double *accVib_p = &accVib;
double attitude = 0.0;
        double *attitude_p = &attitude;
double helthStat = 0.0;
        double *helthStat_p = &helthStat;
double imuTemp = 0.0;
        double *imuTemp_p = &imuTemp;

        int choice = 0;
        int choiceDat =0;

                printf("please enter file address\n");
                fgets(csvFile, sizeof(csvFile), stdin);
                csvFile[strcspn(csvFile, "\r\n")] = '\0';
                fp = fopen(csvFile,"r");
        if (!fp)
                {
                printf("file could not be read\n");
                return 1;
                }
        else
        {
                printf(" File being read: %s\n", csvFile);
       /* while (fgets(line, sizeof(line), fp))
        {
                char *token = strtok(line, ",");
                while (token != NULL)
                {
                        token = strtok(NULL, ",");
                        printf("%s\n",token);
                }
        }*/
        }

                do{
                menuDisplay();

                        scanf("%d", &choice);

                switch(choice)
                {
                case 1:

                        printf("choice one");
                break;
                case 2:
                        printf("choice 2");

                break;

                case 3:
                        printf("choice 3");

                break;

                case 4:
                        do{
                        customMenuDisplay();

                //choosing the data to see
                        scanf("%d", &choiceDat);
                /*switch cases for each typer of data, easier to do this than an avg func for all since not all cases are Avg's*/
                        switch(choiceDat)
                        {
                        case 1:
                        printf("data type: Axis Roll\n");
                        break;
                        case 2:
                        printf("data type: Axis Pitch\n");
                        break;
                        case 3:
                        printf("data type: Battery Voltage\n");
                        break;
                        case 4:
                        printf("data type: Current Amperage\n");
                        break;
                        case 5:
                        printf("data type: Battery Voltage Sag\n");
                        break;
                        case 6:
                        printf("data type: Barometric Altitute\n");
                        break;
                        case 7:
                        printf("data type:Accelerometer Vibration\n");
                        break;
                        case 8:
                        printf("data type: Attitude\n");
                        break;
                        case 9:
                        printf("data type: Health Status\n");
                        break;
                        case 10:
                        printf("data type: IMU Temp\n");
                        break;
                        case 11:
                        printf("data type: Exit \n");
                        break;
                        }}
                        while (choiceDat !=11);
                break;

                case 5:
                return 0;
                break;
        }}
               while (choice!=5);
                fclose(fp);




return 0;}
        void menuDisplay()
                {
                printf(" Fight Log analyzer ready\n");
                printf(" please choose one of the options:\n");
                printf("1. HardwareStat\n");
                printf("2. EventLog\n");
                printf("3. Trip Log\n");
                printf("4. Custom Data\n");
                printf("5. Exit\n");

                }

        void customMenuDisplay()
                {

                printf("Please choose which data you would like to see:\n");
                printf("1.Axis Roll     2.Axis Pitch    3.Battery Voltage       4.Current amperage\n");
                printf("5.Battery Volt Sag      6.Baro Altitude         7.Accelerometer Vibration       8.Attitude\n");
                printf("9.Health Status         10.IMU Temperature      11.Exit Custom Menu\n");
                }



        void hardwareStat()
        {
        printf("hardware stat chosen");
        }
        void eventLog()
        {
        printf("event log chosen");
        }
        void report()
        {
        printf("report");
        }
