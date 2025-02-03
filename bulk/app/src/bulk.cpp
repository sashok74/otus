#include "bulk.h"

#include <algorithm>

Bulk::Bulk(std::shared_ptr<Reader> reader, std::shared_ptr<Storage> storage)
    : reader_(std::move(reader)), storage_(std::move(storage)) {}

void Bulk::printVersion() const {
    std::cout << "Version 1.0" << std::endl;
}

/**
 * @brief Обрабатывает команду из потока.
 *
 * Алгоритм:
 * - Чтение команды с использованием  объекта Reader.
 * - Если команда успешно считана, добавить её в Storage и возвращаем true
 * - Если команды закончились то возвращаем false
 *   @include processCommand_algorithm.md
 */
bool Bulk::processCommand() {
    std::string command;
    if (reader_->readCommand(command)) {
        storage_->addCommand(command);
        return true;
    }
    return false;  // No commands
}

void Bulk::flushBulk() {
    storage_->flush();
}

bool ConsoleReader::readCommand(std::string &cmd) {
    if (!std::getline(std::cin, cmd)) {
        return false;  // No more commands.
    }
    return true;
}

void ConsoleLogger::log(std::time_t, const std::string &commands) {
    std::cout << "bulk: " << commands << std::endl;
}

void FileLogger::log(std::time_t timestamp, const std::string &commands) {
    std::ofstream file(generateFilename(timestamp));
    if (!file) {
        throw std::runtime_error("Failed to open log file");
    }
    file << commands << std::endl;
}

std::string FileLogger::generateFilename(std::time_t timestamp) {
    return "bulk" + std::to_string(timestamp) + ".log";
}

LogStorage::LogStorage(std::size_t batchSize) : batchSize_(batchSize) {}

LogStorage::~LogStorage() {
    flush();
}

void LogStorage::trim(std::string &s) {
    // Удаляем начальные пробелы
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    // Удаляем конечные пробелы
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

void LogStorage::addCommand(const std::string &command_in) {
    std::string command = command_in;
    trim(command);
    if (command.empty()) return;
    if (!dynamicMode_) {
        if (command == "{") {
            flush();
            dynamicMode_ = true;
            return;
        }
        commandsBuffer_.emplace_back(std::time(nullptr), command);
        if (commandsBuffer_.size() == batchSize_) flush();
    } else {
        if (command == "}") {
            flush();
            dynamicMode_ = false;
            return;
        }
        commandsBuffer_.emplace_back(std::time(nullptr), command);
    }
}

void LogStorage::flush() {
    if (commandsBuffer_.empty()) return;

    std::ostringstream oss;
    std::time_t firstTimestamp = commandsBuffer_.front().first;
    for (size_t i = 0; i < commandsBuffer_.size(); ++i) {
        oss << commandsBuffer_[i].second;
        if (i < commandsBuffer_.size() - 1) oss << ", ";
    }
    std::string output = oss.str();
    for (const auto &logger : loggers_) {
        logger(firstTimestamp, output);
    }
    commandsBuffer_.clear();
}

void LogStorage::addListener(const std::shared_ptr<Logger> &listener) {
    std::weak_ptr<Logger> weakListener = listener;
    loggers_.push_back([weakListener](std::time_t timestamp, const std::string &commands) {
        if (auto sharedListener = weakListener.lock()) {
            sharedListener->log(timestamp, commands);
        } else {
            std::cout << "не возможно вызвать слушатель" << std::endl;
        }
    });
}

void LogStorage::addListener(const Callback &callback) {
    loggers_.push_back(callback);
}
