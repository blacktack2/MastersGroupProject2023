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

void Logger::log(Severity severity, const std::string& message, const char* file, int line) {
#ifndef ENABLE_VERBOSE_LOG
	if ((unsigned int)severity & (unsigned int)Severity::VERBOSE_MASK)
		return;
#endif
	logTo(cLogFileStandard, severity, message, file, line);
#ifdef ENABLE_VERBOSE_LOG
	logTo(cLogFileVerbose, severity, message, file, line);
#endif
}

void Logger::logTo(const std::string& logFile, Severity severity, const std::string& message, const char* file, int line) {
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::ofstream stream(logFile, std::ios_base::app);
	if (!stream)
		return;
	stream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
		<< (file == nullptr ? " " :
			std::string(" Line: ").append(std::to_string(line))
			.append(" File: ").append(file).append(" "))
		<< cSeverityStrings.find(severity)->second << " " << message << "\n";
	stream.close();
}
