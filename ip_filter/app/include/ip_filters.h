#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <fstream>

// результат чтения IP-адреса
enum class ReadResult {
    Success,
    Failure,
    EndOfStream
};

// Класс для хранения IP-адреса
class IPAddress {
private:
    std::array<uint8_t, 4> octets = {0, 0, 0, 0};
    static bool isValidOctet(int value) {
        return value >= 0 && value <= 255;
    }
public:
    IPAddress() = default;
    IPAddress(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) : octets{o1, o2, o3, o4} {}

    friend std::ostream &operator<<(std::ostream &os, const IPAddress &ip) {
        for (int i : {0,1,2,3}) {
            if (i > 0) os << ".";
            os << static_cast<int>(ip.octets[i]);
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &is, IPAddress &ip) {
        int o1, o2, o3, o4;
        char sep1, sep2, sep3;
        if ((is >> o1 >> sep1 >> o2 >> sep2 >> o3 >> sep3 >> o4) &&
            sep1 == '.' && sep2 == '.' && sep3 == '.' &&
            isValidOctet(o1) && isValidOctet(o2) && isValidOctet(o3) && isValidOctet(o4)) {
            ip.octets = {static_cast<uint8_t>(o1), static_cast<uint8_t>(o2), static_cast<uint8_t>(o3), static_cast<uint8_t>(o4)};
        } else {
            is.setstate(std::ios::failbit); 
        }
        return is;
    }

    bool operator<(const IPAddress &rhs) const { return octets < rhs.octets; }
    uint8_t operator[](size_t index) const { return octets[index]; }
    bool operator==(const IPAddress &rhs) const { return octets == rhs.octets; }
};

// Интерфейс потока, для тестирования с помощью gmock
class IStream {
public:
    virtual ~IStream() = default;
    virtual ReadResult read(IPAddress &ip) = 0;
};

// Стандартный поток ввода/вывода
class StandardIOStream : public IStream {
public:
    ReadResult read(IPAddress& ip) override {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return std::cin.eof() ? ReadResult::EndOfStream : ReadResult::Failure;
        }

        const auto tabPos = line.find('\t');
        std::string ipText = (tabPos != std::string::npos) ? line.substr(0, tabPos) : line;

        std::istringstream iss(ipText);
        return (iss >> ip) ? ReadResult::Success : ReadResult::Failure;
    }
};

// Чтение IP-адресов из файла
class FileStream : public IStream {
private:
    std::ifstream file;

public:
    FileStream(const std::string &filePath) {
        file.open(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file.");
        }
    }

    ReadResult read(IPAddress& ip) override {
        std::string line;
        
        // Чтение строки из файла
        if (!std::getline(file, line)) {
            return file.eof() ? ReadResult::EndOfStream : ReadResult::Failure;
        }

        const auto tabPos = line.find('\t');
        std::string ipText = (tabPos != std::string::npos) ? line.substr(0, tabPos) : line;

        std::istringstream iss(ipText);
        return (iss >> ip) ? ReadResult::Success : ReadResult::Failure;
    }
};

// Функция загрузки IP-адресов в вектор
void GetIPAddresses(std::vector<IPAddress> &ips, IStream &io);

// Функция сортировки IP адресов в обратном порядке
std::vector<IPAddress> SortIPAddressesRevers(const std::vector<IPAddress> &ips);

// Фильтрация по заданному октету
std::vector<IPAddress> FilterIPAddresses(const std::vector<IPAddress> &ips, uint8_t firstOctet, uint8_t secondOctet = 0);

// Фильтрация по любому октету
std::vector<IPAddress> FilterIPAddressesAnyOctet(const std::vector<IPAddress> &ips, uint8_t octet);

// Функция вывода IP-адресов
void PrintIPAddresses(const std::vector<IPAddress> &ips);

