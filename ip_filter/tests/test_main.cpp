#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ip_filters.h"

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
   // можно загрузить результат в вектор и сравнить 
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
