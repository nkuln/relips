#pragma once
#include "Stdafx.h"

public ref class GenerationCompletedArgs : EventArgs
{
public:
	GenerationCompletedArgs(bool hasException, Exception^ e);
	bool HasException(){ return m_hasException; }
	Exception^ InnerException(){ return m_innerException; }

private:
	bool m_hasException;
	Exception^ m_innerException;
	
};
