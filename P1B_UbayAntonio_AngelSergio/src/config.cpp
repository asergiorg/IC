#include <Arduino.h>
#include "config.h"
#include <ChRt.h>
#include <ch.h>
#include <chconf.h>
#include <chcore.h>
#include <chlib.h>
#include <chlicense.h>
#include <chtypes.h>
#include <cmparams.h>
#include <hal_st.h>
#include <hal_st_lld.h>
#include <math.h>

char thread_name[NUM_THREADS][15] = {  "top",
                                       "worker_1", "worker_2", "worker_3",
                                       "idle" };

volatile uint32_t threadPeriod_ms[NUM_THREADS] = { CYCLE_MS, 200, 100, 200, 0 };
volatile int threadLoad[NUM_THREADS] = {0, 50, 50, 50, 0};

int minThreadLoad[NUM_THREADS] = {0, 1, 1, 1, 0};
volatile int maxThreadLoad[NUM_THREADS] = {0, 50, 50, 50, 0};
int aleatoryLoads[24] = {0, 0, 0, 0, 0, 20, 0, 40, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float utilization_share[NUM_THREADS] = {0, 0.30, 0.25, 0.30, 0};

volatile uint32_t threadEffectivePeriod_ms[NUM_THREADS] = { 0, 0, 0, 0, 0 };
volatile uint32_t threadCycle_ms[NUM_THREADS] = { 0, 0, 0, 0, 0 };

// Struct to measure the cpu load using the ticks consumed by each thread
systemLoad_t sysLoad;