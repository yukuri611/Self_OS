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

    union CapabilityHeader {
        uint32_t data;
        struct {
            uint32_t cap_id : 8;
            uint32_t next_ptr : 8;
            uint32_t cap : 16;
        } __attribute__((packed)) bits;
    } __attribute__((packed));

    const uint8_t kCapabilityMSI = 0x05;
    const uint8_t kCapabilityMSIX = 0x11;

    CapabilityHeader ReadCapabilityHeader(const Device& dev, uint8_t addr);
    struct MSICapability {
        union {
            uint32_t data;
            struct {
                uint32_t cap_id : 8;
                uint32_t next_ptr : 8;
                uint32_t msi_enable : 1;
                uint32_t multi_msg_capable : 3;
                uint32_t multi_msg_enable : 3;
                uint32_t addr_64_capable : 1;
                uint32_t per_vector_mask_capable : 1;
                uint32_t : 7;
            } __attribute__((packed)) bits;
        } __attribute__((packed)) header ;
    
        uint32_t msg_addr;
        uint32_t msg_upper_addr;
        uint32_t msg_data;
        uint32_t mask_bits;
        uint32_t pending_bits;
    } __attribute__((packed));

    /** @brief MSI または MSI-X 割り込みを設定する
    *
    * @param dev  設定対象の PCI デバイス
    * @param msg_addr  割り込み発生時にメッセージを書き込む先のアドレス
    * @param msg_data  割り込み発生時に書き込むメッセージの値
    * @param num_vector_exponent  割り当てるベクタ数（2^n の n を指定）
    */
    Error ConfigureMSI(const Device& dev, uint32_t msg_addr, uint32_t msg_data,
                        unsigned int num_vector_exponent);

    enum class MSITriggerMode {
        kEdge = 0,
        kLevel = 1
    };

    enum class MSIDeliveryMode {
        kFixed          = 0b000,
        kLowestPriority = 0b001,
        kSMI            = 0b010,
        kNMI            = 0b100,
        kINIT           = 0b101,
        kExtINT         = 0b111,
    };

    Error ConfigureMSIFixedDestination(
        const Device& dev, uint8_t apic_id,
        MSITriggerMode trigger_mode, MSIDeliveryMode delivery_mode,
        uint8_t vector, unsigned int num_vector_exponent);
}