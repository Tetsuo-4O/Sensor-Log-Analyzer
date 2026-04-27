Branch created to store contribution and document changes/revisions made to accomodate GPS module integration. 
All changes noted in relevant code sections; feel free to change, update, or recommend changes/fixes. 

This repo contains EVERYTHING necesarry to run and test our sensor log analyzer menu alongside trip report option 3. It compiles like this:
-> gcc MenuDraft2.c gpsmainparse.c gpsstats.c gpsexport.c -o sensorlog -lm 
Run the program like this:
-> ./sensorlog
Then designate main log and gps log.csv. ensure to use -lm argument in terminal window during compilation to link math functions.
Selecting option 3/TRIP report will generate a kml file in the common directory. Drag-and-drop that kml file into a Google Earth window or the Google Earth Pro desktop app to generate a VISUAL flight path on the map, complete with important metrics and markers. 
