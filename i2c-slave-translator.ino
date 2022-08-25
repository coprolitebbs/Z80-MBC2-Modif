#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// Define Slave I2C Address
#define SLAVE_ADDR 0x08

// Define Slave answer size
#define ANSWERSIZE 5

//Для Serial-порта выбраны отдельные пины, реализующие SoftSerial-порт
//Это сделано для возможности отладки через основной Serial-порт
#define RS232_RX_PIN 3  // Rx Pin
#define RS232_TX_PIN 2  // Tx Pin

//Пин для сигнализирования модему о том, что можно посылать данные
//Если HIGH – модем посылает, мы их читаем отсюда с SoftSerial
#define PIN_ACCESS 9


SoftwareSerial Sserial(RS232_RX_PIN, RS232_TX_PIN);  // (Rx, Tx)

//String answer = "Hello";
bool DataReaded = false;
bool DataNeedOut = false;
String ReadedData = "";
String OutData = "";
char rec[256];
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
  while (Wire.available()){
    byte x = Wire.read();
    Sserial.write((char)x);
  }
}

//Отправка по запросу с Master'a на шину i2с байт с буфера считанных с Serial модема данных 
//Отправляется один байт, за ним байт флага. Если байт последний в буфере, то, флаг = 0
void requestEvent() {
  byte resp = 0;//lastch;
  byte atr = 1;
  //if ((DataReaded) && (!locked)) {
  PORTB |= _BV(1);
  if (DataReaded) {
    resp = (byte)rec[recptr];
    recptr++;
    if (recptr == reclen) {
      DataReaded = false;
      reclen = 0;
      recptr = 0;
      atr = 0;
    }
  }  
  Wire.write(resp);
  Wire.write(atr);  
}



//Цикличная функция чтения с порта модема в буфер накопленных байт.
//Поскольку, на прием-передачу Serial прерываний не предусмотрено, 
//могут возникать ситуации, что при рассинхронизации готовности к передачи модема и готовности 
//к приему данной платы-транслятора окна приема и передачи могут расходиться.
//Опытным путем установлено, что чем меньше этот буфер, тем стабильнее работает прием-передача,
//и байты не теряются.
void readData() {
  //byte sz = 0;
  if (!DataReaded) {
    locked = true;
    reclen = 0;
    
    //digitalWrite(PIN_ACCESS, HIGH);
    //Устанавливаем в HIGH пин разрешения записи в порт для модема 
    PORTB &= ~(_BV(1));
    //int mx = Sserial.available();
    //if (mx > 255) mx = 255;
    while ((Sserial.available() > 0) && (reclen < 2)) {
      rec[reclen] = Sserial.read();
      reclen++;
    }
    //digitalWrite(PIN_ACCESS, LOW);
    //Устанавливаем в LOW пин разрешения записи в порт для модема 
    PORTB |= _BV(1);
    if (reclen > 0) {
      recptr = 0;
      DataReaded = true;
    }
    locked = false;
  }
}


//Вывод в Serial для отладки
void printData() {
  PORTB |= _BV(1);
  if (DataReaded) {
    //Serial.print(ReadedData);
    //delay(10);
    for (byte i = 0; i < reclen; i++) {
      Serial.write((char)rec[i]);
    }
    DataReaded = false;
    //ReadedData = "";
    //reclen = 0;
    //delay(2);
  }
}

//Чтение с Serial для отладки
void getSerial() {
  //byte sz = 0;
  PORTB |= _BV(1);
  outreclen = 0;
  outrecptr = 0;
  //int mx = Serial.available();
  //if (mx > 255) mx = 255;
  while ((Serial.available() > 0) && (outreclen < 255)) {
    //for (int i = 0; i < mx; i++) {
    //char rd = Serial.read();
    //OutData = OutData + rd;
    //sz++;
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
    /*for (int i = 0; i < OutData.length(); i++) {
      Sserial.write(OutData.charAt(i));
      }*/
    for (int i = 0; i < outreclen; i++) {
      Sserial.write((char)outrec[i]);
    }
    //Sserial.print(OutData);
    DataNeedOut = false;
    outreclen = 0;

    //OutData = "";
    //delay(2);
  }
  locked = false;
}








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
  Sserial.begin(2400);
  Serial.begin(9600);
  delay(500);
}

void loop() {
  readData();
  //printData();
  //getSerial();
  //outData();

}
