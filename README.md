# keepDatCool

Build a simple Dashboard/Alertcenter to monitor fridges and other cooling devices to prevent fallouts.

Used hardware:
	- Raspberry PI 2B - PI 3 would be better, but my 2B was bored
	- SDCard for the PI
	- optional USB WifiStick for the Pi
	- otherwise it will need an ethernet connection using a lan-cable
	- 2x Wemos D1 
	- 2x DHT22 - TemperatureSensor
	- 3x microUSB PowerAdapter or 1x microUSB PowerAdapter + 2x 3,7V Battery 

## setting up the base

The Raspberry will be our mainstation/controlcenter/whatever and since it needs internet-access, plug in the lan-cable or a working usb-wifistick.
Now we need to put an OS on the SDCard for the PI to startup.
There are different ways to do this depening on your OS. 

- insert link - 

connect to your pi using ssh and type in the following commands:

git clone https://github.com/e1senhans/keepDatCool.git
cd keepDatCool
sudo install.sh

## setting up the sensors

Now we want to setup the two temperatureSensors. For this we will use a WemosD1 which is in short a "minicomputer" with wifi. It can be connected to any kind of hardwaresensors very easy and is a common used base for several cool projects.
In my case i am using a prebuild "Shield" for the Wemos D1 which includes a DHT22 - the temperaturesensor. Using such a shield theres no need to solder anything. 
Otherwise you will have to connect the DHT22-Sensor to the Wemos D1 as seen here:

- insert image - 

Since the WemosD1 is Arduino-compatible, we will use the ArduinoIDE to upload the SensorSoftware onto the WemosD1.

0. clone this git-repository using "git clone https://github.com/e1senhans/keepDatCool.git"
1. connect the WemosD1 to your PC using... a cable
2. Download ArduinoIDE
3. Install the WemosD1 board-management - insert link
4. Open the Script "TempSensor.a?" of the cloned repo with your ArduinoIDE 
5. Edit the Script and insert your wifi-credentials / rename the sensor
6. Press the "Upload Script"-Button to compile and upload it to your WemosD1
7. if you encounter any errors, trust your skills, use google and solve the problem
8. connect pin RST and D0 of the WemosD1 using a small cable - you need to do this to make the DeepSleep work - insert link
9. power up the WemosD1 using the microUSB-PowerAdapter or a simple Battery - insert link
10. open up the browser -> http://keepdatcool:8080/ and after a while you should see the sensors coming up

 
