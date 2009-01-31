#include "StdAfx.h"
#include "GenerationProgressArgs.h"

GenerationProgressArgs::GenerationProgressArgs( int percentageCompleted, String^ message )
: m_percentageCompleted(percentageCompleted), m_message(message)
{
}