/**
 * @file   Logger.h
 * @brief  Utility class for logging debug information to a file.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#pragma once
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <string>

#ifdef _DEBUG
#define ENABLE_VERBOSE_LOG
#endif

namespace Debug {
	enum class LogLevel {
		LOG_INFO  = 0b00000, /** General information */
		LOG_TRACE = 0b01001, /** Finer-grained information */
		LOG_DEBUG = 0b01010, /** General debug information */
		LOG_WARN  = 0b01011, /** Event which may lead to an error */
		LOG_ERROR = 0b10100, /** Error in the application */
		LOG_FATAL = 0b10101, /** Error which prevents the application from continuing */
		VERBOSE_MASK = 0b01000,
		ERROR_MASK   = 0b10000,
	};
	const std::unordered_map<LogLevel, std::string> cSeverityStrings = {
		{LogLevel::LOG_INFO , "Info "},
		{LogLevel::LOG_TRACE, "Trace"},
		{LogLevel::LOG_DEBUG, "Debug"},
		{LogLevel::LOG_WARN , "Warn "},
		{LogLevel::LOG_ERROR, "Error"},
		{LogLevel::LOG_FATAL, "Fatal"},
	};

	/**
	 * @brief Utility class for logging debug information to a file.
	 */
	class Logger {
	public:
		/**
		 * @brief Write a message to the log files corresponding to the
		 * severity.
		 * @param level   Type of message to be logged
		 * @param message Message to be logged.
		 * @param file    File where the message is being logged from. Will not
		 * display if nullptr.
		 * @param line    Line of the file where the message is being logged from.
		 * Will not display if file is nullptr.
		 */
		void log(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0);

		/**
		 * @brief Log general information.
		 */
		inline void info(const std::string& message) {
			log(LogLevel::LOG_INFO, message);
		}
		/**
		 * @brief Log verbose debugging information.
		 */
		inline void trace(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(LogLevel::LOG_TRACE, message);
#endif
		}
		/**
		 * @brief Log general debug information.
		 */
		inline void debug(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(LogLevel::LOG_DEBUG, message);
#endif
		}
		/**
		 * @brief Log warnings of events which may lead to errors.
		 */
		inline void warn(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(LogLevel::LOG_WARN, message);
#endif
		}
		/**
		 * @brief Log non-fatal errors in the application.
		 */
		inline void error(const std::string& message) {
			log(LogLevel::LOG_ERROR, message);
		}
		/**
		 * @brief Log errors which prevent the application from continuing.
		 */
		inline void fatal(const std::string& message) {
			log(LogLevel::LOG_FATAL, message);
		}

		inline bool initSuccess() {
			return mInitSuccess;
		}

		static Logger& getLogger() {
			static Logger logger = Logger();
			return logger;
		}
	private:
		Logger();
		~Logger();

		void logTo(const std::string& logFile, LogLevel level, const std::string& message, const char* file = nullptr, int line = 0);

		bool mInitSuccess = false;

		const std::string cLogFileStandard = "standard.log";
#ifdef ENABLE_VERBOSE_LOG
		const std::string cLogFileVerbose  = "debug.log";
#endif
	};
}
