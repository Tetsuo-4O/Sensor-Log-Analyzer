# **Sensor Log Analyzer**

This Sensor Log Analyzer allows the user to designate flight instrument and gps logs from UAS (unmanned aerial systems/drones) for parsing and flight data reporting. 

  


## Description and Features

By using flight logs and GPS logs this program is able to display a flight overview, an event log for the flight, and hardware/systems status during the flight. The program also features a custom report option, permitting the user to designate report content including metrics like axis roll/pitch rates, battery voltage, current draw, battery voltage sag, barometer altitude, accelerometer vibration, attitude, hardware health, and IMU/instrument temperatures. Lastly, Using the data collected the program then creates a kml file that will show a map of the flight on google earth. This program is useful to UAS pilots and mission planners for the purposes of data collection, efficiency/aircraft tuning, and comprehensive flight review. 

## File Format

This program was designed to handle .csv files as inputs.

.csv stands for Comma-Seperated Values and is a plain text format to store spreadsheets.

Files taken from UAS/Drones must be converted to .csv before being used in the program.

To convert UAS data to .csv a public repository designed for this can be used, such as:

https://github.com/cleanflight/blackbox-tools.git

Example files generated:

Main log - LOG123.01.csv

GPS log  - LOG123.01.gps.csv


This program was also designed to handle .kml files as an output.

.kml stands for keyhole markup language and is an XML-based format that stores geographic/positional data.

The program outputs a .kml file to the directory file path when prompted.

## Run Instructions

To deploy this project run:

```c
git clone https://github.com/Tetsuo-4O/Sensor-Log-Analyzer.git
```
This will clone the directory to your terminal. (This requires the "git" package installed for cygwin.)

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

  - Displays Hardware Status containing Battery Voltage, Current Draw, Voltage Sag, IMU Temp and Acceleration Vibration of UAS.
```c
Please choose one of the options:
1. Hardware Stat
2. Event Log
3. Trip Log
4. Custom Data
5. Exit
1

--- Hardware Status ---
Battery Voltage  : 19.76 V
Current Draw     : 0.00 A
Voltage Sag      : 20.66 V
IMU Temperature  : 24.5 C
Accel Vibration  : 1518.00
-----------------------
```


When choosing option 2: Event Log

  - Displays Event Log.
```c
...
```

When choosing option 3: Trip Log

  - Displays Flight Overview containing events, flight duration, distance, altitude, rate of climb/descent, speed, and aquired satellites.
  - Generates a kml file at directory file path containing geographic data. This file can be used on google earth to view flight path.

```c
Please choose one of the options:
1. Hardware Stat
2. Event Log
3. Trip Log
4. Custom Data
5. Exit
3
================================================
             IN-FLIGHT TRIP REPORT
================================================
Log-file: /cygdrive/c/Users/Student/Desktop/School/LOG7.01.gps.csv

--------------Flight Overview-----------------
Events recorded: 32 warnings(0 critical)
Total flight duration: 16m 06s
Total distance flown: 4674.17 m
Greatest distance from homepoint: 52.12 m
Maximum recorded altitude: 53 m
Maximum rate of climb: 592 cm/s
Maximum rate of descent: -871 cm/s
Maximum rate of speed: 14.04 m/s
Average flight speed: 4.89 m/s
Maximum acquired satellites: 11
Minimum acquired satellites: 7
Average satellite count: 10
Average HDOP(satellite signal quality): 149.03

---------Event Log---------
[0:22] HDOP degraded WARNING
[0:22] HDOP warning resolved
kmltitle = '/cygdrive/c/Users/Student/Desktop/School/LOG7.01.gps.kml'
fp = 0xa00020718
Flight path KML generated at: /cygdrive/c/Users/Student/Desktop/School/LOG7.01.gps.kml
Drag and drop file into Google Earth for real-world flight demonstration.
```

When choosing option 4: Custom Data

  - Custom Data Menu will prompt user with ten selections to pull individual data from.
  - User may select the catagories using 1-10 and after making selections use -1 to finish.
  
```c
 please choose one of the options:
1. HardwareStat
2. EventLog
3. Trip Log
4. Custom Data
5. Exit
4

==== Custom Data Menu ====
 Enter choices (-1 to finish):

1. Axis Roll        2. Axis Pitch        3. Battery Voltage       4. Current
5. Voltage Sag      6. Baro Altitude     7. Accel Vibration       8. Attitude
9. Health Status    10. IMU Temperature
Choice: 1

1. Axis Roll        2. Axis Pitch        3. Battery Voltage       4. Current
5. Voltage Sag      6. Baro Altitude     7. Accel Vibration       8. Attitude
9. Health Status    10. IMU Temperature
Choice: 2

1. Axis Roll        2. Axis Pitch        3. Battery Voltage       4. Current
5. Voltage Sag      6. Baro Altitude     7. Accel Vibration       8. Attitude
9. Health Status    10. IMU Temperature
Choice: 3

1. Axis Roll        2. Axis Pitch        3. Battery Voltage       4. Current
5. Voltage Sag      6. Baro Altitude     7. Accel Vibration       8. Attitude
9. Health Status    10. IMU Temperature
Choice: -1

--- Selected Data ---

-- Axis Roll = 0.00

-- Axis Pitch = 5.00

-- Battery Voltage = 19.76
----------------------
```



When choosing option 5: Exit

  - Program will terminate.
```c
    please choose one of the options:
1. HardwareStat
2. EventLog
3. Trip Log
4. Custom Data
5. Exit
5

User@Student-PC ~/Sensor-Log-Analyzer
~
```
## Point of Contact
For questions about the code email one of the students below:

- Aedan S. - aedansnyder@unm.edu
- Andreas C. - achacon21@unm.edu
- Fuad A. - fawad@unm.edu
- Gabriella C. - gchavez007@unm.edu
- Naysan H. - nsanhtut@unm.edu
