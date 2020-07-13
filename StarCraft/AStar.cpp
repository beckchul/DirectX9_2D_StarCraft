#include "StdAfx.h"
#include "AStar.h"
#include "ObjMgr.h"
#include "BackGround.h"
#include "queue"

void CAStar::StartAStar(const int& iStartIndex
	, const int& iGoalIndex)
{
	if (iStartIndex == iGoalIndex)
		return;

	int iEndIndex = iGoalIndex;
	const vector<TILE*> pVector
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	if (&pVector == NULL)
		return;

	if (pVector[iStartIndex]->byOption == 2 ||
		pVector[iStartIndex]->byOption == 3 ||
		pVector[iStartIndex]->byOption == 5 ||
		pVector[iStartIndex]->byOption == 6 ||
		pVector[iStartIndex]->byOption == 7)
	{
		int iSettingIndex = iStartIndex;

		int iX = iSettingIndex % TILEEX;
		int iY = iSettingIndex / TILEEX;

		int iX2 = iEndIndex % TILEEX;
		int iY2 = iEndIndex / TILEEX;

		if (iX < iX2)
			++iSettingIndex;
		else if (iX > iX2)
			--iSettingIndex;
		if (iY < iY2)
			iSettingIndex += TILEEX;
		else if (iY > iY2)
			iSettingIndex -= TILEEX;

		if (iSettingIndex < 0 || iSettingIndex >= TILEEX * TILEEY)
			return;

		m_BestList.push_back(iSettingIndex);
		return;
	}

	while (pVector[iEndIndex]->byOption == 1 ||
		pVector[iEndIndex]->byOption == 2 ||
		pVector[iEndIndex]->byOption == 3 ||
		pVector[iEndIndex]->byOption == 5 ||
		pVector[iEndIndex]->byOption == 6 ||
		pVector[iEndIndex]->byOption == 7)
	{
		int iX = iStartIndex % TILEEX;
		int iY = iStartIndex / TILEEX;

		int iX2 = iEndIndex % TILEEX;
		int iY2 = iEndIndex / TILEEX;

		if (iX < iX2)
			--iEndIndex;
		else if (iX > iX2)
			++iEndIndex;
		else if (iY < iY2)
			iEndIndex -= TILEEX;
		else if (iY > iY2)
			iEndIndex += TILEEX;

		if (iEndIndex < 0 || iEndIndex >= TILEEX * TILEEY)
			return;

		if (iStartIndex == iEndIndex)
			return;
	}
	m_BestList.clear();
	m_iStartIndex = iStartIndex;
	m_iGoalIndex = iEndIndex;

	MakeRoute();
}

void CAStar::StartAStarPos(const D3DXVECTOR3& vStartPos
	, const D3DXVECTOR3& vGoalPos)
{
	m_iStartIndex = GetTileIndex(vStartPos);

	m_iGoalIndex = GetTileIndex(vGoalPos);

	if (m_iGoalIndex == -1)
		return;

	StartAStar(m_iStartIndex, m_iGoalIndex);
}

void CAStar::MakeRoute()
{
	int F = 0; // 비용
	int G = 0; // 시작점으로부터 새로운 사각형까지의 이동비용길을 찾아갈 수록 G의 값은 커진다.
	int H = 0; // 얻어진 사각형으로부터 최종목적지점 까지의 예상이동비용입니다.

 // p : 열린 목록
	priority_queue< pair<int, NODE*>, vector< pair<int, NODE*> >, greater< pair<int, NODE*> > > p; //  pair < F값,  노드 >

	int iX = m_iGoalIndex % TILEEX;
	int iY = m_iGoalIndex / TILEEX;

	int iX2 = m_iStartIndex % TILEEX;
	int iY2 = m_iStartIndex / TILEEX;

	int Start[2] = { iX, iY };
	int End[2] = { iX2, iY2 };

	int G_Openlist[TILEEX][TILEEY] = {}; // 오픈리스트의 G값을 저장합니다.
	bool Closelist[TILEEX][TILEEY]; // 닫힌 영역인지 판별하기 위한 배열
	memset(Closelist, false, sizeof(Closelist));
	list<NODE*> Deletelist; // 삭제할 노드 리스트들

	NODE* NodeArr[TILEEX][TILEEY];

	NODE StartNode = { G, H, Start, nullptr }; // 시작 지점

	p.push(make_pair(G + H, &StartNode));
	G_Openlist[StartNode.vertex[0]][StartNode.vertex[1]] = 0;
	NodeArr[StartNode.vertex[0]][StartNode.vertex[1]] = nullptr; // 시작 지점의 부모 노드는 널 포인터이다.

	bool IsFinish = false;

	const vector<TILE*> pVector
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	while (p.size() && !IsFinish)
	{
		NODE* Node = new NODE;
		Deletelist.push_back(Node);
		Node = p.top().second; // 우선 순위 큐로 가장 F값이 작은 값을 뱉어준다

		p.pop();

		for (int i = -1; i <= 1; ++i) // 시작점 근처에 붙어있고 지나갈 수 있는 모든 사각형들을 봅시다. 
		{
			for (int j = -1; j <= 1; ++j)
			{
				int iIndex[2] = { Node->vertex[0] + i, Node->vertex[1] + j };

				if (iIndex[0] == End[0] && iIndex[1] == End[1]) // 종료
					IsFinish = true;

				// 예외적인 경우
				if (iIndex[0] < 0 || iIndex[1] < 0 || iIndex[0] >= TILEEX || iIndex[1] >= TILEEY) // 끝 부분 ( TILEEX, TILEEY 보다 작아야 함)
					continue;
				if (i == 0 && j == 0) // 자기 자신
					continue;
				if (true == Closelist[iIndex[0]][iIndex[1]]) // 닫힌 영역
					continue;
				if (0 != pVector[iIndex[0] + iIndex[1] * TILEEX]->byOption &&
					4 != pVector[iIndex[0] + iIndex[1] * TILEEX]->byOption)
					continue;

				G = Node->G + ((i * j == 0) ? 10 : 14);
				H = (abs(End[0] - iIndex[0]) + abs(End[1] - iIndex[1])) * 10;

				if (0 != G_Openlist[iIndex[0]][iIndex[1]]) // 열린 영역일 때
				{
					if (G_Openlist[iIndex[0]][iIndex[1]] > G) // 구한 G값이 원래 G값보다 작다면,
					{
						NodeArr[iIndex[0]][iIndex[1]]->pNode = Node; // 부모 갱신
						NodeArr[iIndex[0]][iIndex[1]]->G = G;
						G_Openlist[iIndex[0]][iIndex[1]] = G; // G값 갱신

						p.push(make_pair(G + H, NodeArr[iIndex[0]][iIndex[1]])); // 열린 목록의 F 값을 갱신하는 것

						break;
					}
				}
				else // 새롭게 추가될 노드들
				{
					NODE* pNode = new NODE(G, H, iIndex, Node); // 추가된 각각의 사각형들에게 현재 노드를 부모사각형이라고 저장합니다 
					Deletelist.push_back(pNode);

					p.push(make_pair(G + H, pNode)); // 그리고 그 지나갈 수 있는 사각형들을 열린목록에 추가합니다.
					G_Openlist[pNode->vertex[0]][pNode->vertex[1]] = G;
					NodeArr[pNode->vertex[0]][pNode->vertex[1]] = pNode;
				}
			}
		}
		// 다시는 볼 필요가 없는 사각형들의 목록인 닫힌목록에 추가합니다.
		Closelist[Node->vertex[0]][Node->vertex[1]] = true;
	}

	NODE* pTemp = p.top().second; // 목표 지점( 인덱스가 End[2] 랑 같다)
	m_BestList.clear();

	int iCount = 0;
	while (pTemp->pNode != nullptr)
	{
		if (iCount != 0)			// 누르는 순간에 시작 인덱스를 목표지점으로 넣는것을 방지
		{
			m_BestList.push_back(pTemp->vertex[0] + pTemp->vertex[1] * TILEEX);
		}
		++iCount;
		pTemp = pTemp->pNode;
	}

	m_BestList.push_back(m_iGoalIndex);		// 자기가 클릭한 인덱스 전까지 가는것을 방지

	// 다 끝나고 메모리 해제
	for_each(Deletelist.begin(), Deletelist.end(), DEL_OBJ());
	Deletelist.clear();
}

int CAStar::GetTileIndex(const D3DXVECTOR3& vPos)
{

	int iX = S_INT_CAST(vPos.x / TILECX);
	int iY = S_INT_CAST(vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return -1;

	return iIndex;
}

void CAStar::Release(void)
{
	m_BestList.clear();
}

CAStar::CAStar(void)
{
}

CAStar::~CAStar(void)
{
	Release();
}


void CAStar::StopCheck(CGameObject * pObject, CGameObject* pObject2)
{
	

	int iX = S_INT_CAST(pObject->GetInfo().vPos.x / TILECX);
	int iY = S_INT_CAST(pObject->GetInfo().vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	if (m_BestList.end() == m_BestList.begin())
		return;

	list<int>::iterator iter = m_BestList.end();
	
	--iter;

	int iX2 = (*iter) % TILEEX;
	int iY2 = (*iter) / TILEEX;


	int iObj2Index = S_INT_CAST(pObject->GetInfo().vPos.x / TILECX) + S_INT_CAST(pObject->GetInfo().vPos.y / TILECY) * TILEEX;

	if (iObj2Index < 0 || iObj2Index >= TILEEX * TILEEY)
		return;

	if ((abs(iX - iX2) < 2 && abs(iY - iY2) < 2 && pObject->GetState() != UNIT_ATTACK && pObject2->GetState() != UNIT_ATTACK)
		|| (*iter) == iObj2Index)
		m_BestList.clear();
	else if (abs(iX - iX2) > 10 && abs(iY - iY2) > 10/* && pObject->GetState() != UNIT_ATTACK*/)
	{
		return;
	}
	else //if(pObject->GetState() != UNIT_ATTACK)
	{
		int iX3 = S_INT_CAST(pObject2->GetInfo().vPos.x / TILECX);
		int iY3 = S_INT_CAST(pObject2->GetInfo().vPos.y / TILECY);

		const vector<TILE*> pVector
			= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

		int iIndex3 = iX3 + (TILEEX * iY3);
		if (iIndex3 < 0 || iIndex3 >= TILEEX * TILEEY)
			return;

		ObjMgr->FindGameObject<CBackGround>()->SetTileUnitOption(iIndex3);

		StartAStarPos(pVector[iIndex]->vPos, pVector[(*iter)]->vPos);

		if (m_BestList.end() == m_BestList.begin())
			return;
		list<int>::iterator iter2 = m_BestList.end();

		--iter2;
		 
		iX2 = (*iter2) % TILEEX;
		iY2 = (*iter2) / TILEEX;
		if (abs(iX - iX2) < 1 && abs(iY - iY2) < 1)
			m_BestList.clear();

		ObjMgr->FindGameObject<CBackGround>()->SetTileUnitOption(iIndex3);
	}
}