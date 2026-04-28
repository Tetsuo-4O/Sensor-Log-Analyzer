# **Sensor Log Analyzer**

This Sensor Log Analyzer allows the user to designate flight instrument and gps logs from UAS (unmanned aerial systems/drones) for parsing and flight data reporting. 

  


## Description and Features

By using flight logs and GPS logs this program is able to display a flight overview, an event log for the flight, and hardware/systems status during the flight. The program also features a custom report option, permitting the user to designate report content including metrics like axis roll/pitch rates, battery voltage, current draw, battery voltage sag, barometer altitude, accelerometer vibration, attitude, hardware health, and IMU/instrument temperatures. This program is useful to UAS pilots and mission planners for the purposes of data collection, efficiency/aircraft tuning, and comprehensive flight review. 

## File Format

This program was designed to handle .csv files only.

.csv stands for Comma-Seperated Values, a plain text format to store spreadsheets.

Files taken from UAS/Drones must be converted to .csv before being used in the program.

Example files:

Main log - LOG13.01.csv

GPS log  - LOG13.01.gps.csv 

## Run Instructions

To deploy this project run:

```c
git clone https://github.com/Tetsuo-4O/Sensor-Log-Analyzer.git
```
This will clone the directory to your terminal. (This requires the git package installed for cygwin.)

Change to Sensor-Log-Analyzer directory:

```c
cd Sensor-Log-Analyzer
```

To compile the program use the following:

```c
gcc MenuDraft4.c gpsstats.c gpsexports.c gpsmainparse.c -o Sensorlog -lm
```

Lastly in order to run the program use:

```c
./Sensorlog
```

## Usage

(Explain how to use in steps) 

To run this code correctly...
```c
int main(){
    printf("example code");
}
```

show examples 
```c
Enter flight log path:
/home/user/file/example_log01.01.csv
Enter GPS log path:
/home/user/file/example_log01.01.gps.csv
```

## Point of Contact
For questions about the code email one of the students below:

- Aedan S. - aedansnyder@unm.edu
- Andreas C. - achacon21@unm.edu
- Fuad A. - fawad@unm.edu
- Gabriella C. - gchavez007@unm.edu
- Naysan H. - nsanhtut@unm.edu
