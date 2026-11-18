#include "frame_decoder.h"
#include "printer_manager.h"
#include "token_classifyer.h"

#define FRAME_START    0xF
#define FRAME_END      0x0

#define FRAME_ONE_SHOT 0x1
#define FRAME_PERIODIC 0x2
#define FRAME_STATUS   0x3

static bool    inFrame = false;
static uint8_t frameType = 0;
static uint8_t expectedBlocks = 0;
static uint8_t receivedBlocks = 0;

static const char *frameName(uint8_t type) {
  switch (type) {
    case FRAME_ONE_SHOT: return "one-shot";
    case FRAME_PERIODIC: return "periodic";
    case FRAME_STATUS:   return "status";
    default:             return "unknown";
  }
}

void resetFrameState() {
  inFrame = false;
  receivedBlocks = 0;
  expectedBlocks = 0;
}

static void readCommandResponse(const CanPacket &p) {
  uint32_t message = ((uint32_t)p.data[0] << 24) | ((uint32_t)p.data[1] << 16) |
                     ((uint32_t)p.data[2] << 8)  |  (uint32_t)p.data[3];

  String operation = ((uint8_t)(message >> 24) == 1) ? "us" : "help";
  uint8_t sensor = (message >> 16) & 0xFF;
  uint8_t error = (message >> 11) & 0x1;
  String firstParameter = getFirstParameter(message >> 12 & 0xF);
  String secondParameter = getSecondParameter(message >> 8 & 0x7);

  String decodification = operation + " " + firstParameter + " " + secondParameter;
  if (error) {
    Serial.println(decodification + " went wrong on sensor " + String(sensor));
    printErrorMessage(message & 0xFF);
  } else {
    Serial.println(decodification + " successfully executed on sensor " + String(sensor));
  }
}

static void readFrameDelimiter(const CanPacket &p) {
    uint8_t marker = p.data[0] >> 4;
    uint8_t type   = p.data[0] & 0x3;
    uint8_t blocks = p.data[1];

    if (marker == FRAME_START) {
        if (inFrame) Serial.println("Warning: frame starting without ending the previous one");
        if (type < FRAME_ONE_SHOT) {
            Serial.println("Warning: unknown type of frame");
            resetFrameState();
            return;
        }
        inFrame = true;
        frameType = type;
        expectedBlocks = blocks;
        receivedBlocks = 0;
        Serial.println(String("[") + frameName(type) + "] " + String(blocks) + " block(s):");

    } else if (marker == FRAME_END) {
        if (!inFrame) {
            Serial.println("Warning: ending frame without starting");
            return;
        }
        if (type != frameType) Serial.println("Warning: type ending frame does not match with the type of the starting frame");
        if (blocks != receivedBlocks || blocks != expectedBlocks) {
            Serial.println("Warning: expected " + String(expectedBlocks) +
                            " blocks, received " + String(receivedBlocks) +
                            " (ending frame indicates " + String(blocks) + ")");
        }
        resetFrameState();

    } else {
        Serial.println("Warning: unknown frame indicator");
    }
}

static void readDataBlock(const CanPacket &p) {
    if (!inFrame) {
        Serial.println("Warning: data blocks received out of frame");
        return;
    }
    receivedBlocks++;
    uint8_t sensor = p.data[0];

    if (frameType == FRAME_STATUS) {
        if (p.length != 6) {
            Serial.println("Aviso: bloque de status con tamaño incorrecto");
            Serial.println("Warning: wrong status block size");
            return;
        }
        uint16_t i2c = ((uint16_t)p.data[1] << 8) | p.data[2];
        uint8_t minDelay = p.data[3];
        uint8_t flagF = (p.data[4] >> 2) & 0x1;
        uint8_t unit = p.data[4] & 0x3;
        uint8_t period = p.data[5];

        Serial.println("  Sensor " + String(sensor) +
                    " | I2C 0x" + String(i2c, HEX) +
                    " | minimum delay " + String(minDelay) +
                    " | F=" + String(flagF) +
                    " | period " + String(period) + " " + getSecondParameter(unit) + " ms");
    } else {
        if (p.length != 3) {
            Serial.println("Warning: wrong medition block size");
            return;
        }
        uint8_t  unit = p.data[1] >> 6;
        uint16_t distance = ((uint16_t)(p.data[1] & 0x3F) << 8) | p.data[2];

        Serial.println("  Sensor " + String(sensor) + ": " +
                    String(distance) + " " + getSecondParameter(unit));
    }
}

void readResponse(const CanPacket &p) {
    switch (p.length) {
        case 4:  readCommandResponse(p); break;
        case 2:  readFrameDelimiter(p);  break;
        case 3:
        case 6:  readDataBlock(p);       break;
        default:
        Serial.println("Warning: CAN packet of " + String(p.length) + " non-recognize bytes");
    }
}