#include "genericmodel.h"

class CPropBushs : public CGenericModel
{
public:
	void Spawn(void);
	void Precache(void);
	void KeyValue(KeyValueData* pkvd);

	string_t m_isBush;
};
