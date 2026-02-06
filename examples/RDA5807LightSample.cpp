//==============================================================================
// Пример использования библиотеки RDA5807Light.
//------------------------------------------------------------------------------
#include <RDA5807Light.h>
#include <SysClock.h>

#define WIRE_CLOCK 100000UL // 100кГц
RDA5807M radio;             // Создаем экземпляр RDA5807M

int8_t scanDirection = 0; // 0 - не сканируем, 1 - сканируем вверх, -1 - вниз

void printPrm(); // Печать в порт текущих параметров радио

//==============================================================================
int main() {

  SystemCoreClockUpdate();
#ifdef LOG_ENABLE
  USART_Printf_Init(115200);
#endif
  logs("SystemClk: %lu\r\n", SystemCoreClock);        // Для посмотреть частоту процесора (48мГц)
  logs("   ChipID: 0x%08lX\r\n", DBGMCU_GetCHIPID()); // Для посмотреть ID чипа, от нефиг делать

  while (!radio.begin(WIRE_CLOCK)) { // Запускаем радио
    logs("RDA5807M not answering... waiting and trying again\r\n");
    delay(500);
  }

  radio.setFrequency(870); // Встанем на начало диапазона
  radio.setBassBoost(true);
  radio.setVolume(10);
  logs("Radio started!\r\n");

  uint32_t lastUpdate = 0;
  uint32_t lastScan = 0;
  while (1) {
    uint32_t now = millis();
    if (now - lastUpdate > 1000) { // Раз в секунду читаме параметры и выводим в порт
      lastUpdate = now;
      radio.updateStatus(); // Прочитаем регистры RDA
      printPrm();
    }

    if (now - lastScan > 10000) { // Раз в 10 секунд переключаемся на следующую станцию
      lastScan = now;

      scanDirection = 1;
      if (scanDirection != 0) { // Переключение каналов
        logs("Dir: %d\r\n", scanDirection);
        if (scanDirection > 0) {
          radio.seekUp();
          logs("Seek up\r\n");
          delay(500);
        } else if (scanDirection < 0) {
          radio.seekDown();
          logs("Seek down\r\n");
          delay(500);
        }
        scanDirection = 0;
      }
    }
  }
}

void printPrm() {
  logs("Freq: %d | RSSI: %d | STC: %d | RDS Ready: %d | Station: %s\r\n",
       radio.getFrequency(), radio.getSignalStrength(), radio.isStationReady(),
       radio.isRdsReady(), radio.getStationName());
}
