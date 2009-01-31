#include "StdAfx.h"
#include "Fragment.h"

Fragment::Fragment(void)
	: m_start(0), m_end(0), m_note(0)
{

}

Fragment::Fragment( int start, int end, int note ) 
	: m_start(start), m_end(end), m_note(note)
{

}

Fragment::~Fragment(void)
{
}

int Fragment::Length()
{
	return m_end - m_start;
}
bool operator<(const Fragment& a, const Fragment& b) {
	return a.Start() < b.Start();
}

