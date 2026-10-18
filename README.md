# ⚡ Underground Cable Fault Detection System

## 📌 Project Overview

The Underground Cable Fault Detection System is an ESP32-based embedded system designed to detect and identify faults in underground electrical cables.

Underground cable faults can occur due to insulation failure, short circuits, or open circuits. Identifying the fault location manually can be time-consuming.

This project monitors voltage levels at different points along a cable and uses an ESP32 microcontroller to detect faults. A web-based dashboard displays the fault status and estimated fault location.

## 🎯 Objectives

- Detect open-circuit and short-circuit faults in underground cables.
- Identify the approximate fault location using multiple cable monitoring points.
- Monitor voltage levels using an ESP32 microcontroller.
- Display fault information through a web-based dashboard.
- Provide visual and audible alerts using LEDs and a buzzer.

## 🛠️ Hardware Requirements

- ESP32 Development Board
- Voltage Sensing Circuit
- Underground Cable Fault Simulation Setup
- RGB LED
- Buzzer
- Connecting Wires
- Power Supply

> **Note:** The components listed above represent the project's intended hardware. Refer to the actual circuit and hardware setup for the final component specifications.

## 💻 Software Requirements

- Arduino IDE
- ESP32 Board Support Package
- Embedded C/C++ Programming
- HTML, CSS and JavaScript for the web interface

## ⚙️ Working Principle

1. The voltage sensing circuit monitors voltage at different points along the cable.
2. The ESP32 reads the voltage values from the monitoring points.
3. The measured values are compared with predefined conditions.
4. The system identifies possible open-circuit or short-circuit faults.
5. The approximate fault location is determined based on the affected monitoring point.
6. The web dashboard displays the system status and fault information.
7. LEDs and a buzzer provide visual and audible alerts when a fault is detected.

## 🌐 Web Dashboard

The web interface provides a convenient way to monitor the cable fault detection system.

Dashboard features include:

- Live voltage monitoring
- Fault status indication
- Approximate fault location display
- Visual fault alerts
- ESP32-based web server interface

*The exact dashboard features depend on the implemented firmware.*

## 🔄 System Workflow

```text
Underground Cable
       |
       v
Voltage Sensing Circuit
       |
       v
ESP32 Microcontroller
       |
       v
Voltage Analysis
       |
       v
Fault Detection
       |
       +-------------------+
       |                   |
       v                   v
 Web Dashboard       LED and Buzzer
       |
       v
Fault Status and
Approximate Location
