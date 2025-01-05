#include <gtest/gtest.h>
#include <sstream>
#include "../app/include/print_ip.hpp"
#include "../app/include/cprint_ip.hpp"
#include "../app/include/concept_print_ip.hpp"

class CoutRedirector
{
public:
    CoutRedirector() { originalBuffer = std::cout.rdbuf(buffer.rdbuf()); }
    ~CoutRedirector() { std::cout.rdbuf(originalBuffer); }
    std::string getOutput() const { return buffer.str(); }

private:
    std::ostringstream buffer;
    std::streambuf *originalBuffer;
};

TEST(PrintIpTests, IntegralType)
{
    {
        CoutRedirector redirect;
        print_ip(2130706433); // 127.0.0.1 in uint32_t
        EXPECT_EQ(redirect.getOutput(), "127.0.0.1\n");
    }

    {
        CoutRedirector redirect;
        print_ip(static_cast<unsigned char>(255)); // 255
        EXPECT_EQ(redirect.getOutput(), "255\n");
    }
}

TEST(PrintIpTests, StringType)
{
    {
        CoutRedirector redirect;
        print_ip(std::string("192,168,1,1"));
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }
}

TEST(PrintIpTests, ContainerType)
{
    {
        CoutRedirector redirect;
        print_ip(std::vector<int>{192, 168, 1, 1});
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }

    {
        CoutRedirector redirect;
        print_ip(std::list<short>{10, 0, 0, 1});
        EXPECT_EQ(redirect.getOutput(), "10.0.0.1\n");
    }
}

TEST(PrintIpTests, TupleType)
{
    {
        CoutRedirector redirect;
        print_ip(std::make_tuple(192, 168, 1, 1));
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }

    // print_ip(std::make_tuple(192, "168", 1, 1));
}

// Тест для целочисленных типов
TEST(IpPrinterTests, PrintIntegralType)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(uint32_t(2130706433)); // 127.0.0.1
        EXPECT_EQ(redirect.getOutput(), "127.0.0.1\n");
    }
}

// Тест для строкового типа
TEST(IpPrinterTests, PrintStringType)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::string("192, 168, 1, 1")); // 192.168.1.1
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }
}

// Тест для std::vector<int>
TEST(IpPrinterTests, PrintVectorInt)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::vector<int>{192, 168, 1, 1}); // 192.168.1.1
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }
}

// Тест для std::list<short>
TEST(IpPrinterTests, PrintListShort)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::list<short>{10, 0, 0, 1}); // 10.0.0.1
        EXPECT_EQ(redirect.getOutput(), "10.0.0.1\n");
    }
}

// Тест для std::tuple с одинаковыми типами
TEST(IpPrinterTests, PrintTupleSameType)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::make_tuple(192, 168, 1, 1)); // 192.168.1.1
        EXPECT_EQ(redirect.getOutput(), "192.168.1.1\n");
    }
}

// Тест для неподдерживаемого типа
TEST(IpPrinterTests, UnsupportedType)
{
    IpPrinter printer;

    // Проверяем, что неподдерживаемый тип
    // printer.print(std::make_tuple(192, "168", 1, 1));
}

// Тест для строки с пробелами
TEST(IpPrinterTests, PrintStringWithSpaces)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::string("   10  ,  0  ,  0  ,  1  ")); // 10.0.0.1
        EXPECT_EQ(redirect.getOutput(), "10.0.0.1\n");
    }
}

// Тест для пустого контейнера
TEST(IpPrinterTests, PrintEmptyContainer)
{
    {
        IpPrinter printer;
        CoutRedirector redirect;

        printer.print(std::vector<int>{}); // Пустой вектор
        EXPECT_EQ(redirect.getOutput(), "\n");

        CoutRedirector redirect2;
        printer.print(std::list<short>{}); // Пустой список
        EXPECT_EQ(redirect2.getOutput(), "\n");
    }
}

TEST(IpPrinterCptTests, IntegralType)
{
    {
        CoutRedirector redirect;
        IpPrinterCpt::print(int8_t{-1}); // 255
        EXPECT_EQ(redirect.getOutput(), "255\n");
    }

    {
        CoutRedirector redirect;
        IpPrinterCpt::print(int16_t{0}); // 0.0
        EXPECT_EQ(redirect.getOutput(), "0.0\n");
    }

    {
        CoutRedirector redirect;
        IpPrinterCpt::print(int32_t{2130706433}); // 127.0.0.1
        EXPECT_EQ(redirect.getOutput(), "127.0.0.1\n");
    }

    {
        CoutRedirector redirect;
        IpPrinterCpt::print(int64_t{8875824491850138409}); // 123.45.67.89.101.112.131.41
        EXPECT_EQ(redirect.getOutput(), "123.45.67.89.101.112.131.41\n");
    }
}

TEST(IpPrinterCptTests, StringType)
{
    CoutRedirector redirect;
    IpPrinterCpt::print(std::string{"helo, world"});
    EXPECT_EQ(redirect.getOutput(), "helo, world\n");
}

TEST(IpPrinterCptTests, ContainerType)
{
    {
        CoutRedirector redirect;
        IpPrinterCpt::print(std::vector<int>{100, 200, 300, 400}); // 100.200.300.400
        EXPECT_EQ(redirect.getOutput(), "100.200.300.400\n");
    }

    {
        CoutRedirector redirect;
        IpPrinterCpt::print(std::list<short>{400, 300, 200, 100}); // 400.300.200.100
        EXPECT_EQ(redirect.getOutput(), "400.300.200.100\n");
    }
}

TEST(IpPrinterCptTests, TupleType)
{
    {
        CoutRedirector redirect;
        IpPrinterCpt::print(std::make_tuple(123, 456, 789, 0)); // 123.456.789.0
        EXPECT_EQ(redirect.getOutput(), "123.456.789.0\n");
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}