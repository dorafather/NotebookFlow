#include <stdlib.h>
#include "ID.h"

namespace nsUtil
{
IdMgr::IdMgr(int _nIdMaxNum)
	: m_nIdMaxNum(_nIdMaxNum), m_parrIdNode(NULL)
{
	pthread_mutex_init(&m_Lock, 0);
	if( m_nIdMaxNum < 0 )
		m_nIdMaxNum = 0;

	reset();
}
IdMgr::~IdMgr()
{
	pthread_mutex_destroy(&m_Lock);
	free(m_parrIdNode);
}
int IdMgr::getAvailableId()
{
	int nId = -1;
	pthread_mutex_lock(&m_Lock);
	nId = m_stHeadNode.m_nNextId;
	if( !popId(nId) )
		nId = -1;
	pthread_mutex_unlock(&m_Lock);
	return nId;
}
bool IdMgr::getAvailableId(int _nId)
{
	bool bResult = false;
	pthread_mutex_lock(&m_Lock);
	bResult = popId(_nId);
	pthread_mutex_unlock(&m_Lock);
	return bResult;
}
bool IdMgr::releaseId(int _nId)
{
	bool bResult = false;
	pthread_mutex_lock(&m_Lock);
	bResult = pushId(_nId);
	pthread_mutex_unlock(&m_Lock);
	return bResult;
}
bool IdMgr::reset()
{
	pthread_mutex_lock(&m_Lock);
	m_unIdAvailNum = m_nIdMaxNum;
	free(m_parrIdNode);
	m_parrIdNode = (IdNode_t *)malloc(sizeof(IdNode_t)*m_nIdMaxNum);
	if( !m_parrIdNode )
	{
		pthread_mutex_unlock(&m_Lock);
		return false;
	}
	for( int i = 0; i < m_nIdMaxNum; ++i )
	{
		m_parrIdNode[i].m_nId = i;
		if( i == 0 )
		{
			m_stHeadNode.m_nNextId = i;
			m_parrIdNode[i].m_nPrevId = E_CONST_HEAD_ID;
		}
		else
		{
			m_parrIdNode[i].m_nPrevId = i-1;
		}
		if( i == m_nIdMaxNum-1 )
		{
			m_parrIdNode[i].m_nNextId = E_CONST_TAIL_ID;
			m_stTailNode.m_nPrevId = i;
		}
		else
		{
			m_parrIdNode[i].m_nNextId = i+1;
		}
		m_parrIdNode[i].m_bUse = false;
	}
	pthread_mutex_unlock(&m_Lock);
	return true;
}
bool IdMgr::pushId(int _nId)
{
	if( !m_parrIdNode )
		return false;
	if( _nId >= 0 && _nId < m_nIdMaxNum )
	{
		if( m_parrIdNode[_nId].m_bUse )
		{
			int nPrevId = m_stTailNode.m_nPrevId;
			if( nPrevId == E_CONST_HEAD_ID )
			{
				m_stHeadNode.m_nNextId = _nId;
				m_parrIdNode[_nId].m_nPrevId = E_CONST_HEAD_ID;
				m_parrIdNode[_nId].m_nNextId = E_CONST_TAIL_ID;
				m_stTailNode.m_nPrevId = _nId;
				m_parrIdNode[_nId].m_bUse  = false;
				++m_unIdAvailNum;

				return true;
			}
			else if( nPrevId >= 0 && nPrevId < m_nIdMaxNum )
			{
				m_parrIdNode[nPrevId].m_nNextId = _nId;
				m_parrIdNode[_nId].m_nPrevId = nPrevId;
				m_parrIdNode[_nId].m_nNextId = E_CONST_TAIL_ID;
				m_stTailNode.m_nPrevId = _nId;
				m_parrIdNode[_nId].m_bUse  = false;
				++m_unIdAvailNum;

				return true;
			}
		}
	}
	return false;
}
bool IdMgr::popId(int _nId)
{
	if( !m_parrIdNode )
		return false;
	if( _nId >= 0 && _nId < m_nIdMaxNum )
	{
		if( !m_parrIdNode[_nId].m_bUse )
		{
			int nPrevId = m_parrIdNode[_nId].m_nPrevId;
			int nNextId = m_parrIdNode[_nId].m_nNextId;
			IdNode_t * pstPrevIdNode = NULL, * pstNextIdNode = NULL;
			if( nPrevId == E_CONST_HEAD_ID )
				pstPrevIdNode = &m_stHeadNode;
			else if( nPrevId >= 0 && nPrevId < m_nIdMaxNum )
				pstPrevIdNode = &(m_parrIdNode[nPrevId]);
			if( nNextId == E_CONST_TAIL_ID )
				pstNextIdNode = &m_stTailNode;
			else if( nNextId >= 0 && nNextId < m_nIdMaxNum )
				pstNextIdNode = &(m_parrIdNode[nNextId]);
			if( pstPrevIdNode && pstNextIdNode )
			{
				pstPrevIdNode->m_nNextId = nNextId;
				pstNextIdNode->m_nPrevId = nPrevId;
				m_parrIdNode[_nId].m_bUse = true;
				if( m_unIdAvailNum > 0 )
					--m_unIdAvailNum;
				return true;
			}
		}
	}
	return false;
}
}
