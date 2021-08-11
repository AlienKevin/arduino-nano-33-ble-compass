/* Compass example for the Nano 33 BLE Sense
 * A compass direction is calculated from the magnetic readings X and Y of the LSM9DS1 chip.
 * A higher value of maxcount increases accuracy but slows the measurement cycle. 
 * The compass must be calibrated for the magnetic disturbance of the environment.
 * A push button should be connected between D3 (buttonpin) and GND.
 * The onboard led connected to D13 is used as indicator. 
 * Keep the button pressed till the led goes ON 
 * Releasing it starts the calibration indicated by the fast flashing led.
 * During calibration the board must at least be turned over a full 360 deg. 
 * To finish push the button again. This stores the values and restarts the compass with the new value. 
 * The printed offsetvalues can be used in the program as initial values of offsetX and offsetY 
 * see https://forum.arduino.cc/t/digital-compass-with-nano-ble-33-sense/640308/25
 * Written by Femme Verbeek 2020 for educational purposes 
*/

#include <Arduino_LSM9DS1.h>
#define ledpin 13

void calibrate();
String read_line();

// Modify these offset numbers at will with the outcome of the calibration values
float offsetX = 0;
float offsetY = 0;
const int maxcount = 40; //higher number = more accurate but slower
bool already_asked_to_calibrate = false;

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
    while (offsetX == 0 && offsetY == 0 && !already_asked_to_calibrate)
    {
        Serial.print("Do you want to calibrate the IMU? (y/n): ");
        String response = read_line();
        if (response == "y" || response == "yes")
        {
            calibrate();
            already_asked_to_calibrate = true;
        }
        else if (response == "n" || response == "no")
        {
            already_asked_to_calibrate = true;
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

String read_line()
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
