# arduino-nuclei-sdk

## Introduction
This is an Arduino package for RV-STAR board based on Nuclei SDK. You can use it both in Arduino IDE and by make commands.

## Usage
### Use in Arduino IDE
1. You need firstly download and setup Arduino IDE (only support Windows Version) from [Arduino Home](www.arduino.cc).
2. Add the board JSON index file URL: https://young-1253969066.cos.ap-nanjing.myqcloud.com/package_rvstar_index.json to your Preferences in File Option, open Board Manager in Tool Option and setup package which named **RVSTAR(GD32V) by Young**.
3. Create a new Sketch and write your Arduino Style codes. You can get reference from [Arduino Reference](https://www.arduino.cc/reference/en/).
4. Connect the board, select the GD32VF103 RV-STAR and RV-Debugger in Tool Options, then you can build and upload your codes.

###  Use by Make command
1. You need firstly download and setup Make command tools and  Nuclei SDK tools according to the [Nuclei-SDK’s documentation](https://doc.nucleisys.com/nuclei_sdk/).
2. Write your main codes in the main.c or main.cpp file in this directory and of course you can also create other *.c *.cpp and *.h code modules then include them.
3. Connect the board, open this directory in terminal, use `make clean` to clean the environment and then use `make upload` to compile the codes and upload to the board.

## Support Functions
These are the functions that this package supports. You can find their usage from [Arduino Reference](https://www.arduino.cc/reference/en/).

### Digital I/O
- digitalRead()
- digitalWrite()
- pinMode()

### Analog I/O
- analogRead()
- analogWrite()

### Advanced I/O
- tone()
- noTone()
- pulseIn()
- pulseLong()

### Time
- delay()
- delayMicroSeconds()
- micros()
- millis()

### Communication
- Serial
  - Serial1 for the USB-C UART port
  - Serial2 for the pins C10 and C11 UART port
- [Wire](https://www.arduino.cc/en/Reference/Wire)
- [SPI](https://www.arduino.cc/en/Reference/SPI)

### External Interrupts
- attachInterrupt()
- detachInterrupt()

### Interrupts
- interrupts()
- noInterrupts()

### Math
- abs()
- constrain() 
- map() 
- max() 
- min() 
- pow() 
- sq() 
- sqrt()

### Trigonometry
- cos()
- sin()
- tan()

### Random Numbers
- random() 
- randomSeed() 

### Bits and Bytes
- bit() 
- bitClear() 
- bitRead() 
- bitSet() 
- bitWrite() 
- highByte() 
- lowByte() 

### Characters
- isAlpha()
- isAlphaNumeric()
- isAscii()
- isControl() 
- isDigit() 
- isGraph() 
- isHexadecimalDigit() 
- isLowerCase() 
- isPrintable() 
- isPunct() 
- isSpace() 
- isUpperCase() 
- isWhitespace() 