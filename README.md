# **Sensor Log Analyzer**

This Sensor Log Analyzer allows the user to designate flight instrument and gps logs from UAS (unmanned aerial systems/drones) for parsing and flight data reporting. 

  


## Description and Features

By using flight logs and GPS logs this program is able to display a flight overview, an event log for the flight, and hardware/systems status during the flight. The program also features a custom report option, permitting the user to designate report content including metrics like axis roll/pitch rates, battery voltage, current draw, battery voltage sag, barometer altitude, accelerometer vibration, attitude, hardware health, and IMU/instrument temperatures. Lastly, Using the data collected the program then creates a kml file that will show a map of the flight on google maps. This program is useful to UAS pilots and mission planners for the purposes of data collection, efficiency/aircraft tuning, and comprehensive flight review. 

## File Format

This program was designed to handle .csv files only.

.csv stands for Comma-Seperated Values, a plain text format to store spreadsheets.

Files taken from UAS/Drones must be converted to .csv before being used in the program.

To convert UAS data to .csv a public repository designed for this can be used, such as:

https://github.com/cleanflight/blackbox-tools.git


Example files generated:

Main log - LOG123.01.csv

GPS log  - LOG123.01.gps.csv 

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

In order to run the program correctly you will need two .csv files, the main log and the GPS log.

After running the program you will be prompted to enter the main flight log path.

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
```

In order to enter the flight log path you can drag and drop the file onto the terminal when prompted.

  Example 1 after dragging LOG7.01.csv onto cygwin terminal:

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
/cygdrive/c/Users/Student/Desktop/School/LOG7.01.csv
```

Alternatively you can place the flight logs into the directory folder and access them using their name.

  Example 2 of having LOG7.01.csv in Sensor-Log-Analyzer directory folder:

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
LOG7.01.csv
```

After entering the path of the flight log, you will be prompted to enter the GPS log.

Enter the GPS log path using either method above.

  Example 1:

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
/cygdrive/c/Users/Student/Desktop/School/LOG7.01.csv
--- Enter GPS log path:
/cygdrive/c/Users/Student/Desktop/School/LOG7.01.gpscsv
```

  Example 2:

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
LOG7.01.csv
--- Enter GPS log path:
LOG7.01.gps.csv
```

Once the files are parsed you may then choose an option 1-5 from the menu:

```c
======= Flight Analyzer Ready =======
--- Enter flight log path:
/cygdrive/c/Users/Student/Desktop/School/LOG7.01.csv
--- Enter GPS log path:
/cygdrive/c/Users/Student/Desktop/School/LOG7.01.gpscsv
----------- Parsing Files.... -----------
--Records parsed: 10076
 Please choose one of the options:
1. Hardware Stat
2. Event Log
3. Trip Log
4. Custom Data
5. Exit
```

When choosing option 1: Hardware Stat

...

When choosing option 2: Event Log

...

When choosing option 3: Trip Log

...

When choosing option 4: Custom Data

...

When choosing option 5: Exit

Program will terminate.


## Point of Contact
For questions about the code email one of the students below:

- Aedan S. - aedansnyder@unm.edu
- Andreas C. - achacon21@unm.edu
- Fuad A. - fawad@unm.edu
- Gabriella C. - gchavez007@unm.edu
- Naysan H. - nsanhtut@unm.edu
