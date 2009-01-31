#pragma once

class GenerationCompletedArgs_
{
public:
	GenerationCompletedArgs_(void);
	~GenerationCompletedArgs_(void);
	bool HasException();
	Exception InnerException();
};
