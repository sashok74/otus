#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ip_filters.h"
#include <queue>

// Мок-объект для IStream, сделать чтение из файла.
class MockIStream : public IStream {
public:
    MOCK_METHOD(ReadResult, read, (IPAddress& ip), (override));
};


void PrintIPAddresses(const std::vector<IPAddress> &ips, std::ostream &output) {
    for (const auto &ip : ips) {
        output << ip << std::endl;
    }
}

void PrintIPAddresses(const std::vector<IPAddress> &ips, std::vector<IPAddress> &result) {
    for (const auto &ip : ips) {
        result.push_back(ip);
    }
}

// Утилита для загрузки IP-адресов из файлов
std::queue<IPAddress> LoadIPsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open test file: " + filePath);
    }

    std::queue<IPAddress> ipQueue;
    std::string line;

    while (std::getline(file, line)) {
        const auto tabPos = line.find('\t');
        std::string ipText = (tabPos != std::string::npos) ? line.substr(0, tabPos) : line;

        IPAddress ip;
        std::istringstream iss(ipText);
        if (iss >> ip) {
            ipQueue.push(ip);
        } else {
            throw std::runtime_error("Invalid IP format in test file.");
        }
    }

    return ipQueue;
}

// для примера с gmock
TEST(IPUtilsTest, MockIStreamFileInputTest) {
    auto testIPs = LoadIPsFromFile("ip_filter.tsv");
    MockIStream mockStream;

    // Настраиваем мок для возврата IP-адресов из очереди
    EXPECT_CALL(mockStream, read(::testing::_))
        .WillRepeatedly([&testIPs](IPAddress& ip) -> ReadResult {
            if (testIPs.empty()) {
                return ReadResult::EndOfStream;
            }
            ip = testIPs.front();
            testIPs.pop();
            return ReadResult::Success;
        });

    // Проверяем GetIPAddresses
    std::vector<IPAddress> ips;
    std::vector<IPAddress> res;
    GetIPAddresses(ips, mockStream);

    // Проверяем, что все IP-адреса загружены
    EXPECT_FALSE(ips.empty());

    // Проверяем сортировку
    auto sorted = SortIPAddressesRevers(ips);
    PrintIPAddresses(sorted, res);

    auto filtered = FilterIPAddresses(sorted, 1);
    PrintIPAddresses(filtered, res);

    filtered = FilterIPAddresses(sorted, 46, 70);
    PrintIPAddresses(filtered, res);

    filtered = FilterIPAddressesAnyOctet(sorted, 46);
    PrintIPAddresses(filtered, res);

    // Загружаем ожидаемые результаты
    auto expectedIPs = LoadIPsFromFile("ip_filter_result.tsv");
    std::vector<IPAddress> expectedIPsVec;
    while (!expectedIPs.empty()) {
        expectedIPsVec.push_back(expectedIPs.front());
        expectedIPs.pop();
    }

    // Сравниваем с ожидаемыми результатами
    EXPECT_EQ(res, expectedIPsVec);
}

TEST(FilterIPAddressesTest, FilterByOctet) {
    // Подготовка данных
    std::vector<IPAddress> ips = {
        IPAddress({1, 2, 3, 4}),
        IPAddress({46, 70, 0, 1}),
        IPAddress({1, 2, 3, 7}),        
        IPAddress({10, 20, 30, 40})
    };

    auto filtered = FilterIPAddresses(ips, 1);  
    ASSERT_EQ(filtered.size(), 2);              
    EXPECT_EQ(filtered[0], IPAddress({1, 2, 3, 4})); 
    EXPECT_EQ(filtered[1], IPAddress({1, 2, 3, 7}));  
}

TEST(IntegrationalTest, FullDataTest) {
    //сравним итоговый поток с тестовым
    std::ifstream inputFile("ip_filter.tsv");
    std::ifstream expectedOutputFile("ip_filter_result.tsv");
    std::vector<IPAddress> ips;
    FileStream fileStream("ip_filter.tsv");

    // Читаем IP-адреса
    GetIPAddresses(ips, fileStream);

    // Выполняем сортировку
    auto sorted = SortIPAddressesRevers(ips);

    // вывод данных в строковый поток
    std::stringstream output;
    PrintIPAddresses(sorted, output);

    auto filtered = FilterIPAddresses(sorted, 1);
    PrintIPAddresses(filtered, output);

    filtered = FilterIPAddresses(sorted, 46, 70);
    PrintIPAddresses(filtered, output);

    filtered = FilterIPAddressesAnyOctet(sorted, 46);
    PrintIPAddresses(filtered, output);

    // Сравниваем полученный результат с ожидаемым
    std::stringstream expectedOutputStream;
    expectedOutputStream << expectedOutputFile.rdbuf();
    
    EXPECT_EQ(output.str(), expectedOutputStream.str());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
