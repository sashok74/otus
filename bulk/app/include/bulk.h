#pragma once

#include <chrono>
#include <ctime>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Класс для управления пакетами команд.
 */
class Bulk {
   public:
    /**
     * @brief Класс для чтения команд.
     */
    class Reader {
       public:
        /**
         * @brief Виртуальный деструктор.
         */
        virtual ~Reader() = default;

        /**
         * @brief Читает команду.
         * @param[in,out] command Cтрока, куда будет записана команда.
         * @return Успешность операции чтения.
         */
        virtual bool readCommand(std::string &) = 0;
    };

    /**
     * @brief Класс для хранения команд.
     */
    class Storage {
       public:
        /**
         * @brief Виртуальный деструктор.
         */
        virtual ~Storage() = default;

        /**
         * @brief Добавить команду в хранилище.
         * @param command Строка с командой.
         */
        virtual void addCommand(const std::string &command) = 0;

        /**
         * @brief Очистить накопленные команды.
         */
        virtual void flush() = 0;
    };

    /**
     * @brief Конструктор класса Bulk.
     * @param reader Указатель на объект для чтения команд.
     * @param storage Указатель на объект для хранения команд.
     */
    Bulk(std::shared_ptr<Reader> reader, std::shared_ptr<Storage> storage);

    /**
     * @brief Напечатать версию программы.
     */
    void printVersion() const;

    /**
     * @brief Обработать команду.
     * @return Успешность обработки.
     */
    bool processCommand();

    /**
     * @brief Очистить пакет команд.
     */
    void flushBulk();

   private:
    std::shared_ptr<Reader> reader_;  ///< Указатель на объект чтения команд.
    std::shared_ptr<Storage> storage_;  ///< Указатель на объект хранения команд.
};

/**
 * @brief Чтение команд с консоли.
 */
class ConsoleReader : public Bulk::Reader {
   public:
    /**
     * @brief Реализует чтение команды из консоли.
     * @param cmd Буфер для записи прочитанной команды.
     * @return Успешность чтения команды.
     */
    bool readCommand(std::string &cmd) override;
};

/**
 * @brief Интерфейc для логирования.
 */
class Logger {
   public:
    /**
     * @brief Виртуальный деструктор.
     */
    virtual ~Logger() = default;
    //~Logger(){std::cout << "log destroy";}
    /**
     * @brief Логирование команд.
     * @param timestamp Время когда пришла команда.
     * @param commands Строка с командами.
     */
    virtual void log(std::time_t timestamp, const std::string &commands) = 0;
};

/**
 * @brief Логирование команд в консоль.
 */
class ConsoleLogger : public Logger {
   public:
    /**
     * @brief Логирование команд с указанием времени в консоль.
     * @param timestamp Время логирования.
     * @param commands Строка с командами.
     */
    void log(std::time_t, const std::string &commands) override;
};

/**
 * @brief Логирование команд в файл.
 */
class FileLogger : public Logger {
   public:
    /**
     * @brief Логирование команд с указанием времени в файл.
     * @param timestamp Время когда пришла команда.
     * @param commands Строка с командами.
     */
    void log(std::time_t timestamp, const std::string &commands) override;

   private:
    /**
     * @brief Генерация имени файла на основе времени первой команды в блоке.
     * @param timestamp Время для генерации имени файла.
     * @return Имя файла.
     */
    static std::string generateFilename(std::time_t timestamp);
};

/**
 * @brief Хранилище команд с возможностью выполнить обратный вызов при создании блока.
 */
class LogStorage : public Bulk::Storage {
   public:
    using Callback = std::function<void(std::time_t, const std::string &)>;
    /**
     * @brief Конструктор LogStorage.
     * @param batchSize Размер пакета.
     */
    explicit LogStorage(std::size_t batchSize);

    /**
     * @brief Деструктор LogStorage.
     */
    ~LogStorage();

    /**
     * @brief Добавить команду в буфер.
     * @param command Строка с командой.
     */
    void addCommand(const std::string &command) override;

    /**
     * @brief Сбросить буферизированные команды.
     */
    void flush() override;

    /**
     * @brief Метод для добавления колбека в список.
     * @param logger Указатель на логер.
     */
    void addListener(const std::shared_ptr<Logger> &listener);

    /**
     * @brief Метод для добавления колбека в список.
     * @param callback функция или лямбда..
     */
    void addListener(const Callback &callback);

    /**
     * @brief Метод для добавления колбека в список.
     * @param obj указатель на объект метод которого вызываем как колбэк
     * @param method метод класса
     */
    template <typename T>
    void addListener(std::shared_ptr<T> obj, void (T::*method)(std::time_t, const std::string &)) {
        std::weak_ptr<T> weakObj = obj;
        loggers_.push_back([weakObj, method](std::time_t timestamp, const std::string &commands) {
            if (auto sharedObj = weakObj.lock()) {
                (sharedObj.get()->*method)(timestamp, commands);
            } else {
                std::cout << "Неовозможно вызвать функцию" << std::endl;
            }
        });
    }

   private:
    using LogItem = std::pair<std::time_t, std::string>;  ///< Пара времени и команды.
    std::size_t batchSize_;                               ///< Размер пакета.
    std::deque<LogItem> commandsBuffer_;                  ///< Буфер команд.
    std::vector<Callback> loggers_;                       ///< Список колбеков
    bool dynamicMode_ = false;  ///< Флаг динамического режима.
    void trim(std::string &s);
};