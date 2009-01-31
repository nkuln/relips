#pragma once

class StageGenerator_
{
public:
	StageGenerator_(void);
	~StageGenerator_(void);

	bool CreateStageFromFile(char* musicFile, char* outFile);
	void CreateStageFromFileAsync(char* musicFile, char* outFile);
	bool InOperation();
	void AbortOperation();
};
