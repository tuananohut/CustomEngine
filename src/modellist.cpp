#include "headers/modellist.h"

ModelList::ModelList()
{
	m_ModelInfoList = nullptr;
}

ModelList::ModelList(const ModelList& other) {}

ModelList::~ModelList() {}

void ModelList::Initialize(int numModels)
{
	int i;

	m_modelCount = numModels;

	m_ModelInfoList = new ModelInfoType[m_modelCount];

	srand((unsigned int)time(NULL));

	for (i = 0; i < m_modelCount; i++)
	{
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f + 5.0f;
	}
}

void ModelList::Shutdown()
{
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = nullptr;
	}
}

int ModelList::GetModelCount()
{
	return m_modelCount;
}

void ModelList::GetData(int index, float& positionX, float& positionY, float& positionZ)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;
}