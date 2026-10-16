/* -----------------------------------------------------------------------------
 *                        Práctica 1 - Primera parte
 *
 * Objetivos principales:
 * 1. Configurar el RTC con la fecha y hora de compilación del firmware.
 * 2. Programar una alarma periódica del RTC cada 10 segundos para simular la
 *    lectura de un sensor.
 * 3. Generar una cadena de texto con la fecha y hora en cada activación de la
 *    alarma.
 * 4. Almacenar dicha cadena en un fichero ubicado en la memoria FLASH externa
 *    de la MKR 1310.
 * 5. Mantener el microcontrolador en modo sleep indefinido y despertarlo
 *    mediante la alarma del RTC.
 *
 * Funcionalidades extra:
 * E1. Implementar una interrupción externa por flanco de bajada en un pin
 *     configurado como entrada pull-up, registrando en el fichero la fecha y
 *     hora junto con una indicación de que el evento fue provocado por una
 *     interrupción externa.
 * E2. Implementar una segunda interrupción externa que permita realizar
 *     el volcado o visualización del contenido completo del fichero almacenado
 *     en la memoria FLASH.
 *
 *  Autores: Ubay Antonio Batista Santana,
 *           Ángel Sergio Reyes Guedes
 * -----------------------------------------------------------------------------
 */

//-------------------------------- Dependencias --------------------------------

#include <time.h>
#include <RTCZero.h>
#include <Arduino_MKRMEM.h>
#include <ArduinoLowPower.h>

//-------------------------- Declaración de variables --------------------------

RTCZero rtc;

#define EXTERNAL_PIN 5
#define DUMP_PIN 6
#define PERIOD_SEC 10
#define DEBOUNCE_MS 200
#define USB_CONFIGURATION_TIME 500

volatile uint32_t _period_sec = 0;
volatile bool alarmFlag = false;
volatile bool externalFlag = false;

char textToSave[96];

Arduino_W25Q16DV flash(SPI1, FLASH_CS);
char filename[] = "datos.txt";

int writtenLines = 0;
int writtenLinesByExternalInterruption = 0;

volatile bool dumpRequested = false;

//---------------------------- Secondary functions -----------------------------

bool setDateTime(const char *date_str, const char *time_str) {
  char month_str[4];
  char months[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  uint16_t i, mday, month = 0, hour, min, sec, year;

  if (sscanf(date_str, "%3s %hu %hu", month_str, &mday, &year) != 3) return false;
  if (sscanf(time_str, "%hu:%hu:%hu", &hour, &min, &sec) != 3) return false;

  for (i = 0; i < 12; i++) {
    if (!strncmp(month_str, months[i], 3)) {
      month = i + 1;
      break;
    }
  }
  if (i == 12) return false;

  rtc.setTime((uint8_t)hour, (uint8_t)min, (uint8_t)sec);
  rtc.setDate((uint8_t)mday, (uint8_t)month, (uint8_t)(year - 2000));
  return true;
}

void setPeriodicAlarm(uint32_t period_sec, uint32_t offsetFromNow_sec) {
  _period_sec = period_sec;
  rtc.setAlarmEpoch(rtc.getEpoch() + offsetFromNow_sec);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
}

void setTextToSave(bool fromExternal) {
  const char *weekDay[7] = { "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat" };

  time_t epoch = rtc.getEpoch();

  struct tm stm;
  gmtime_r(&epoch, &stm);

  snprintf(textToSave, sizeof(textToSave), "%s %4u/%02u/%02u %02u:%02u:%02u",
           weekDay[stm.tm_wday],
           stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday,
           stm.tm_hour, stm.tm_min, stm.tm_sec);

  if (fromExternal) {
    strncat(textToSave, " Creado por interrupcion externa",
            sizeof(textToSave) - strlen(textToSave) - 1);
    writtenLinesByExternalInterruption++;
  }
}

//---------------------------------- Callbacks ---------------------------------

void alarmCallback() {
  alarmFlag = true;
  rtc.setAlarmEpoch(rtc.getEpoch() + _period_sec);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
}

void externalCallback() {
  externalFlag = true;
}

void dumpCallback() {
  dumpRequested = true;
}

//------------------------------ Files Management ------------------------------
void onExitWithError() {
  filesystem.unmount();
  exit(EXIT_FAILURE);
}

bool createFile() {
  File file = filesystem.open(filename, CREATE | TRUNCATE);
  if (!file) {
    SerialUSB.print("Creation of file ");
    SerialUSB.print(filename);
    SerialUSB.println(" failed. Aborting ...");
    onExitWithError();
    return false;
  }
  file.close();
  return true;
}

bool writeInFile() {
  File file = filesystem.open(filename, WRITE_ONLY | APPEND);
  if (!file) {
    SerialUSB.print("Opening file ");
    SerialUSB.print(filename);
    SerialUSB.println(" failed for appending. Aborting ...");
    onExitWithError();
    return false;
  }

  delay(USB_CONFIGURATION_TIME);

  SerialUSB.print("Writing: ");
  SerialUSB.print(textToSave);

  int const bytes_to_write = strlen(textToSave);
  int const bytes_written = file.write((void *)textToSave, bytes_to_write);
  int const nl_written = file.write((void *)"\n", 1);

  if (bytes_to_write != bytes_written || nl_written != 1) {
    SerialUSB.print("write() failed with error code ");
    SerialUSB.println(filesystem.err());
    SerialUSB.println("Aborting ...");
    file.close();
    onExitWithError();
    return false;
  }

  writtenLines++;
  SerialUSB.print(" | Written lines: ");
  SerialUSB.print(writtenLines);
  SerialUSB.print(" | Lines written by external interruption: ");
  SerialUSB.print(writtenLinesByExternalInterruption);
  SerialUSB.print(" | ");
  file.close();

  return true;
}

void dumpRecord() {
  SerialUSB.println("\n--- LOG DUMP START ---");

  File file = filesystem.open(filename, READ_ONLY);
  if (!file) {
    SerialUSB.print("Opening file ");
    SerialUSB.print(filename);
    SerialUSB.print(" failed for appending. Aborting ...");
    onExitWithError();
  } else {
    while (!file.eof()) {
      char c;
      int const bytes_read = file.read(&c, sizeof(c));
      if (bytes_read) {
        SerialUSB.print(c);
      }
    }
    file.close();
  }

  SerialUSB.println("--- END OF LOG DUMP ---\n");
}

//-------------------------------------------------------------------------------

void setup() {
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, LOW);

  SerialUSB.begin(9600);
  while (!SerialUSB) { ; }

  rtc.begin();
  flash.begin();

  if (!setDateTime(__DATE__, __TIME__)) {
    SerialUSB.println("setDateTime() failed!\nExiting ...");
    while (1) { ; }
  }

  int res = filesystem.mount();
  if (res != SPIFFS_OK && res != SPIFFS_ERR_NOT_A_FS) {
    SerialUSB.print("mount() failed with error code ");
    SerialUSB.println(res);
    exit(EXIT_FAILURE);
  }

  createFile();

  pinMode(EXTERNAL_PIN, INPUT_PULLUP);
  pinMode(DUMP_PIN, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(DUMP_PIN, dumpCallback, FALLING);
  LowPower.attachInterruptWakeup(EXTERNAL_PIN, externalCallback, FALLING);
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, alarmCallback, CHANGE);

  setPeriodicAlarm(PERIOD_SEC, PERIOD_SEC);

  SerialUSB.println("Sistema listo. Entrando en bajo consumo...");
  SerialUSB.flush();
}

void loop() {
  noInterrupts();
  bool fromAlarm = alarmFlag;
  bool fromExternal = externalFlag;
  bool fromDump = dumpRequested;

  alarmFlag = false;
  externalFlag = false;
  dumpRequested = false;
  interrupts();

  if (fromAlarm || fromExternal) {
    setTextToSave(fromExternal);

    if (writeInFile()) {
      SerialUSB.println("Se ha escrito con éxito.");
    } else {
      SerialUSB.println("No se ha escrito con éxito.");
    }

    if (fromExternal) {
      delay(DEBOUNCE_MS);
      noInterrupts();
      externalFlag = false;
      interrupts();
    }
  }

  if (fromDump) {
    uint32_t t = millis();
    while (!SerialUSB && (millis() - t < 2000)) {
      delay(10);
    }

    delay(USB_CONFIGURATION_TIME);

    dumpRecord();
  }

  if (!alarmFlag && !externalFlag && !dumpRequested) {

    SerialUSB.flush();
    delay(100);

    LowPower.sleep();
  }
}