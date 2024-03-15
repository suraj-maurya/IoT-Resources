# Universal Asynchronous Receiver-Transmitter (UART)

Universal Asynchronous Receiver-Transmitter (UART), a serial communication protocol that can be used to send data between an Arduino board and other devices

> [!NOTE]
> It's among the earliest serial protocols and even though it has in many places been replaced by SPI and I2C it's still widely used for lower-speed and lower-throughput applications because it is very simple, low-cost and easy to implement.

Communication via UART is enabled by the Serial class, which has a number of methods available, including reading & writing data.

## Serial Class

With the Serial class, you can send / receive data to and from your computer over USB, or to a device connected via the Arduino's RX/TX pins.

> [!NOTE]
> The GIGA R1 WiFi, Mega 2560 and Due boards have 3 serial port.

### The Serial class have several methods with some of the essentials being:

- begin() - begins serial communication, with a specified baud rate (many examples use either 9600 or 115200).
- print() - prints the content to the Serial Monitor.
- println() - prints the content to the Serial Monitor, and adds a new line.
- available() - checks if serial data is available (if you send a command from the Serial Monitor).
- read() - reads data from the serial port.
- write() - writes data to the serial port.

> [!NOTE]
> The Serial class is supported on all Arduino boards.

## [Arduino UART Pins](https://docs.arduino.cc/learn/communication/uart/#serial-class)

## Technical Specifications

### How UART Works
 UART operates by transmitting data as a series of bits, including a start bit, data bits, an optional parity bit, and stop bit(s). Unlike parallel communication, where multiple bits are transmitted simultaneously, UART sends data serially, one bit at a time. As the name reveals the protocol operates asynchronous which means that it doesn't rely on a shared clock signal. Instead, it uses predefined baud rates to determine the timing of data bits.

![Working Daigram](https://docs.arduino.cc/static/d3a2c9ad97f1b7479b997e8d89426aaf/a6d36/parallelSerial.png)