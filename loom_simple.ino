// Compile for board "Adafruit Feather M0", NOT loomified

#include "src/Loom_Manager.h"
#include "src/Sensors/Loom_Random/Loom_Random.h"

Manager manager;
Loom_Random random_sensor(manager);

void setup() {
    manager.beginSerial();
    manager.initialize();
}

void loop() {
    manager.measure();
    manager.display_data();
    manager.pause(5000);
}