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
	int F = 0; // ���
	int G = 0; // ���������κ��� ���ο� �簢�������� �̵������� ã�ư� ���� G�� ���� Ŀ����.
	int H = 0; // ����� �簢�����κ��� ������������ ������ �����̵�����Դϴ�.

 // p : ���� ���
	priority_queue< pair<int, NODE*>, vector< pair<int, NODE*> >, greater< pair<int, NODE*> > > p; //  pair < F��,  ��� >

	int iX = m_iGoalIndex % TILEEX;
	int iY = m_iGoalIndex / TILEEX;

	int iX2 = m_iStartIndex % TILEEX;
	int iY2 = m_iStartIndex / TILEEX;

	int Start[2] = { iX, iY };
	int End[2] = { iX2, iY2 };

	int G_Openlist[TILEEX][TILEEY] = {}; // ���¸���Ʈ�� G���� �����մϴ�.
	bool Closelist[TILEEX][TILEEY]; // ���� �������� �Ǻ��ϱ� ���� �迭
	memset(Closelist, false, sizeof(Closelist));
	list<NODE*> Deletelist; // ������ ��� ����Ʈ��

	NODE* NodeArr[TILEEX][TILEEY];

	NODE StartNode = { G, H, Start, nullptr }; // ���� ����

	p.push(make_pair(G + H, &StartNode));
	G_Openlist[StartNode.vertex[0]][StartNode.vertex[1]] = 0;
	NodeArr[StartNode.vertex[0]][StartNode.vertex[1]] = nullptr; // ���� ������ �θ� ���� �� �������̴�.

	bool IsFinish = false;

	const vector<TILE*> pVector
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	while (p.size() && !IsFinish)
	{
		NODE* Node = new NODE;
		Deletelist.push_back(Node);
		Node = p.top().second; // �켱 ���� ť�� ���� F���� ���� ���� ����ش�

		p.pop();

		for (int i = -1; i <= 1; ++i) // ������ ��ó�� �پ��ְ� ������ �� �ִ� ��� �簢������ ���ô�. 
		{
			for (int j = -1; j <= 1; ++j)
			{
				int iIndex[2] = { Node->vertex[0] + i, Node->vertex[1] + j };

				if (iIndex[0] == End[0] && iIndex[1] == End[1]) // ����
					IsFinish = true;

				// �������� ���
				if (iIndex[0] < 0 || iIndex[1] < 0 || iIndex[0] >= TILEEX || iIndex[1] >= TILEEY) // �� �κ� ( TILEEX, TILEEY ���� �۾ƾ� ��)
					continue;
				if (i == 0 && j == 0) // �ڱ� �ڽ�
					continue;
				if (true == Closelist[iIndex[0]][iIndex[1]]) // ���� ����
					continue;
				if (0 != pVector[iIndex[0] + iIndex[1] * TILEEX]->byOption &&
					4 != pVector[iIndex[0] + iIndex[1] * TILEEX]->byOption)
					continue;

				G = Node->G + ((i * j == 0) ? 10 : 14);
				H = (abs(End[0] - iIndex[0]) + abs(End[1] - iIndex[1])) * 10;

				if (0 != G_Openlist[iIndex[0]][iIndex[1]]) // ���� ������ ��
				{
					if (G_Openlist[iIndex[0]][iIndex[1]] > G) // ���� G���� ���� G������ �۴ٸ�,
					{
						NodeArr[iIndex[0]][iIndex[1]]->pNode = Node; // �θ� ����
						NodeArr[iIndex[0]][iIndex[1]]->G = G;
						G_Openlist[iIndex[0]][iIndex[1]] = G; // G�� ����

						p.push(make_pair(G + H, NodeArr[iIndex[0]][iIndex[1]])); // ���� ����� F ���� �����ϴ� ��

						break;
					}
				}
				else // ���Ӱ� �߰��� ����
				{
					NODE* pNode = new NODE(G, H, iIndex, Node); // �߰��� ������ �簢���鿡�� ���� ��带 �θ�簢���̶�� �����մϴ� 
					Deletelist.push_back(pNode);

					p.push(make_pair(G + H, pNode)); // �׸��� �� ������ �� �ִ� �簢������ ������Ͽ� �߰��մϴ�.
					G_Openlist[pNode->vertex[0]][pNode->vertex[1]] = G;
					NodeArr[pNode->vertex[0]][pNode->vertex[1]] = pNode;
				}
			}
		}
		// �ٽô� �� �ʿ䰡 ���� �簢������ ����� ������Ͽ� �߰��մϴ�.
		Closelist[Node->vertex[0]][Node->vertex[1]] = true;
	}

	NODE* pTemp = p.top().second; // ��ǥ ����( �ε����� End[2] �� ����)
	m_BestList.clear();

	int iCount = 0;
	while (pTemp->pNode != nullptr)
	{
		if (iCount != 0)			// ������ ������ ���� �ε����� ��ǥ�������� �ִ°��� ����
		{
			m_BestList.push_back(pTemp->vertex[0] + pTemp->vertex[1] * TILEEX);
		}
		++iCount;
		pTemp = pTemp->pNode;
	}

	m_BestList.push_back(m_iGoalIndex);		// �ڱⰡ Ŭ���� �ε��� ������ ���°��� ����

	// �� ������ �޸� ����
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