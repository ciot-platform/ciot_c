# Changelog

All notable changes to the CIoT C Library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- `ciot_mqtt_client_set_process_all_topics` function to allow CIOT to handle MQTT messages from all topics
- `CIOT_LOG` macro for logging `ciot_log_data_t` messages

### Fixed
- Enable MBus client and server features in custom configuration

## [0.1.0] - 2025-10-20

### Added

#### Core Library
- Initial release of CIoT C library
- Core CIOT interface and management system
- Error handling and logging infrastructure
- Serialization support using Protocol Buffers
- Event management system
- CRC utilities

#### Network Interfaces
- **BLE (Bluetooth Low Energy)**: Advertisement and scanning support
- **WiFi**: Station and AP modes
- **Ethernet**: Network connectivity support
- **TCP**: TCP socket communication
- **MQTT Client**: MQTT protocol support with publish/subscribe
- **HTTP Client**: HTTP client implementation
- **HTTP Server**: HTTP server implementation
- **NTP**: Network Time Protocol client

#### Communication Protocols
- **MBus Client**: Modbus client implementation
- **MBus Server**: Modbus server implementation
- **UART**: Serial communication interface
- **USB**: USB communication support

#### System Features
- **System (SYS)**: System information and control
- **GPIO**: General Purpose I/O control
- **PWM**: Pulse Width Modulation support
- **Timer**: Timer functionality
- **OTA (Over-The-Air)**: Firmware update support
- **DFU (Device Firmware Update)**: NRF DFU support
- **Storage**: Multiple storage backends (NVS, SPIFFS, FAT, Partition)

#### Platform Support
- ESP32 platform support
- ESP8266 platform support
- Nordic nRF platform support
- Linux platform support
- Windows platform support
- Mongoose OS integration

#### Build & Development
- CMake build system integration
- Common build configurations
- GitHub Actions CI/CD pipeline
- Doxygen documentation generation
- Unity test framework integration

#### Examples
- CLI (Command Line Interface) example
- CIOTA example application
- Device example with custom configuration
- ESP32 platform example
- ESP8266 platform example
- MBus client example
- MBus server example
- Nordic nRF platform example

#### Configuration
- Flexible configuration system with custom config support
- Feature flags for enabling/disabling components:
  - BLE Advertisement/Scanning
  - Ethernet
  - GPIO
  - HTTP Client/Server
  - MBus Client/Server
  - MQTT Client
  - NTP
  - OTA
  - PWM
  - System
  - TCP
  - Timer
  - UART
  - USB
  - WiFi
  - Storage
  - Serialization
  - Cryptography
  - DFU

#### Dependencies
- Protocol Buffers (nanopb) for message serialization
- mbedTLS for cryptography support
- nanomodbus for Modbus protocol
- Unity test framework for unit testing

### Documentation
- Complete Doxygen API documentation
- Configuration guide
- Example applications with documentation

[Unreleased]: https://github.com/ciot-platform/ciot_c/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/ciot-platform/ciot_c/releases/tag/v0.1.0
