#pragma once

public ref class GenerationProgressArgs : EventArgs
{
public:
	GenerationProgressArgs(int percentageCompleted, String^ message);
	int PercentageCompleted(){ return m_percentageCompleted; }
	String^ Message(){ return m_message; }
private:
	int m_percentageCompleted;
	String^ m_message;

};
