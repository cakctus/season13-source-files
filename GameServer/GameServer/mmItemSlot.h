#pragma once
struct _ITEM_SLOT
{
	int MinItemIndex;
	int MaxItemIndex;
	int ItemSlot;
};

class GItemSlot
{
public:
	GItemSlot();
	~GItemSlot();
	void Load(char* path);
	BOOL GetItemByLocation(int Index);
	BOOL GetItemBySlot(int Index, int Slot);
private:
	std::vector<_ITEM_SLOT> m_ItemLocation;

};
extern GItemSlot gGItemSlot;
