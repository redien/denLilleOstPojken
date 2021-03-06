#ifndef SWITCH_H
#define SWITCH_H

#include "Tile.h"
#include "Tilemap.h"
#include "Collectable.h"
#include "IODevice.h"
#include "WallSwitch.h"

class Switch: public Collectable
{
private:
	Tile* m_tile;
	vector<WallSwitch*> m_targets;
	float m_cooldown;
	SoundInfo* m_switchSound;
public:
	Switch(SpriteInfo* p_spriteInfo, Tile* p_tile, GameStats* p_gameStats,
		vector<WallSwitch*> p_targets, SoundInfo* p_switchSound);
	~Switch();
	void setTargets(vector<WallSwitch*> p_targets);
	void update(float p_deltaTime, InputInfo p_inputInfo);
	void reset();
	void consume();
};

#endif