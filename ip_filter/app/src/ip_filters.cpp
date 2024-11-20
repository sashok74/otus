#include <iostream>
#include <array>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <fstream>
#include "ip_filters.h"
    
// загрузка адресов в вектор
void GetIPAddresses(std::vector<IPAddress> &ips, IStream& io)  {
    IPAddress ip;
    bool EndOfStream = false;
    while (EndOfStream == false) {
        auto result = io.read(ip);
        if (result == ReadResult::EndOfStream) {
            EndOfStream = true;
            continue;
        }
        else if (result == ReadResult::Failure) {
           // std::cout << "Invalid IP address entered. Try again." << std::endl;
            continue;
        }
        ips.push_back(ip);
       // std::cout << "IP address entered: " << ip << " count of ip:" <<   ips.size() << std::endl;
    }
}

//вернуть сортированый вектор
std::vector<IPAddress> SortIPAddressesRevers(const std::vector<IPAddress>& ips) {
    std::vector<IPAddress> sorted = ips;
    std::sort(sorted.begin(), sorted.end(), [](const IPAddress& lhs, const IPAddress& rhs) {
        return rhs < lhs;
    });
    return sorted;
}


std::vector<IPAddress> FilterIPAddresses(const std::vector<IPAddress>& ips, uint8_t firstOctet, uint8_t secondOctet) {
    std::vector<IPAddress> filtered;
    for (const auto& ip : ips) {
        if (ip[0] == firstOctet && (secondOctet == 0 || ip[1] == secondOctet)) {
            filtered.push_back(ip);
        }
    }
    return filtered;
}

std::vector<IPAddress> FilterIPAddressesAnyOctet(const std::vector<IPAddress>& ips, uint8_t octet) {
    std::vector<IPAddress> filtered;
    for (const auto& ip : ips) {
        if (ip[0] == octet || ip[1] == octet || ip[2] == octet || ip[3] == octet) {
            filtered.push_back(ip);
        }
    }
    return filtered;
}

void PrintIPAddresses(const std::vector<IPAddress>& ips) {
    for (const auto& ip : ips) {
        std::cout << ip << std::endl;
    }
}
