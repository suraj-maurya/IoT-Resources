# Universal Asynchronous Receiver-Transmitter (UART)

Universal Asynchronous Receiver-Transmitter (UART), a serial communication protocol that can be used to send data between an Arduino board and other devices

> [!CAUTION]
> This is the short notes, extrated from the detailed documentation and tutorials. For details material, visit->[uartRefrence.md](https://github.com/suraj-maurya/IoT-Resources/blob/main/Protocols/Communication/UART/uartRefrence.md) 

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

> [!CAUTION]
> Consider that you need to connect a common ground between the devices to define the high and low signals for UART communication. Without a common ground, devices may not be able to correctly interpret transmitted data.

### Components 
The key components of UART include the transmitter, receiver, and baud rate. The transmitter collects data from a source, formats it into serial bits, and sends it via a TX (Transmit) pin. The receiver receives it via a RX (Receive) pin, processes incoming serial data and converts it into parallel data for the host system.

### Timing and Synchronization
Unlike synchronous serial communication protocols such as SPI and I2C, UART operates operates asynchronously, meaning it doesn't rely on a shared clock signal to coordinate data transmission.

#### Baud Rate
 The baud rate is specified in bits per second (bps) and represents the number of bits transmitted in one second. In UART, both the transmitting and receiving devices must agree on the same baud rate to ensure successful communication

 > [!NOTE]
 > A higher baud rate allows for faster data transmission, but it also demands a more precise timing synchronization between the sender and receiver. On the other hand, a lower baud rate may be suitable for applications where timing accuracy is less critical, but it results in slower data transfer.

 ### Flow Control in UART
UART Flow Control is a method for slow and fast devices to communicate with each other over UART without the risk of losing data. Consider the case where two units are communicating over UART. A transmitter T is sending a long stream of bytes to a receiver R. R is a slower device than T, and R cannot keep up. It needs to either do some processing on the data or empty some buffers before it can keep receiving data.

R needs to tell T to stop transmitting for a while. This is where flow control comes in. Flow control provides extra signaling to inform the transmitter that it should stop (pause) or start (resume) the transmission

## UART Messages

In UART communication, each data frame is encapsulated by start and stop bits. These bits serve a vital role in establishing the boundaries of data transmission and ensuring synchronization between the sender and receiver.

![UART FRAM FORMATE](https://docs.arduino.cc/static/ff1d2c3971a36f4dea095a4d44fe3ce0/a6d36/message.png)

## Serial USB Examples

### Basic Print Example
This example will send the string Hello World! from an Arduino to a computer, using the Serial.println() function. Data will be sent every one second.

```
void setup(){
  Serial.begin(9600); //initialize serial communication at a 9600 baud rate
}

void loop(){
  Serial.println("Hello world!");
  delay(1000);
}
```

### Read
To send data from a computer to an Arduino (from the Serial Monitor), we can make use of the Serial.available() and Serial.read() functions. First, we check if there's any data available, and if so, we read it and print it out.

This example will essentially print out whatever you enter in the Serial Monitor (because we send the data to the board, but we also print it back).

```
int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600); //initialize serial communication at a 9600 baud rate
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }
}
```
### Transmit / Receive Messages

```
String sendMessage;
String receivedMessage;

void setup() {
  Serial.begin(9600);    // Initialize the Serial monitor for debugging
  Serial1.begin(9600);   // Initialize Serial1 for sending data
}

void loop() {
  while (Serial1.available() > 0) {
    char receivedChar = Serial1.read();
    if (receivedChar == '\n') {
      Serial.println(receivedMessage);  // Print the received message in the Serial monitor
      receivedMessage = "";  // Reset the received message
    } else {
      receivedMessage += receivedChar;  // Append characters to the received message
    }
  }

  if (Serial.available() > 0) {
    char inputChar = Serial.read();
    if (inputChar == '\n') {
      Serial1.println(sendMessage);  // Send the message through Serial1 with a newline character
      sendMessage = "";  // Reset the message
    } else {
      sendMessage += inputChar;  // Append characters to the message
    }
  }
}
```


> [!NOTE]
> In programming, the newline character ('\n') is like pressing "Enter" key. It's a special character that tells the computer, "Move to the next line." In our case we know that a message is sent after pressing enter which equals the newline character ('n').

