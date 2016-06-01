#include "stdafx.h"
#include <queue>
#pragma once

template <class T>
class CIDGenerator
{
public:
	typedef queue<T>	FREELIST;

	CIDGenerator(T start_id, T max_id, int block_count = 500)
	{
		UNREFERENCED_PARAMETER(block_count);

		m_LastID = start_id;
		m_MaxID = max_id;
		m_BlockCount = 500;
		AllocID(m_BlockCount);

		m_nUsed = 0;
	}

	T NewID()
	{
		if (m_FreeList.empty())
			AllocID(m_BlockCount);

		T new_id = m_FreeList.front();
		m_FreeList.pop();

		m_nUsed++;
		CalcUsage();

		return new_id;
	}

	void FreeID(T id)
	{
		_ASSERTE(m_nUsed > 0);

		m_nUsed--;
		CalcUsage();

		m_FreeList.push(id);
	}

	void GetUsage(DWORD& nTotal, DWORD& nUsed, DWORD& nFree)
	{
		nTotal = m_nTotal;
		nUsed = m_nUsed;
		nFree = m_nFree;
	}

protected:
	T			m_MaxID;
	T			m_LastID;
	FREELIST	m_FreeList;
	int			m_BlockCount;

	DWORD		m_nTotal;
	DWORD		m_nUsed;
	DWORD		m_nFree;

	void	AllocID(int count)
	{
		_ASSERTE(m_LastID < (T)(m_MaxID - count));

		for (int i = 0; i < count; i++, m_LastID++) m_FreeList.push(m_LastID);
	}

	void	CalcUsage()
	{
		m_nTotal = m_LastID;
		m_nFree = m_nTotal - m_nUsed;
	}
};
