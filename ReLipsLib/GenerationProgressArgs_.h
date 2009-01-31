#pragma once

class GenerationProgressArgs_
{
public:
	GenerationProgressArgs_(void);
	~GenerationProgressArgs_(void);

	int PercentageCompleted();
	char* Message();
};