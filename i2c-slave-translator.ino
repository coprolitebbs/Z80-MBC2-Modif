#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// Define Slave I2C Address
#define SLAVE_ADDR 0x08

// Define Slave answer size
#define RDBUFFERLEN 1

//#define USE_SOFT_SERIAL 1

//Для Serial-порта выбраны отдельные пины, реализующие SoftSerial-порт
//Это сделано для возможности отладки через основной Serial-порт
#define RS232_RX_PIN 3  // Rx Pin
#define RS232_TX_PIN 2  // Tx Pin

//Пин для сигнализирования модему о том, что можно посылать данные
//Если HIGH – модем посылает, мы их читаем отсюда с SoftSerial
#define PIN_ACCESS 9

#ifdef USE_SOFT_SERIAL
SoftwareSerial Sserial(RS232_RX_PIN, RS232_TX_PIN);  // (Rx, Tx)
#endif

bool DataReaded = false;
bool DataNeedOut = false;
String ReadedData = "";
String OutData = "";
uint8_t rec[RDBUFFERLEN];
byte reclen = 0;
byte recptr = 0;

char outrec[256];
byte outreclen = 0;
byte outrecptr = 0;
bool locked = false;
//String receiveBuffer = "";
byte lastch = 0;

//Чтение по i2c прилетевшего байта и сразу отправка его в Serial-порт модема.
//Все равно, при этом событии он простаивает
void receiveEvent(int numBytes) {
  PORTB |= _BV(1);
  while (Wire.available()) {
    byte x = Wire.read();
#ifdef USE_SOFT_SERIAL
    Sserial.write((char)x);
#else
    Serial.write((char)x);
#endif
  }
}

//Отправка по запросу с Master'a на шину i2с байт с буфера считанных с Serial модема данных
//Отправляется один байт, за ним байт флага. Если байт последний в буфере, то, флаг = 0, иначе 1
//Если же данных с модема вообще небыло, вернем флаг = 2
void requestEvent() {
  uint8_t pack[2];
  pack[0] = 0;
  pack[1] = 2;    //Атрибут байта установим по умолчанию в 2. Это значит, что с модема ничего не читалось,
				  //и нам нечего возвращать на шину I2C компьютеру
  PORTB |= _BV(1);
  if (DataReaded) {
    pack[0] = (uint8_t)rec[recptr++];
    pack[1] = 1;
    if (recptr == reclen) {
      DataReaded = false;
      reclen = 0;
      recptr = 0;      
      pack[1] = 0;
    }
  }
  Wire.write(pack,2);
  delayMicroseconds(1);
}



//Цикличная функция чтения с порта модема в буфер накопленных байт.
//Поскольку, на прием-передачу Serial прерываний не предусмотрено,
//могут возникать ситуации, что при рассинхронизации готовности к передачи модема и готовности
//к приему данной платы-транслятора окна приема и передачи могут расходиться.
//Опытным путем установлено, что чем меньше этот буфер (RDBUFFERLEN), тем стабильнее работает прием-передача,
//и байты не теряются.
void readData() {
  //byte sz = 0;
  if (!DataReaded) {
    locked = true;
    reclen = 0;
    //Устанавливаем в HIGH пин разрешения записи в порт для модема
    //digitalWrite(PIN_ACCESS, HIGH);
    PORTB &= ~(_BV(1));
#ifdef USE_SOFT_SERIAL
    //while ((Sserial.available() > 0) && (reclen <= RDBUFFERLEN)) {
    while ((Sserial.available() > 0) && (reclen <= RDBUFFERLEN)) {
      rec[reclen++] = Sserial.read();
      //reclen++;
      delayMicroseconds(5);
    }
#else
    delayMicroseconds(10);
      while (Serial.available() && (reclen < RDBUFFERLEN)) {
        rec[reclen++] = Serial.read();
        //delayMicroseconds(2);
      }
#endif
    //Устанавливаем в LOW пин разрешения записи в порт для модема
    //digitalWrite(PIN_ACCESS, LOW);
    PORTB |= _BV(1);
    if (reclen > 0) {
      recptr = 0;
      DataReaded = true;
    }
    locked = false;
  }
}


#ifdef USE_SOFT_SERIAL

//Вывод в Serial для отладки
void printData() {
  PORTB |= _BV(1);
  if (DataReaded) {
    for (byte i = 0; i < reclen; i++) {
      Serial.write((char)rec[i]);
    }
    DataReaded = false;
  }
}

//Чтение с Serial для отладки
void getSerial() {
  PORTB |= _BV(1);
  outreclen = 0;
  outrecptr = 0;
  while ((Serial.available() > 0) && (outreclen < 255)) {
    outrec[outreclen] = Serial.read();
    outreclen++;
  }
  if (outreclen > 0) DataNeedOut = true;

}

//Отправка считанного в SoftSerial для отладки
void outData() {
  PORTB |= _BV(1);
  locked = true;
  if (DataNeedOut) {
    for (int i = 0; i < outreclen; i++) {
      Sserial.write((char)outrec[i]);
    }
    //Sserial.print(OutData);
    DataNeedOut = false;
    outreclen = 0;
  }
  locked = false;
}


#endif





void setup() {

  pinMode(RS232_RX_PIN, INPUT_PULLUP);
  pinMode(RS232_TX_PIN, OUTPUT);

  pinMode(PIN_ACCESS, OUTPUT);
  digitalWrite(PIN_ACCESS, LOW);

  Wire.begin(SLAVE_ADDR);

  // Function to run when data requested from master
  Wire.onRequest(requestEvent);

  // Function to run when data received from master
  Wire.onReceive(receiveEvent);

  // Setup Serial Monitor
#ifdef USE_SOFT_SERIAL
  Sserial.begin(4800);
#endif
  Serial.begin(4800/*,SERIAL_8E2*/);
  delay(500);
}

void loop() {
  readData();
#ifdef USE_SOFT_SERIAL
  printData();
  getSerial();
  outData();
#endif

}
