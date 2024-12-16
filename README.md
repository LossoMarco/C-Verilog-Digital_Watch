# C-Verilog-Digital_Watch

## Introduction
The project aims to develop a digital clock displaying hours, minutes, and seconds, adjustable via a rotary encoder. It utilizes various clock frequencies to ensure high precision in timekeeping and includes optional features to enhance user experience, such as a choice between 12-hour and 24-hour formats and a stopwatch function that detects "short" and "long" clicks. The report details the development process, challenges faced, and adopted solutions.

## Hardware Configuration
The project started with a pre-configured base in the Platform Designer of Quartus, which provided modules for timers at different frequencies, interfacing with external GPIO for the rotary encoder, and modules for controlling 7-segment displays. The initial hardware configuration included several connected modules:

- **Display Control Module:** Manages six 7-segment displays.
- **Timer Modules:** Three timer modules operate at 1Hz, 2Hz, and 100Hz, respectively.
- **Rotary Encoder Module:** Interfaces with the rotary encoder for input.
- **Key Encoder Module:** Handles the button associated with the rotary encoder.

These timer modules are basic counters that write their value to a memory register and are utilized in the C program for precise timekeeping.

## Modifications to the Verilog HDL Code
Following the hardware configuration, modifications were made to the Verilog HDL code in the top-level entity to connect the ARM processor with external peripherals. This included linking the 7-segment displays and the rotary encoder to the processor, allowing for real-time data control and visualization.

## 7-Segment Display Control
A Verilog module was used to control the 7-segment displays, with specific codes corresponding to different visual outputs, such as turning off all LEDs or displaying the letter "h." The modifications to the code allowed for effective management of the display outputs.

## C Code Implementation
The C code implementation was divided into several sections:

- **Memory Address Definitions:** Specific memory addresses were defined for various peripherals, including the encoder, display, timers, and buttons.
- **Clock Functionality:** A function was implemented to update the clock based on second, minute, and hour increments, with appropriate handling for both 12-hour and 24-hour formats.
- **Time Adjustment with Rotary Encoder:** Mechanisms were created to allow users to adjust the clock using the rotary encoder, with visual indicators for which value is being adjusted.

## Stopwatch Implementation
Alongside the clock, a stopwatch feature was integrated, allowing users to switch between clock and stopwatch displays using the rotary encoder. A short click starts or stops the stopwatch, while a long click resets it.

## Conclusion
The project successfully resulted in a fully functional digital clock with advanced features, including adjustable time settings, blinking display indicators during modifications, and dual time formats. The integration of a stopwatch function further enriches the user experience.

Utilizing a pre-configured hardware base in Platform Designer accelerated the development process, allowing for a focus on high-level functionalities and peripheral interfacing. The modifications to Verilog HDL and the implementation of the C program were crucial for the system's functionality and user interface.

Overall, the project exemplifies the effective combination of hardware and software to create a complex and functional embedded system, laying a strong foundation for future enhancements and expansions.
