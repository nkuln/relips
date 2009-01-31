#include "StdAfx.h"
#include "GenerationCompletedArgs.h"


GenerationCompletedArgs::GenerationCompletedArgs( bool hasException, Exception^ e )
	:m_hasException(hasException), m_innerException(e)
{

}