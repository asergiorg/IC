#include "top.h"
#include "config.h"

BSEMAPHORE_DECL(top_sem, true);

THD_FUNCTION(top, arg) {
  (void)arg;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize sysLoad struct
  memset(&sysLoad, 0, sizeof(sysLoad));

  systime_t lastTime_i = 0;
  systime_t period_i = TIME_MS2I(CYCLE_MS);

  // Reset top_sem as "taken"
  chBSemReset(&top_sem, true);

  while (!chThdShouldTerminateX()) {

    // Wait a certain amount of time
    // chThdSleepMilliseconds(CYCLE_MS);
    systime_t deadline_i = lastTime_i + period_i;
    if (deadline_i > chVTGetSystemTimeX()) {
      chBSemWaitTimeout(&top_sem, sysinterval_t(deadline_i - chVTGetSystemTimeX()));
    }

    // Turn on the LED
    digitalWrite(LED_BUILTIN, HIGH);

    // Accumulated ticks for this cycle
    uint32_t accumTicks = 0;

    // This assumes that no other thread will accumulate ticks during this sampling
    // so we can use this timestamp for all threads
    lastTime_i = chVTGetSystemTimeX();

    // tid starts at 1 because we do not include this thread (top)
    for (int tid = 1; tid < NUM_THREADS; tid++) {
      threadLoad_t * thdLoad = &(sysLoad.threadLoad[tid]);
      thdLoad->lastSampleTime_i = lastTime_i;
      systime_t ticks = chThdGetTicksX(thdLoad->thd);

      thdLoad->ticksPerCycle = ticks - thdLoad->ticksTotal;
      thdLoad->ticksTotal = ticks;
      accumTicks += thdLoad->ticksPerCycle;
    }

    for (int tid = 1; tid < NUM_THREADS; tid++) {
      threadLoad_t * thdLoad = &sysLoad.threadLoad[tid];
      thdLoad->loadPerCycle_per = (100 * (float)thdLoad->ticksPerCycle) / accumTicks;
      SerialUSB.print(thread_name[tid]);
      SerialUSB.print("  ticks(last cycle): ");
      SerialUSB.print(thdLoad->ticksPerCycle);
      SerialUSB.print("  CPU(%): ");
      SerialUSB.print(thdLoad->loadPerCycle_per);
      SerialUSB.print("   Cycle duration(ms): ");
      SerialUSB.print(threadCycle_ms[tid]);
      SerialUSB.print("  period(ms): ");
      SerialUSB.println(threadEffectivePeriod_ms[tid]);
    }
    SerialUSB.println();


    float estimations[NUM_THREADS] = {0., 0., 0., 0., 0.};

    // Estimate the charge per iteration
    for (int tid = 1; tid < NUM_THREADS-1; tid++){
      if (threadLoad[tid] != 0){
        float runsPerWindow = CYCLE_MS / (float)threadPeriod_ms[tid];
        estimations[tid] = (float)sysLoad.threadLoad[tid].ticksPerCycle / runsPerWindow / threadLoad[tid];
      }
    }

    // Calculate the max value of the thread
    for (int tid = 1; tid < NUM_THREADS-1; tid++){
        int period = (int)threadPeriod_ms[tid];
  
        int max = maxThreadLoad[tid];
        float cost = estimations[tid];
        if (cost > 0) max = utilization_share[tid] * period / cost; 
  
        maxThreadLoad[tid] = max;
    }

    int nextLoads[NUM_THREADS] = {0, 0, 0, 0, 0};

    // Calculate the next load
    for (int tid = 1; tid < NUM_THREADS-1; tid++){
      threadLoad_t * idleThdLoad = &sysLoad.threadLoad[NUM_THREADS - 1];
      float idleLoad = ((float)idleThdLoad->ticksPerCycle) / accumTicks;
      float scaleFactor = OBJECTIVE_LOAD - 1 + idleLoad;
      int load = threadLoad[tid] + threadLoad[tid] * scaleFactor;
      load = (load < minThreadLoad[tid]) ? minThreadLoad[tid] : load;
      load = (load > maxThreadLoad[tid]) ? maxThreadLoad[tid] : load;
      nextLoads[tid] = load;
    }

    float worker1Load = (float)(nextLoads[1] * estimations[1]) / threadPeriod_ms[1];
    float worker2Load = (float)(nextLoads[2] * estimations[2]) / threadPeriod_ms[2];
    float worker3Load = (float)(nextLoads[3] * estimations[3]) / threadPeriod_ms[3]; 

    // Satisfy period restriction
    while((worker1Load + worker2Load + worker3Load) > OBJECTIVE_LOAD) {
      float factor = OBJECTIVE_LOAD / (worker1Load + worker2Load + worker3Load);
      for (int i=1; i < 4; i++){
        int load = nextLoads[i] * factor;
        load = (load < minThreadLoad[i]) ? minThreadLoad[i] : load;
        nextLoads[i] = load;

      }

      worker1Load = (float)(nextLoads[1] * estimations[1]) / threadPeriod_ms[1];
      worker2Load = (float)(nextLoads[2] * estimations[2]) / threadPeriod_ms[2];
      worker3Load = (float)(nextLoads[3] * estimations[3]) / threadPeriod_ms[3];
    }

    // Apply scale factor
    for(int i = 1; i < 4; i++){
      threadLoad[i] = nextLoads[i];
    }

    // Switch off the LED
    digitalWrite(LED_BUILTIN, LOW);
  }
}