#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {
    const uint16_t kConfigAddress = 0xcf8; // IO port address for CONFIG_ADDRESS register
    const uint16_t kConfigData = 0xcfc; // IO port address for CONFIG_DATA register
    struct ClassCode {
        uint8_t base, sub, interface;
    
        /** @brief ベースクラスが等しい場合に真を返す */
        bool Match(uint8_t b) { return b == base; }
        /** @brief ベースクラスとサブクラスが等しい場合に真を返す */
        bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub; }
        /** @brief ベース，サブ，インターフェースが等しい場合に真を返す */
        bool Match(uint8_t b, uint8_t s, uint8_t i) {
            return Match(b, s) && i == interface;
        }
    };
    struct Device {
        uint8_t bus, device, function, header_type;
        ClassCode class_code;
    };


    void WriteAddress(uint32_t address); //write integer specified to CONFIG_ADDRESS
    void WriteData(uint32_t address); //write integer specified to CONFIG_DATA
    uint32_t ReadData(); //read integer from CONFIG_DATA
    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function); //read vendor id register
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function); //read device id register
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function); //read header type register
    ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function); //read class code register
    inline uint16_t ReadVendorId(const Device& dev) {
        return ReadVendorId(dev.bus, dev.device, dev.function);
    }   
    uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr);
    void WriteConfReg(const Device& dev, uint8_t reg_addr, uint32_t value);
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function); //read bus numbers register
    bool IsSingleFunctionDevice(uint8_t header_type); //check if the device is a single function device

    
    inline std::array<Device, 32> devices; //array of devices found
    inline int num_device; //number of devices found

    Error ScanAllBus(); //scan all PCI devices and store them in devices array. change num_device
    
    constexpr uint8_t CalcBarAddress(unsigned int bar_index) {
        return 0x10 + 4 * bar_index;
    }

    WithError<uint64_t> ReadBar(Device& device, unsigned int bar_index);
}