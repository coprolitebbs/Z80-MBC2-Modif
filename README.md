# Z80-MBC2-Modif
z80-MBC2 И модификации


# Файлы
* /out/ - Последний вариант, экспортные файлы Kicad платы Z80-MBC2. Перепутаны RX и TX с выхода микросхемы MAX232 на разъем RS232, требуется менять дорожки.

* Modified-RetroWiFiModem.7z - Прошивка для Wemos D1 Mini адаптированная для платы i2c-транслятора

* i2c-slave-translator.ino - Прошивка для Arduino Pro Mini 5V, используемая в качестве i2c-транслятора для модема

* S220718-R240620_IOS-Z80-MBC2.7z - Прошивка для Atmega-32A с текущей IOS и поддержкой WIFI-модема

* BIOS.7z - Исходники BIOS СЗ/Ь 3ю0 модифицированные для использования WIFI-модема

* BIOS_compiled_for_Z80-MBC2.7z - Скомпилированная для Z80-MBC2.0 операционная система CP/M 3.0 и лоадер к ней

* cpmtools_GUI.7z - Утилита CPMTools_GUI для операций с образами дисков

* /disks/ - Все диски CP/M 3.0 для Z80-MBC2

* /pterm/ - Терминал на Turbo Pascal 3.0 для работы с Wifi-модемом

Все AT-команды для работы с модемом доступны по ссылке на оригинальныю [прошивку](https://github.com/mecparts/RetroWiFiModem) Wifi-модема

# Схема подключения модема и транслятора:
![Схема подключения модема и транслятора](/img/I2c_modem.jpg)

# Ссылки
Оригинальная прошивка [Retro WiFi modem](https://github.com/mecparts/RetroWiFiModem)
