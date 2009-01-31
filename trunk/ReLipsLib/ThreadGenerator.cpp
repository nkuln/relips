#include "StdAfx.h"
#include "ThreadGenerator.h"



ThreadGenerator::ThreadGenerator(StageGenerator^ instance, String^ musicFile, String^ outFile )
:m_musicFile(musicFile), m_outFile(outFile), m_sgInstance(instance)
{

}

void ThreadGenerator::ThreadEntryPoint()
{
	m_sgInstance->CreateStageFromFile(m_musicFile, m_outFile);
}