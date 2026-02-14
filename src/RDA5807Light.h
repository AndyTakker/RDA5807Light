#pragma once

#include <Logs.h>
#include <RDA5807M_Regs.h>
#include <Wire.h>
#include <string.h>

#undef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


#define I2C_ADDR_SEQ 0x10   // Последовательный режим (все регистры с 0x02 последовательно)
#define I2C_ADDR_INDEX 0x11 // Индексный режим (к конкретному регистру)

#define RDA_HEADER "FM Radio" // Текст вместо названия станции, если RDS не работает

class RDA5807M {
  private:
  RDA_REGS_U rda = {0}; // Регистры RDA5807M

  char stationName[9];    // PS: 8 символов + '\0'
  char rdsStationName[8]; // Внутренний буфер для проверки корректности приема названия станции

  bool writeRegisters(uint8_t count);
  bool writeReg(uint8_t reg);
  bool readRegisters(void);
  void resetStation();

  public:
  RDA5807M();
  bool begin(uint32_t bound = 400000UL);

  // Управление частотой
  void setFrequency(uint16_t freq);
  void seekUp();
  void seekDown();
  void cancelSeek();

  // Аудио и настройки
  int8_t setVolume(int8_t volume);
  void setSoftMute(bool enabled);
  void setBassBoost(bool enabled);
  void setMono(bool enabled);

  // Информация
  void updateStatus();
  int getSignalStrength();
  bool isStationReady();
  bool isStereo();
  uint16_t getFrequency();

  // RDS
  bool isRdsReady();
  const char *getStationName(); // Возвращает имя станции (например, "DOPO*HOE")
};
