#include "LogSystem.h"

#include <windows.h>
#include <iostream>
#include <format>

Mrk::LogSystem* Mrk::LogSystem::instance = nullptr;

void Mrk::LogSystem::Init(LogTargetType targetType)
{
	if (instance == nullptr)
	{
		try
		{
			instance = new LogSystem();
			switch (targetType)
			{
			case Mrk::LogTargetType::None:
				instance->target = std::make_unique<Private::NoneLogTarget>();
				break;
			case Mrk::LogTargetType::Console:
				instance->target = std::make_unique<Private::ConsoleLogTarget>();
				break;
			case Mrk::LogTargetType::File:
				instance->target = std::make_unique<Private::FileLogTarget>();
				break;
			default:
				instance->target = std::make_unique<Private::NoneLogTarget>();
				break;
			}
		}
		catch (const std::exception&)
		{
			instance->target = std::make_unique<Private::NoneLogTarget>();
			throw std::runtime_error("Init Log System Failed");
		}
	}
}

void Mrk::LogSystem::Log(LogLevel level, std::string_view file, std::string_view func, std::string_view log)
{
	if (!instance)
	{
		throw std::runtime_error("Forget To Call LogSystem::Init() ?");
	}
	else
	{
		std::string logHead;
		switch (level)
		{
		case Mrk::LogLevel::Error:
			logHead = "Error";
			break;
		case Mrk::LogLevel::Worning:
			logHead = "Worning";
			break;
		case Mrk::LogLevel::Debug:
			logHead = "Debug";
			break;
		case Mrk::LogLevel::Info:
			logHead = "Info";
			break;
		default:
			break;
		}

		instance->target->ProcessLog(std::format("{} : At File [{}], In Func [{}] : {}",logHead, file, func, log));
	}
}

void Mrk::LogSystem::Log(std::string_view log)
{
	instance->target->ProcessLog(std::format("{}{}", "Log : ", log));
}

Mrk::LogSystem::LogSystem()
{

}

Mrk::Private::ConsoleLogTarget::ConsoleLogTarget()
{
	if (!GetConsoleWindow())
	{
		if (AllocConsole())
		{
			FILE* fp;
			freopen_s(&fp, "CONOUT$", "w", stdout);
			freopen_s(&fp, "CONOUT$", "w", stderr);
			freopen_s(&fp, "CONIN$", "r", stdin);
		}
		else
		{
			throw;
		}
	}
}

Mrk::Private::ConsoleLogTarget::~ConsoleLogTarget()
{
	FreeConsole();
}

void Mrk::Private::ConsoleLogTarget::ProcessLog(std::string_view log)
{
	std::cout << log << "\n";
}

Mrk::Private::FileLogTarget::FileLogTarget()
{
}

Mrk::Private::FileLogTarget::~FileLogTarget()
{

}

void Mrk::Private::FileLogTarget::ProcessLog(std::string_view log)
{
}
