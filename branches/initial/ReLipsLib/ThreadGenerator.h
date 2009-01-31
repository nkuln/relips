#pragma once
#include "StageGenerator.h"

public ref class ThreadGenerator
{
public:
	ThreadGenerator(StageGenerator^ instance, String^ musicFile, String^ outFile);
	void ThreadEntryPoint();
private:
	String^ m_musicFile;
	String^ m_outFile;
	StageGenerator^ m_sgInstance;
};
