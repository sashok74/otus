#include "include/print_ip.hpp"
#include "include/cprint_ip.hpp"
#include "concept_print_ip.hpp"

int main()
{
    print_ip(std::string("helo, world"));
    print_ip( int8_t{-1} ); // 255
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100  
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0

    std::cout << "\nвариант2: if constexpr \n\n";

    IpPrinter pip;
    pip.print(std::string("helo, world"));
    pip.print( int8_t{-1} ); // 255
    pip.print( int16_t{0} ); // 0.0
    pip.print( int32_t{2130706433} ); // 127.0.0.1
    pip.print( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    pip.print( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    pip.print( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100  
    pip.print( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0

    std::cout << "\nвариат3: concept \n\n";

    IpPrinterCpt pipc;
    pipc.print(std::string("helo, world"));
    pipc.print( int8_t{-1} ); // 255
    pipc.print( int16_t{0} ); // 0.0
    pipc.print( int32_t{2130706433} ); // 127.0.0.1
    pipc.print( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    pipc.print( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    pipc.print( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100  
    pipc.print( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
   // pipc.print( std::make_tuple(123, 456, 789, "0") ); // 123.456.789.0

    return 0;
}