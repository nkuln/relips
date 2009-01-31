#pragma once

class Fragment
{
public:
	Fragment(void);
	Fragment(int start, int end, int note);
	~Fragment(void);

	int Note() const { return m_note; }
	void Note(int val) { m_note = val; }

	int Start() const { return m_start; }
	void Start(int val) { m_start = val; }

	int End() const { return m_end; }
	void End(int val) { m_end = val; }

	int Length();

private:
	int m_start;
	int m_end;
	int m_note;
};