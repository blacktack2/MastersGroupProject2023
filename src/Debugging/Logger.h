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
	enum class Severity {
		LOG_INFO  = 0b00000,
		LOG_TRACE = 0b01001,
		LOG_DEBUG = 0b01010,
		LOG_WARN  = 0b01011,
		LOG_ERROR = 0b10100,
		LOG_FATAL = 0b10101,
		VERBOSE_MASK = 0b01000,
		ERROR_MASK   = 0b10000,
	};
	const std::unordered_map<Severity, std::string> cSeverityStrings = {
		{Severity::LOG_INFO , "Info" },
		{Severity::LOG_TRACE, "Trace"},
		{Severity::LOG_DEBUG, "DEBUG"},
		{Severity::LOG_WARN , "WARN" },
		{Severity::LOG_ERROR, "ERROR"},
		{Severity::LOG_FATAL, "FATAL"},
	};

	class Logger {
	public:
		void log(Severity severity, const std::string& message, const char* file = nullptr, int line = 0);

		inline void info(const std::string& message) {
			log(Severity::LOG_INFO, message);
		}
		inline void trace(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(Severity::LOG_TRACE, message);
#endif
		}
		inline void debug(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(Severity::LOG_DEBUG, message);
#endif
		}
		inline void warn(const std::string& message) {
#ifdef ENABLE_VERBOSE_LOG
			log(Severity::LOG_WARN, message);
#endif
		}
		inline void error(const std::string& message) {
			log(Severity::LOG_ERROR, message);
		}
		inline void fatal(const std::string& message) {
			log(Severity::LOG_FATAL, message);
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

		void logTo(const std::string& logFile, Severity severity, const std::string& message, const char* file = nullptr, int line = 0);

		bool mInitSuccess = false;

		const std::string cLogFileStandard = "standard.log";
#ifdef ENABLE_VERBOSE_LOG
		const std::string cLogFileVerbose  = "debug.log";
#endif
	};
}
