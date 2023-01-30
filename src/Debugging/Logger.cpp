/**
 * @file   Logger.cpp
 * @brief  See Logger.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>

using namespace Debug;

Logger::Logger() {
	std::ofstream stream(cLogFileStandard);
	if (!stream)
		return;
#ifdef ENABLE_VERBOSE_LOG
	std::ofstream vStream(cLogFileVerbose);
	if (!vStream)
		return;
#endif
	mInitSuccess = true;
}

Logger::~Logger() {
}

void Logger::log(LogLevel level, const std::string& message, const char* file, int line) {
#ifdef ENABLE_VERBOSE_LOG
	logTo(cLogFileVerbose, level, message, file, line);
#endif
	if ((unsigned int)level & (unsigned int)LogLevel::VERBOSE_MASK)
		return;
	logTo(cLogFileStandard, level, message, file, line);
}

void Logger::logTo(const std::string& logFile, LogLevel level, const std::string& message, const char* file, int line) {
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::ofstream stream(logFile, std::ios_base::app);
	if (!stream)
		return;
	stream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S ")
		<< cSeverityStrings.find(level)->second
		<< (file == nullptr ? "" :
			std::string(" at line: ").append(std::to_string(line))
			.append(" of file: ").append(file).append(" "))
		<< " " << message << "\n";
	stream.close();
}
