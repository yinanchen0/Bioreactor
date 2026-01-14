# TB Vaccine Bioreactor Control System

This project focuses on the design and implementation of a bioreactor control system for
tuberculosis (TB) vaccine production. TB is a serious infectious disease caused by
*Mycobacterium tuberculosis* and remains a major global health challenge, particularly in
low-income countries such as Uganda.

The system is intended to support yeast cultivation used in vaccine manufacturing by
precisely controlling and monitoring key bioreactor parameters.

## Project Objectives
- Maintain stable environmental conditions required for yeast growth
- Enable real-time monitoring and data logging
- Provide remote control via a web-based interface

## System Requirements
- **Temperature:** 25–35 °C (±0.5 °C)
- **pH level:** 3–7 (±0.2)
- **Stirring speed:** 500–1500 RPM (±20 RPM)

## System Architecture
The bioreactor control system is composed of four main subsystems:

1. **Heating Subsystem**  
   Thermistor and heating element for temperature regulation

2. **pH Control Subsystem**  
   pH probe with acid and alkaline pumps for pH adjustment

3. **Stirring Subsystem**  
   Motor, propeller, and phototransistor for speed feedback

4. **Connection and Control Subsystem**  
   Arduino Uno and ESP32 with a web server for monitoring, control, and data logging

   ![Bioreactor System Diagram](bioreactor.png)


## Overview
Sensor data from the heating, pH, and stirring subsystems are processed by the
connection/control subsystem. Based on this data, the system automatically adjusts
actuators to maintain target setpoints and logs all measurements with timestamps.
Remote access via a web interface enables real-time supervision and control of the
bioreactor.

This project was developed as part of a multidisciplinary engineering effort to support
TB vaccine production in resource-limited settings.
