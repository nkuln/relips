#include "StdAfx.h"
#include "Stage.h"

Stage::Stage(char *filename)
{
	m_frags = new deque<Fragment *>();
	LoadFromFile(filename);
}

Stage::~Stage(void)
{
	ClearFragments();
	delete m_frags;
}

void Stage::MockStage()
{
	m_frags->push_back(new Fragment(100000,200000,Note::NT_A));
	m_frags->push_back(new Fragment(250000,350000,Note::NT_G));
	m_frags->push_back(new Fragment(400000,600000,Note::NT_G));
	m_frags->push_back(new Fragment(700000,800000,Note::NT_B));
}

void Stage::LoadFromFile( char* filename )
{
	ClearFragments();
	FILE *fi = fopen(filename, "r");
	int start, end, note;
	while(!feof(fi)){
		fscanf(fi,"%d %d %d\n", &start, &end, &note);

		if(start == 9277000){
			cout << "xxxxxx" << endl;
		}

		Fragment *f = new Fragment(start, end, note);
		m_frags->push_back(f);
	}
	fclose(fi);
}

void Stage::ClearFragments()
{
	for(deque<Fragment *>::iterator i = m_frags->begin(); i != m_frags->end(); i++){
		delete *i;
	}
}