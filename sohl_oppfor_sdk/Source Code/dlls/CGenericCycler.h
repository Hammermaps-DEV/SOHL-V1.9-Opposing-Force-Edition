//
// we should get rid of all the other cyclers and replace them with this.
//
class CGenericCycler : public CCycler
{
public:
	void Spawn() { GenericCyclerSpawn((char *)STRING(pev->model), Vector(-16, -16, 0), Vector(16, 16, 72)); }
};