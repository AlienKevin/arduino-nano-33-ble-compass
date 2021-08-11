# Compass
**Ouputs compass direction from 0 to 360 degrees, calibration procedure included for accurate measurement.**

This is a Platformio project that runs on Arduino Nano 33 BLE Sense.

# Usage
A compass direction is calculated from the magnetic readings X and Y of the LSM9DS1 chip. A higher value of `maxCount` increases accuracy but slows the measurement cycle. The compass must be calibrated for the magnetic disturbance of the environment. The onboard led connected to D13 is used as indicator during calibration.

At the start, you will be asked whether to calibrate or not, if you haven't set `offsetX` and `offsetY` produced by last calibration. If you reply yes to calibration request, the orange onboard LED will flash fast during calibration, which lasts 15 seconds. During calibration the board must at least be turned over a full 360 degrees. 
Calibration automatically finishes when the LED stops flashing. Then, newly obtained `offsetX` and `offsetY`
are stored and the compass is restarted with the new offsets. The printed offset values can be used in the program as initial values of `offsetX` and `offsetY`.

You can modify `maxCount`, `offsetX`, and `offsetY` within the USER CONFIGURATIONS section in `src/main.cpp`.

# Verification
You can verify whether your calibration is successful by comparing your Arduino's compass direction with the one outputed by your phone's compass app. My test shows that the difference can be minimized to within 3 degrees.

# Credits
Based on [Femme Verbeek's answer](https://forum.arduino.cc/t/digital-compass-with-nano-ble-33-sense/640308/25) on the Arduino forum. Adopted to Platformio and remove the need for a button to calibrate IMU.