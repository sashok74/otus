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
    std::array<uint8_t, 4> octets;

public:
    IPAddress() = default;
    IPAddress(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4) : octets{o1, o2, o3, o4} {}

    friend std::ostream &operator<<(std::ostream &os, const IPAddress &ip) {
        for (int i : {0,1,2,3}) {
            os << (i == 0 ? "" : ".") << int(ip.octets[i]);
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &is, IPAddress &ip) {
        int o1, o2, o3, o4;
        char sep1, sep2, sep3;
        if ((is >> o1 >> sep1 >> o2 >> sep2 >> o3 >> sep3 >> o4) &&
            sep1 == '.' && sep2 == '.' && sep3 == '.' &&
            o1 >= 0 && o1 <= 255 &&
            o2 >= 0 && o2 <= 255 &&
            o3 >= 0 && o3 <= 255 &&
            o4 >= 0 && o4 <= 255) {
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
    ReadResult read(IPAddress &ip) override {
        std::cin >> ip;
        if (std::cin.good()) {
            return ReadResult::Success;
        } else if (std::cin.eof()) {
            return ReadResult::EndOfStream;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return ReadResult::Failure;
        }
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

    ReadResult read(IPAddress &ip) override {
        std::string line;
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string text1;
            if (!std::getline(iss, text1, '\t'))
                return ReadResult::Failure;

            std::istringstream ip_stream(text1);
            ip_stream >> ip;

            if (ip_stream.fail())
                return ReadResult::Failure;

            return ReadResult::Success;
        } else if (file.eof()) {
            return ReadResult::EndOfStream;
        } else {
            return ReadResult::Failure;
        }
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

