# Ejercicio propuesto
Usando el código presentado en el último ejemplo como punto de partida, se pide desarrollar un
esquema de monitorización que permita balancear la carga computacional en el microprocesador y
llevarla a una condición “óptima”, definida como aquella en la que la utilización del procesador no
supera el 85% (15% del tiempo en la tarea idle/loop) y las tareas se ejecutan respetando el periodo
establecido, pero con el máximo posible de intensidad computacional.
Veamos algunos detalles e ideas:
* A diferencia del ejemplo anterior, en el que usábamos una intensidad fija para cada hebra o
tarea, definida mediante el vector threadLoad[NUM_THREADS], ahora cada tarea
dispondrá de un rango ajustable de intensidad o carga computacional y será necesario
declarar un valor mínimo y otro máximo.
* Se recomienda comenzar verificando que se respetan las frecuencias de operación de cada
hebra de trabajo con las hebras configuradas al valor mínimo de intensidad o carga (mínimo
consumo de tiempo de procesador).
* Una vez “estabilizado” el periodo/frecuencia de repetición de las hebras, la hebra top puede
ajustar de forma cíclica la intensidad de carga de cada hebra y evaluar la nueva
configuración verificando que:
1. Se respetan las frecuencias de repetición de las hebras
2. No se supera el umbral de carga del sistema
En caso contrario, deberá reducirse la intensidad computacional de las hebras y revaluar la
situación.
# Extra:
* Contemplar la posibilidad de que el tiempo de procesador de cada hebra pueda incluir un
cierto porcentaje de carga variable (aleatoria). En este supuesto, el margen que se ha
reservado de tiempo de CPU debería servir para absorber estos “picos” momentáneos de
carga. Es evidente que este supuesto exige cierta tolerancia o histéresis para distinguir
sobrecargas puntuales de otras situaciones de sobrecarga permanente, entendiendo como
sobrecarga aquella situación en la que el uso de procesador sobrepasa el umbral establecido
(p.e. del 85%).