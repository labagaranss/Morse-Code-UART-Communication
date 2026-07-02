# Morse Code UART Communication using AVR Microcontroller

## About the Project

This project was developed to understand how serial communication works at a low level without using the hardware UART available in AVR microcontrollers. Instead of relying on the built-in UART peripheral, I implemented a Software UART (Bit-Banging) to transmit and receive data.

To make the project more interesting, I used Morse code as the communication method. A push button is used to generate dots and dashes based on how long it is pressed. These Morse symbols are transmitted to another AVR microcontroller through Software UART. The receiver decodes the Morse sequence and displays the corresponding character on a 16×2 I²C LCD.

Building this project helped me understand UART timing, GPIO control, software delays, Morse code decoding, and LCD interfacing in a practical way.

---

## Why I Built This Project

The main purpose of this project was to learn how UART communication actually works internally instead of simply using the hardware UART module.

While working on this project, I wanted to explore:

- How serial data is transmitted bit by bit.
- How accurate timing is maintained in Software UART.
- How Morse code can be converted into readable text.
- How multiple embedded concepts can be integrated into a single project.

---

## Features

- Software UART implementation (Bit-Banging)
- Morse code encoding using a push button
- Morse code decoding on the receiver side
- Supports alphabets (A–Z)
- Supports numbers (0–9)
- Automatic character detection
- Automatic word spacing
- 16×2 LCD display using I²C
- Automatic LCD clear after inactivity

---

## Hardware Used

- AVR Microcontroller
- Push Button
- 16×2 LCD
- I²C LCD Module (PCF8574)
- Breadboard
- Jumper Wires
- 5V Power Supply

---

## Software Used

- Embedded C
- AVR-GCC
- Arduino Ide
- Visual Studio Code
- GitHub

---

## How It Works

### Transmitter

The transmitter continuously monitors the push button.

- A short button press is treated as a **dot (.)**.
- A long button press is treated as a **dash (-)**.

Each symbol is transmitted immediately through Software UART.

If there is no button activity for one second, the transmitter sends a special character (`#`) to indicate that the current Morse character has ended.

If there is no activity for two seconds, it sends (`/`) to indicate the end of a word.

---

### Receiver

The receiver continuously listens for incoming UART data.

Every received dot or dash is stored temporarily in a buffer.

When the receiver detects `#`, it compares the stored Morse sequence with the Morse lookup table and converts it into the corresponding alphabet or number.

When `/` is received, a space is displayed on the LCD.

If the receiver remains inactive for one minute, the LCD is cleared automatically.

---

## Project Structure

```
Morse-Code-UART-Communication
│
├── Images
├── Receiver
│   └── receiver.c
├── Transmitter
│   └── transmitter.c
├── README.md
└── LICENSE
```

---

## Concepts Used

This project helped me understand the following embedded concepts:

- Embedded C Programming
- GPIO Programming
- Software UART (Bit-Banging)
- UART Communication Protocol
- I²C Communication
- LCD Interfacing
- Timer-based Delays
- Morse Code Encoding
- Morse Code Decoding
- Software Timing

---

## Challenges Faced

One of the biggest challenges during development was maintaining accurate UART timing using software delays. Even a small timing mismatch could cause incorrect data reception.

Another challenge was detecting the difference between a dot and a dash while also identifying the end of characters and words based on button inactivity.

Debugging these timing-related issues gave me a much better understanding of embedded programming.

---

## Future Improvements

Some improvements that can be added in future versions are:

- Hardware UART implementation
- Interrupt-based button detection
- Bluetooth communication
- LoRa communication
- OLED display support
- Message history storage
- Wireless Morse code communication

---

## Learning Outcome

This project gave me practical experience with embedded system development. Instead of only studying communication protocols theoretically, I implemented them from scratch. It improved my understanding of UART communication, timing calculations, GPIO programming, and embedded C development.

---

## Author

**Labagaran**

Bachelor of Engineering (Electronics and Communication Engineering)

Interested in Embedded Systems, IoT and Firmware Development.

