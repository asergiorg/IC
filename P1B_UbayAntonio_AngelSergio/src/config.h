#pragma once
#include <Arduino.h>
#include <ChRt.h>
//------------------------------------------------------------------------------
// Parametrization
//------------------------------------------------------------------------------
#define USE_DOUBLE    TRUE   // Change to TRUE to use double precision (heavier)

#define CYCLE_MS      1000
#define NUM_THREADS   5  // Three working threads + loadEstimator (top) +
                         // loop (as the idle thread)
                         // TOP thread is thread with id 0

#define OBJECTIVE_LOAD 0.85

extern char thread_name[NUM_THREADS][15];

extern volatile uint32_t threadPeriod_ms[NUM_THREADS];
extern volatile int threadLoad[NUM_THREADS];

extern int minThreadLoad[NUM_THREADS];
extern volatile int maxThreadLoad[NUM_THREADS];
extern int aleatoryLoads[24];
extern float utilization_share[NUM_THREADS];

extern volatile uint32_t threadEffectivePeriod_ms[NUM_THREADS];
extern volatile uint32_t threadCycle_ms[NUM_THREADS];

typedef struct {
  thread_t * thd;
  systime_t lastSampleTime_i;
  sysinterval_t lastPeriod_i;
  sysinterval_t ticksTotal;
  sysinterval_t ticksPerCycle;
  float loadPerCycle_per;
} threadLoad_t;

typedef struct {
  threadLoad_t threadLoad[NUM_THREADS];
  uint32_t idling_per;
} systemLoad_t;

extern systemLoad_t sysLoad;
