# Z80-MBC2-Modif
z80-MBC2 И модификации


# Файлы
/out/ - Последний вариант, экспортные файлы Kicad платы Z80-MBC2. Перепутаны RX и TX с выхода микросхемы MAX232 на разъем RS232, требуется менять дорожки.

Modified-RetroWiFiModem.7z - Прошивка для Wemos D1 Mini адаптированная для платы i2c-транслятора

i2c-slave-translator.ino - Прошивка для Arduino Pro Mini 5V, используемая в качестве i2c-транслятора для модема

# Схема подключения модема и транслятора:
![Схема подключения модема и транслятора](/img/I2c_modem.jpg)

# Ссылки
Оригинальная прошивка [Retro WiFi modem](https://github.com/mecparts/RetroWiFiModem)
