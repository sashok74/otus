
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bulk.h"

std::vector<std::string> readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

using ::testing::Return;
using ::testing::Sequence;

// Мок-класс для Reader
class MockReader : public Bulk::Reader {
   public:
    MOCK_METHOD(bool, readCommand, (std::string&), (override));
};

// Мок-класс для Storage
class MockStorage : public Bulk::Storage {
   public:
    MOCK_METHOD(void, addCommand, (const std::string&), (override));
    MOCK_METHOD(void, flush, (), (override));
};

// Мок-класс для Logger
class MockLogger : public Logger {
   public:
    MOCK_METHOD(void, log, (std::time_t, const std::string&), (override));
};

// Тест обработки команд
TEST(BulkTest, ProcessCommand) {
    // Создаем мок-объекты
    auto mockReader = std::make_shared<MockReader>();
    auto mockStorage = std::make_shared<MockStorage>();

    // Создаем объект Bulk с мок-объектами
    Bulk bulk(mockReader, mockStorage);

    // Устанавливаем последовательность вызовов
    Sequence s;
    std::string cmd1 = "command1";
    std::string cmd2 = "command2";

    // Ожидаем последовательные вызовы readCommand и подставляем значения
    EXPECT_CALL(*mockReader, readCommand(testing::_))
        .InSequence(s)
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(cmd1), Return(true)))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(cmd2), Return(true)))
        .WillOnce(Return(false));  // Имитация конца ввода

    // Ожидаем, что команды будут добавлены в хранилище
    EXPECT_CALL(*mockStorage, addCommand("command1")).Times(1);
    EXPECT_CALL(*mockStorage, addCommand("command2")).Times(1);
    EXPECT_CALL(*mockStorage, flush()).Times(0);  // flush не вызывается в данном тесте

    // Запускаем обработку команд
    while (bulk.processCommand()) {
    }
}

// Тест обработки команд с логированием
TEST(BulkTest, ProcessCommandWithLogging) {
    auto mockReader = std::make_shared<MockReader>();
    auto mockLogger = std::make_shared<MockLogger>();
    auto mockStorage = std::make_shared<LogStorage>(3);  // Блок по 3 команды родное хранилище

    // Добавляем логгер в хранилище
    mockStorage->addListener(mockLogger);

    // Создаём Bulk с моками
    Bulk bulk(mockReader, mockStorage);

    std::vector<std::string> commands = {"cmd1", "cmd2", "cmd3", "cmd1", "cmd2",
                                         "{",    "cmd3", "cmd4", "}",    "cmd5"};
    size_t index = 0;

    // Мокируем вызовы readCommand()
    EXPECT_CALL(*mockReader, readCommand(testing::_)).WillRepeatedly([&](std::string& cmd) {
        if (index < commands.size()) {
            cmd = commands[index++];
            return true;
        }
        return false;
    });

    // Ожидаем вызовы логгера с правильными блоками команд
    EXPECT_CALL(*mockLogger, log(testing::_, "cmd1, cmd2, cmd3")).Times(1);  // Стандартный блок
    EXPECT_CALL(*mockLogger, log(testing::_, "cmd1, cmd2"))
        .Times(1);  // Стандартный блок прервали {
    EXPECT_CALL(*mockLogger, log(testing::_, "cmd3, cmd4")).Times(1);  // Динамический блок
    EXPECT_CALL(*mockLogger, log(testing::_, "cmd5")).Times(1);  // Остаток

    // Запускаем обработку команд
    while (bulk.processCommand()) {
    }
}

TEST(BulkTest, ProcessCommandWithLoggingVector) {
    // Наборы входных команд и ожидаемых логов
    std::vector<std::vector<std::string>> commandsList = {
        {"cmd1", "cmd2", "cmd3", "cmd1", "cmd2", "{", "cmd3", "cmd4", "}", "cmd5"},
        {"cmdA", "cmdB", "cmdC", "{", "cmdD", "cmdE", "}", "cmdF"},
        {"cmdA", "cmdB", " ", "{", "cmdD", "cmdE", "}", "cmdF"}
        // Можно добавить и другие наборы команд, если требуется
    };

    std::vector<std::vector<std::string>> expectedLogs = {
        {"cmd1, cmd2, cmd3", "cmd1, cmd2", "cmd3, cmd4", "cmd5"},
        {"cmdA, cmdB, cmdC", "cmdD, cmdE", "cmdF"},
        {"cmdA, cmdB", "cmdD, cmdE", "cmdF"}
        // Можно добавить и другие ожидаемые выходные данные
    };

    for (size_t testIndex = 0; testIndex < commandsList.size(); ++testIndex) {
        // Создаём моки заново для каждой итерации
        auto mockReader = std::make_shared<MockReader>();
        auto mockLogger = std::make_shared<MockLogger>();
        auto mockStorage = std::make_shared<LogStorage>(3);  // Блок по 3 команды
        mockStorage->addListener(mockLogger);
        Bulk bulk(mockReader, mockStorage);

        // Переменная для отслеживания позиции в текущем наборе команд
        size_t index = 0;
        const auto& commands = commandsList[testIndex];
        const auto& expected = expectedLogs[testIndex];

        // Мокаем вызовы readCommand() так, чтобы возвращались команды из текущего набора
        EXPECT_CALL(*mockReader, readCommand(testing::_))
            .WillRepeatedly([&](std::string& cmd) -> bool {
                if (index < commands.size()) {
                    cmd = commands[index++];
                    return true;
                }
                std::cout << std::endl;
                return false;
            });

        // Задаём ожидания для логгера: для каждого ожидаемого лога — ровно один вызов log с нужными
        // параметрами
        for (const auto& expectedLog : expected)
            EXPECT_CALL(*mockLogger, log(testing::_, expectedLog)).Times(1);

        // Запускаем обработку команд
        while (bulk.processCommand()) {
        }
    }
}

//=================== MAIN ===================

// Основной тест-метод
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
