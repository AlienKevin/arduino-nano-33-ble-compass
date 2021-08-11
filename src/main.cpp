/* Compass example for the Nano 33 BLE Sense
 * A compass direction is calculated from the magnetic readings X and Y of the LSM9DS1 chip.
 * A higher value of maxcount increases accuracy but slows the measurement cycle. 
 * The compass must be calibrated for the magnetic disturbance of the environment.
 * The onboard led connected to D13 is used as indicator during calibration. 
 * At the start, you will be asked whether to calibrate or not, if you haven't set offsetX and offsetY
 * produced by last calibration. If you reply yes to calibration request, the orange onboard LED will
 * flash fast during calibration, which lasts 15 seconds.
 * During calibration the board must at least be turned over a full 360 deg. 
 * Calibration automatically finishes when the LED stops flashing. Then, newly obtained offsetX and offsetY
 * are stored and the compass is restarted with the new offsets.
 * The printed offset values can be used in the program as initial values of offsetX and offsetY 
 * see https://forum.arduino.cc/t/digital-compass-with-nano-ble-33-sense/640308/25
 * Written by Femme Verbeek 2020 for educational purposes
 * Adopted to Platformio and removed the need for a button to calibrate IMU by Kevin Li in 2021.
*/

#include <Arduino_LSM9DS1.h>
#define ledpin 13

void calibrate();
String readLine();

/***************************************** START OF USER CONFIGURATIONS *****************************************/
// Modify these offset numbers at will with the outcome of the calibration values
float offsetX = 0;
float offsetY = 0;
const int maxcount = 40; //higher number = more accurate but slower
/***************************************** END OF USER CONFIGURATIONS *****************************************/

bool hasAskedToCalibrate = false;

void setup()
{
    pinMode(ledpin, OUTPUT);
    digitalWrite(ledpin, LOW);
    Serial.begin(115200);
    while (!Serial)
        ; // wait till the serial monitor connects
    Serial.println("Starting Compass");
    if (IMU.begin())
        Serial.println("IMU available and working");
    else
        Serial.println("Failed to initialize IMU!");
}

void loop()
{
    while (offsetX == 0 && offsetY == 0 && !hasAskedToCalibrate)
    {
        Serial.print("Do you want to calibrate the IMU? (y/n): ");
        String response = readLine();
        if (response == "y" || response == "yes")
        {
            calibrate();
            hasAskedToCalibrate = true;
        }
        else if (response == "n" || response == "no")
        {
            hasAskedToCalibrate = true;
        }
    }
    float x, y, z;
    float averX = 0, averY = 0;
    Serial.print("Measuring ");
    for (int i = 1; i <= maxcount; i++)
    {
        while (!IMU.magneticFieldAvailable())
            ;
        IMU.readMagneticField(x, y, z);
        averX += x / maxcount;
        averY += y / maxcount;
        digitalWrite(ledpin, LOW);
    }
    float heading = atan2(averY - offsetY, averX - offsetX) * 180 / PI + 180;
    digitalWrite(ledpin, HIGH);
    Serial.print(" Compass direction ");
    Serial.println(heading);
}

void calibrate()
{
    float x, y, z, Xmin, Xmax, Ymin, Ymax;
    boolean ledIsOn = true;
    digitalWrite(ledpin, ledIsOn);
    Serial.println("Recalibrating");
    while (!IMU.magneticFieldAvailable())
        ;
    IMU.readMagneticField(Xmin, Ymin, z); //find initial values
    Xmax = Xmin;
    Ymax = Ymin;

    unsigned long start_time = millis();
    unsigned long end_time = start_time;
    while ((end_time - start_time) <= 15000) // calibrate for 15s
    {
        while (!IMU.magneticFieldAvailable())
            ; // wait till new magnetic reading is available
        IMU.readMagneticField(x, y, z);
        Xmax = max(Xmax, x);
        Xmin = min(Xmin, x);
        Ymax = max(Ymax, y);
        Ymin = min(Ymin, y);
        digitalWrite(ledpin, ledIsOn = !ledIsOn); //reverse on/off led
        end_time = millis();
    }
    offsetX = (Xmax + Xmin) / 2;
    offsetY = (Ymax + Ymin) / 2;
    Serial.print("New offset X= ");
    Serial.print(offsetX);
    Serial.print(" Y= ");
    Serial.println(offsetY);
}

String readLine()
{
    String line = "";
    while (1)
    {
        while (Serial.available() > 0)
        {
            char c = Serial.read();
            if (c != '\r')
            {
                Serial.print(c);
                line += c;
            }
            else
            {
                Serial.read(); // read the next \n following \r
                Serial.println();
                return line;
            }
        }
    }
    return line;
}
