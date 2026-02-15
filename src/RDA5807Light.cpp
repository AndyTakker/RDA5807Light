//============================================================== (c)A.Kolesov ==
// Попытка создать легкую библиотеку для работы с модулем RDA5807 с внятным
// описанием регистров и минимально необходимым функционалом.
// Функции RDS тоже включены в библиотеку, но учитывая полный мусор, который
// передают радиостанции в сигнале RDS, большой вопрос - нужны ли они?
// Напоминание:
// - управление RDA происходит через *аппаратный* интерфейс I2C
// - модуль RDA5807 питается от 3.3в
// - не забывать подтяжку SDA, SCL к +3.3В через 10кОм.
// - на шину SCL повесить конденсатор 30пф. На слишком крутых фронтах SCL
//   модуль может не отвечать.
// - к выходам модуля ROUT и LOUT подключаться через разделительные конденсаторы
//   от 100мкФ.
//------------------------------------------------------------------------------
#include "Rda5807Light.h"

RDA5807M::RDA5807M() {
}

bool RDA5807M::begin(uint32_t bound) {

  Wire.begin(bound);

  // Настройка регистров по умолчанию. Изначально все регистры равны нулю,
  // поэтому инициализируем только то, что отлично от нуля.

  // Регистр 0x02
  rda.named.reg02.bits.DHIZ = 1;       // Audio output high-z disable, 1 = normal operation
  rda.named.reg02.bits.DMUTE = 1;      // Soft mute disable, 1 = normal operation
  rda.named.reg02.bits.ENABLE = 1;     // Power up enable, 1 = enabled
  rda.named.reg02.bits.SOFT_RESET = 1; // Soft reset, 1 = reset (auto-clear)
  rda.named.reg02.bits.RDS_EN = 1;     // RDS enable, 1 = enabled
  rda.named.reg02.bits.NEW_METHOD = 1; // New Demodulate Method: improves sensitivity ~1dB

  // Регистр 0x04
  rda.named.reg04.bits.SOFTMUTE_EN = 1; // Soft Mute Enable: 1=Enabled
  rda.named.reg04.bits.AFCD = 1;        // AFC Disable: 0=AFC enabled, 1=AFC disabl

  // Регистр 0x05
  rda.named.reg05.bits.VOLUME = 15;
  rda.named.reg05.bits.SEEKTH = 0b1000;
  rda.named.reg05.bits.INT_MODE = 1;
  rda.named.reg05.bits.LNA_ICSEL = 0b11; // Максимальный ток усилителя
  rda.named.reg05.bits.LNA_PORT = 0b10;  // LNAP (выбор зависит от антенны)

  // Регистр 0x07
  // Soft Blend — это технология плавного перехода между стерео и моно режимами
  // при ухудшении качества сигнала, реализованная в чипах семейства RDA5807M
  // для улучшения аудиовосприятия.
  rda.named.reg07.bits.TH_SOFTBLEND = 0b10000;
  rda.named.reg07.bits.SOFTBLEND_EN = 1;

  // Пишем все регистры в RDA
  bool result = writeRegisters(7); // 0x02–0x08
  return result;
}

//==============================================================================
// Последовательная запись в регистры модуля, начиная с 0x02 и столько,
// сколько указано.
//------------------------------------------------------------------------------
bool RDA5807M::writeRegisters(uint8_t count) {
  uint8_t reg = 0x02; // Всегда пишем начиная с начала
  uint8_t buf[count * 2];
  for (uint8_t i = 0; i < count; i++) {
    uint16_t value = rda.regs[reg + i];
    buf[i * 2] = (uint8_t)(value >> 8);       // Старший байт (MSB)
    buf[i * 2 + 1] = (uint8_t)(value & 0xFF); // Младший байт (LSB)
  }
  Wire.beginTransmission(I2C_ADDR_SEQ);
  Wire.write(buf, count * 2);
  uint8_t ret = Wire.endTransmission();
  return (ret == 0);
}
//==============================================================================
// Запись в конкретный регистр модуля
// Данные уже должны быть помещены в буфер в ячейку конкретного регистра
//------------------------------------------------------------------------------
bool RDA5807M::writeReg(uint8_t reg) {
  Wire.beginTransmission(I2C_ADDR_INDEX); // Индексный доступ к регистрам
  Wire.write(reg);                        // Передаем адрес регистра
  uint16_t value = rda.regs[reg];         // Значение в буфере, соответствующее регистру
  uint8_t buf[2];
  buf[0] = (uint8_t)(value >> 8);   // Старший байт (MSB)
  buf[1] = (uint8_t)(value & 0xFF); // Младший байт (LSB)
  Wire.write(buf, 2);               // Передаем в модуль
  uint8_t ret = Wire.endTransmission();
  return (ret == 0); // Возвращает 0 если все хорошо
}

//==============================================================================
// Это функция чтения всех читабельных регистров по феншую, как в документации.
//------------------------------------------------------------------------------
bool RDA5807M::readRegisters() {
  uint8_t count_regs = 6; // 6 регистров по 2 байта для чтения из RDA
  Wire.requestFrom(I2C_ADDR_SEQ, (uint8_t)(count_regs * 2));
  while (Wire.available() < count_regs * 2)
    ;
  uint8_t reg = 0x0A; // Читаемые регистры начинаются с 0x0A
  for (int i = 0; i < count_regs; i++) {
    uint16_t val = Wire.read() << 8;
    val |= Wire.read();
    rda.regs[reg + i] = val; // Помещаем прочитанные данные в union регистры
  }
  return true;
}

void RDA5807M::setFrequency(uint16_t freq) {
  resetStation();
  // Проверка диапазона (87.0–108.0 МГц)
  int fr = constrain(freq, 870, 1080); // Сразу переведем в 100 килогерцовые значения
  int channel = (fr - 870);            // 100 кГц шаг (0.1 МГц)
  rda.named.reg02.bits.SOFT_RESET = 0;
  // rda.named.reg02.bits.ENABLE = 1;
  rda.named.reg03.bits.CHAN = channel;
  rda.named.reg03.bits.TUNE = 1;
  writeRegisters(2);
}

//==============================================================================
// Поиск станции вверх по частоте. Останавливается на первой же найденной
// станции.
//------------------------------------------------------------------------------
void RDA5807M::seekUp() {
  resetStation();
  rda.named.reg02.bits.SEEKUP = 1;
  rda.named.reg02.bits.SEEK = 1;
  writeReg(0x02);
}

//==============================================================================
// Поиск станции вниз по частоте. Останавливается на первой же найденной
// станции.
//------------------------------------------------------------------------------
void RDA5807M::seekDown() {
  resetStation();
  rda.named.reg02.bits.SEEKUP = 0;
  rda.named.reg02.bits.SEEK = 1;
  writeReg(0x02);
}

//==============================================================================
// Отмена поиска. Может когда-то понадобится. Например, приема нет, а запустили
// сканирование списка станции. Чтобы бесконечно не сканировать.
//------------------------------------------------------------------------------
void RDA5807M::cancelSeek() {
  rda.named.reg02.bits.SEEK = 0;
  writeReg(0x02);
}

//==============================================================================
// Установка громкости. Проверяет допустимый диапазон
// Возвращает фактически установленный уровень громкости, с учетом ограничений.
// Нулевой уровень громкости не означает тишину. Звук все равно есть.
// Поэтому если уменьшаем меньше нуля - включается Soft Mute.
//------------------------------------------------------------------------------
int8_t RDA5807M::setVolume(int8_t volume) {
  volume = constrain(volume, -1, 15);
  if (volume < 0) {
    setSoftMute(true);
  } else {
    setSoftMute(false);
    rda.named.reg05.bits.VOLUME = volume;
    writeReg(0x05);
  }
  return volume;
}

void RDA5807M::setSoftMute(bool enabled) {
  rda.named.reg02.bits.DMUTE = !enabled;      // Разрешаем Soft Mute
  rda.named.reg04.bits.SOFTMUTE_EN = enabled; // Разрешаем Soft Mute
  writeReg(0x04);
}

void RDA5807M::setBassBoost(bool enabled) {
  rda.named.reg02.bits.BASS = enabled;
  writeReg(0x02);
}

void RDA5807M::setMono(bool enabled) {
  rda.named.reg02.bits.MONO = enabled; // 1 → принудительный моно
  writeReg(0x02);
}

//==============================================================================
// Возвращает текущий уровень сигнала в диапазоне от 0 до 127.
// Шкала логарифмическая.
//------------------------------------------------------------------------------
int RDA5807M::getSignalStrength() {
  return rda.named.reg0B.bits.RSSI;
}

//==============================================================================
// Возвращает true, если станция готова к воспроизведению
//------------------------------------------------------------------------------
bool RDA5807M::isStationReady() {
  return rda.named.reg0A.bits.STC;
}

bool RDA5807M::isStereo() {
  return rda.named.reg0A.bits.ST;
}

//==============================================================================
// Возвращает текущую частоту в 100кГц шагах, т.е. 105.0МГц вернутся как 1050
//------------------------------------------------------------------------------
uint16_t RDA5807M::getFrequency() {
  int channel = rda.named.reg0A.bits.READCHAN;
  return (870 + channel);
}

//==============================================================================
// Если название станции было извлечено из RDS, то возвращает его, если нет -
// возвращает стандартный текст, определенный в RDA_HEADER.
// Название дополняется пробелами до 8 символов.
//------------------------------------------------------------------------------
const char *RDA5807M::getStationName() {
  if (stationName[0] == '\0') {         // Если название не извлечено
    memcpy(stationName, RDA_HEADER, 8); // Копируем стандартное
  }
  for (int i = 0; i < 8; i++) { // Добъем до 8 символов пробелами
    if (stationName[i] == '\0') {
      stationName[i] = ' ';
    }
  }
  return stationName;
}

// !!! Не выбрасывать этот вариант разбора даных RDS. Он как-то работал !!!
/*
bool RDA5807M::hasRDS() {
  return reg02.RDS_EN; // RDS_EN
}


bool RDA5807M::getRDS(uint16_t rdsBlocks[4]) {
  if (reg0A.RDSR) {
    rdsBlocks[0] = buffer[0x0A];
    rdsBlocks[1] = buffer[0x0B];
    rdsBlocks[2] = buffer[0x0C];
    rdsBlocks[3] = buffer[0x0D];
    return true;
  }
  return false;
}

uint16_t RDA5807M::getPI() {
  if (readRegisters(0x0A, 4)) {
    return buffer[0x0B] >> 1; // PI в RDS Block B
  }
  return 0;
}

void RDA5807M::processRDS() {
  uint16_t rds[4];
  if (!getRDS(rds)) {
    // logs("RDS not enabled\r\n");
    return;
  }
  if (!reg0A.RDSS) {
    // logs("RDS not valid\r\n");
    return; // Нет валидного RDS
  }
  // Block B: содержит тип и PS
  // uint16_t blockB = rds[1];
  // uint8_t blera = reg0B.BLERA; // (rds[0] >> 14) & 0x03;
  uint8_t blerb = reg0B.BLERB; // (rds[1] >> 14) & 0x03;
  if (blerb > 2) {
    // logs("BLERB > 2\r\n");
    return; // Слишком много ошибок
  }
  // uint8_t type = (blockB >> 12) & 0x0F;
  // if (type != 0)
  //   return; // Только тип 0 поддерживает PS
  uint16_t rdsb = rds[1];
  uint16_t rdsc = rds[2];
  uint16_t rdsd = rds[3];
  // Serial.print("RDSB: "); Serial.println(rdsb>>12, HEX);
  // if ((rdsb >> 12) & 0x0F == 0) {              // Группа 0
  uint8_t ab_flag = (rdsb >> 11) & 1; // Смена A/B

  // PS: 4 символа в C и D
  char ps1 = (rdsc >> 8) & 0xFF;
  char ps2 = rdsc & 0xFF;
  char ps3 = (rdsd >> 8) & 0xFF;
  char ps4 = rdsd & 0xFF;

  // Сохраняем в буфер (с учётом A/B-смены)
  if (ab_flag == 0) {
    // logs("AB_FLAG == 0\r\n");
    stationName[0] = ps1;
    stationName[1] = ps2;
    stationName[2] = ps3;
    stationName[3] = ps4;
  } else {
    // logs("AB_FLAG == 1\r\n");
    stationName[4] = ps1;
    stationName[5] = ps2;
    stationName[6] = ps3;
    stationName[7] = ps4;
  }
  stationName[8] = '\0';
  // }

  // // PS: 4 символа в Block C и D
  // uint16_t ps1 = rds[2]; // Block C
  // uint16_t ps2 = rds[3]; // Block D

  // // Извлекаем символы (каждый по 8 бит)
  // stationName[0] = (ps1 >> 8) & 0xFF;
  // stationName[1] = ps1 & 0xFF;
  // stationName[2] = (ps2 >> 8) & 0xFF;
  // stationName[3] = ps2 & 0xFF;

  // // Завершаем строку
  // stationName[4] = '\0';
}

void RDA5807M::update() {
  // uint32_t now = millis();
  // if (now - lastRDSUpdate > 50) {
  //   processRDS();
  //   lastRDSUpdate = now;
  // }
}
*/

bool RDA5807M::isRdsReady() {
  return rda.named.reg0A.bits.RDSR;
}

//==============================================================================
// Метод должен вызываться в основном цикле с заданной периодичностью.
// Он читает регистры RDA и кладет их в буфер с адреса 0x0A.
//------------------------------------------------------------------------------
void RDA5807M::updateStatus(void) {
  readRegisters(); // Читаем все читабельные регистры (0x0A - 0x0F)

  if (rda.named.reg0A.bits.STC) { // Станция настроена, сбросим биты настройкии поиска
    rda.named.reg03.bits.TUNE = 0;
    writeReg(0x03);
    rda.named.reg02.bits.SEEK = 0;
    writeReg(0x02);
  }

  // Проверка готовности RDS данных
  if (!rda.named.reg0A.bits.RDSR) { // RDS данные не готовы
    return;
  }

  // Без блока ниже работает лучше
  /*   // Toggle RDS flag to request new data
    reg02.RDS_EN = 0; // clear RDS flag
    buffer[0x02] = to_uint16_t(reg02);
    writeReg(0x02);
    reg02.RDS_EN = 1; // set RDS flag
    buffer[0x02] = to_uint16_t(reg02);
    writeReg(0x02);
   */

  if (rda.named.reg0B.bits.ABCD_E) { // В регистрах REG_B..F данные E, нам не нужны
    return;
  }
  // Декодируем RDS сообщение
  if ((rda.named.reg0D.bits.RDSB & 0xF800) == 0x0000) {       // Это название станции
    uint8_t offset = (rda.named.reg0D.bits.RDSB & 0x03) << 1; // Получить позицию принятого символа
    uint8_t c1 = rda.named.reg0F.bits.RDSD >> 8;              // Получить символ 1
    uint8_t c2 = rda.named.reg0F.bits.RDSD;                   // Получить символ 2
    // Копируем символы названия станции только если они приняты дважды
    if (rdsStationName[offset] == c1) { // Первый символ принят дважды
      stationName[offset] = c1;         // Копируем в название
    } else {
      rdsStationName[offset] = c1; // Сохраняем для следующей проверки
    }
    if (rdsStationName[offset + 1] == c2) { // Второй символ принят дважды
      stationName[offset + 1] = c2;         // Копируем в название
    } else {
      rdsStationName[offset + 1] = c2; // Сохраняем для следующей проверки
    }
  }
}

//==============================================================================
// Функция вызывается при любом изменении станции и чистит.
//------------------------------------------------------------------------------
void RDA5807M::resetStation() {
  memset(rdsStationName, 0, sizeof(rdsStationName));
  memset(stationName, 0, sizeof(stationName));
}
