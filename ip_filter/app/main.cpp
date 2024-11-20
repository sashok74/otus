#include "ip_filters.h"
    

int main(int argc, char *argv[]) {
    std::vector<IPAddress> ips;
    IStream *io;
    if (argc > 1) {
        io = new FileStream (argv[1]);
    } else {
        io = new StandardIOStream ();
    }

    //std::cout << "Enter IP addresses. To finish, press Ctrl+D (Ctrl+Z on Windows)." << std::endl;
    GetIPAddresses(ips, *io);

    //сортировка в обратном порядке
    auto sorted = SortIPAddressesRevers(ips);
    //вывод отсортированных адресов
    PrintIPAddresses(sorted);
    //1.x.x.x
    auto filtered = FilterIPAddresses(sorted, 1);
    PrintIPAddresses(filtered);
    //46.70.x.x
    filtered = FilterIPAddresses(sorted, 46, 70);
    PrintIPAddresses(filtered);
    //любой октет 46.
    filtered = FilterIPAddressesAnyOctet(sorted, 46);
    PrintIPAddresses(filtered);
    return 0;
}