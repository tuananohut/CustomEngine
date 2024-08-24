#ifndef _MODELLIST_H_
#define _MODELLIST_H_

#include <stdlib.h>
#include <time.h>

class ModelList
{
private:
	struct ModelInfoType
	{
		float positionX;
		float positionY;
		float positionZ;
	};

public:
	ModelList();
	ModelList(const ModelList&);
	~ModelList();

	void Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;
};

#endif