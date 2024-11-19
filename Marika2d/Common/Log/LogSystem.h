#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#ifndef LOG_WORNING
#define MRK_LOG_WORNING(x) Mrk::LogSystem::Log(Mrk::LogLevel::Worning, __FILE__, __FUNCTION__, x);
#endif // !LOG_WORNING

#ifndef LOG_ERROR
#define MRK_LOG_ERROR(x) Mrk::LogSystem::Log(Mrk::LogLevel::Error, __FILE__, __FUNCTION__, x);
#endif // !LOG_ERROR

namespace Mrk
{
	namespace Private
	{
		class LogTarget;
	}

	enum class LogLevel
	{
		Error,
		Worning,
		Debug,
		Info
	};

	enum class LogTargetType
	{
		None,
		Console,
		File
	};

	class LogSystem
	{
	public:
		static void Init(LogTargetType target);
		static void Log(LogLevel level, std::string_view file, std::string_view func, std::string_view log);
		static void Log(std::string_view log);
	private:
		LogSystem();

	private:
		static LogSystem* instance;
		std::unique_ptr<Private::LogTarget> target;
	};

	namespace Private
	{
		class LogTarget
		{
		public:
			LogTarget() = default;
			virtual ~LogTarget() = default;
			virtual void ProcessLog(std::string_view) = 0;
		};

		class NoneLogTarget : public LogTarget
		{
		public:
			NoneLogTarget() = default;
			inline virtual ~NoneLogTarget() override {}
			inline virtual void ProcessLog(std::string_view log) override {}
		};

		class ConsoleLogTarget : public LogTarget
		{
		public:
			ConsoleLogTarget();
			virtual ~ConsoleLogTarget() override;
			virtual void ProcessLog(std::string_view log) override;
		};

		class FileLogTarget : public LogTarget
		{
		public:
			FileLogTarget();
			virtual ~FileLogTarget() override;
			virtual void ProcessLog(std::string_view log) override;
		private:
			std::string logFileName = "";
		};
	}
}

