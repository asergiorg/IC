#include "disturber.h"
#include "config.h"

THD_FUNCTION(disturber, arg) {
  (void)arg;

  // Seed for randomness
  randomSeed(analogRead(0));

  while (!chThdShouldTerminateX()) {
    // Sleep for a random duration (between 5 and 15 seconds) without causing any disturbance
    int sleepTime = random(5000, 15001);
    chThdSleepMilliseconds(sleepTime);

    // Wake up, choose a worker at random (1, 2, or 3), and assign them an extra workload
    int targetWorker = random(1, 4); 
    int extraLoad = random(15, 35); // Amount of extra load

    currentExtraLoad[targetWorker] = extraLoad;

    SerialUSB.println("==================================================");
    SerialUSB.print(">>> ALARMA: Añadiendo +");
    SerialUSB.print(extraLoad);
    SerialUSB.print(" de carga a ");
    SerialUSB.println(thread_name[targetWorker]);
    SerialUSB.println("==================================================\n");

    // Maintain this disturbance for a random duration (from 3 to 8 seconds)
    // This gives the system time to readjust the loads to return to 84-86% range
    int activeTime = random(3000, 8001);
    chThdSleepMilliseconds(activeTime);

    // Remove the extra load all at once. This will cause an abrupt drop in total CPU usage,
    // forcing the system to readjust loads upward to return to the 84–86% range
    currentExtraLoad[targetWorker] = 0;

    SerialUSB.println("==================================================");
    SerialUSB.print(">>> ALARMA: Se retiró la carga extra de ");
    SerialUSB.println(thread_name[targetWorker]);
    SerialUSB.println("==================================================\n");
  }
}