#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {
    const uint16_t kConfigAddress = 0xcf8; // IO port address for CONFIG_ADDRESS register
    const uint16_t kConfigData = 0xcfc; // IO port address for CONFIG_DATA register

    void WriteAddress(uint32_t address); //write integer specified to CONFIG_ADDRESS
    void WriteData(uint32_t address); //write integer specified to CONFIG_DATA
    uint32_t ReadData(); //read integer from CONFIG_DATA
    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function); //read vendor id register
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function); //read device id register
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function); //read header type register
    uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function); //read class code register
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function); //read bus numbers register
    bool IsSingleFunctionDevice(uint8_t header_type); //check if the device is a single function device

    struct Device {
        uint8_t bus;
        uint8_t device;
        uint8_t function;;
        uint8_t header_type;
    };

    inline std::array<Device, 32> devices; //array of devices found
    inline int num_device; //number of devices found

    Error ScanAllBus(); //scan all PCI devices and store them in devices array. change num_device

}