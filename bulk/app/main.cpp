#include <array>
#include <cassert>
#include <utility>

#include "bulk.h"

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    std::size_t bulkSize = std::stoul(argv[1]);

    auto reader = std::make_shared<ConsoleReader>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    auto fileLogger = std::make_shared<FileLogger>();
    auto storage = std::make_shared<LogStorage>(bulkSize);

    storage->addListener(consoleLogger);
    storage->addListener(fileLogger, &FileLogger::log);  //этот вариант мне больше нравится.
                                                         //   storage->addListener(
    //       [](std::time_t timestamp, const std::string& commands) { std::cout << "я лямбда!"; });

    Bulk bulk(reader, storage);

    while (bulk.processCommand()) {
        //внимание! идет обработка команд!
    }
    return 0;
}
