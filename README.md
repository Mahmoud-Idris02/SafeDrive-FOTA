# SafeDrive-FOTA

![Background Image](https://github.com/Mahmoud-Idris02/SafeDrive-FOTA/blob/main/FOTA.png?raw=true)
## Project Overview

In the modern era, vehicles have evolved from being primarily mechanical to complex software-driven machines, commonly referred to as Software Defined Vehicles (SDVs). With this transformation, cars now incorporate extensive lines of code—often exceeding 100 million lines. Managing and updating this software manually at the Original Equipment Manufacturer (OEM) level is no longer feasible. 

Our project addresses this challenge by implementing a Firmware Over-The-Air (FOTA) system. This system allows for the remote updating of firmware in electronic control units (ECUs) within vehicles, facilitating the delivery of new features, bug fixes, and performance improvements without the need for physical access to the vehicle. 

## Problem Statement

Smart Cars require regular updates to enhance features and address bugs. Manually updating each vehicle at the OEM level is impractical and can lead to customer dissatisfaction, especially when critical bugs need urgent resolution. Our FOTA system aims to automate and streamline this process, making it more efficient and responsive.

Additionally, with the increased connectivity of modern vehicles, there is a heightened risk of cyberattacks. Ensuring robust security measures is essential to protect automotive systems from unauthorized access, data breaches, and potential physical harm.

## Features

- **Automated Firmware Updates**: Seamlessly deliver updates to vehicle ECUs without requiring physical intervention.
- **Bug Fixes and Performance Improvements**: Quickly address and resolve software issues to enhance vehicle performance.
- **Enhanced Security**: Implement advanced cryptographic techniques to safeguard against cyber threats and unauthorized access.
- **Obstacle Avoidance and Detection System**: Developed to test the functionality of the FOTA system, this feature ensures that the system can manage and update firmware while the vehicle is actively avoiding obstacles.
- **Vehicle Dashboard**: Implemented as a user interface for the system, providing a comprehensive and intuitive way to interact with and manage the FOTA system and its features.
## System Architecture

The FOTA system architecture consists of the following components:

1. **FOTA Management Server:** This server controls the distribution of firmware updates to smart cars. It verifies the firmware and securely sends updates to the vehicle.
   
2. **Gateway:** The gateway acts as an intermediary between the vehicle and the FOTA management server. It manages the communication between the vehicle control unit (VCU) and the management server, ensuring data is securely transmitted and received. The gateway is responsible for receiving the firmware updates and forwarding them to the vehicle.

3. **Vehicle Control Unit (VCU):** The VCU receives the firmware update from the gateway, verifies its integrity, and installs the new firmware onto the vehicle's systems.

4. **Obstacle Avoidance System:** A sensor-based system within the vehicle designed to detect obstacles and avoid collisions during testing of the firmware updates.

5. **Cryptographic Modules:** Encryption and decryption modules ensure that all communications between the server, gateway, and vehicle are secure and tamper-proof.
   
## Technologies Used

- **Programming Languages**: [Embedded C, C++, Python]
- **Frameworks and Libraries**: [Django]
- **Security**: [Cyphrer encryption "AES"]
- **Architecture**: [ARM]

