#include "tools-weapons.h"
#include "armor.h"

#define INVENTORYSIZE 1
#define HUMANHEALTH 12 //starting human health
#define HUMANATTACK 2 //starting human attack
#define THEORHEALTH 9 //starting theor health
#define THEORATTACK 3 //starting theor attack
#define LARWENHEALTH 6 //starting larwen health
#define LARWENATTACK 5 //starting larwen attack
#define HUMANFIRSTLEVEL 20 //starting human xp needed, doubles each new level
#define THEORFIRSTLEVEL 40 //starting theor xp needed, doubles each new level
#define LARWENFIRSTLEVEL 60 //starting larwen xp needed, doubles each new level

struct mainCharacter {
	int health;
	int fixedHealth;
	char *name;
	char *cls;
	int attack;
	int x;
	int y;
	char tile;
	int size_tool;
	int size_weapon;
	int size_armor;
	struct tool t_invent[INVENTORYSIZE];
	struct weapon w_invent[INVENTORYSIZE];
	struct armor a_invent[INVENTORYSIZE];
	bool tool[INVENTORYSIZE];
	bool weapon[INVENTORYSIZE];
	bool armor[INVENTORYSIZE];
	int xp;
	int nextLevel;
	int level;
};

struct monster {
	char *name;
	int health;
	int attack;
	int x;
	int y;
	char tile;
	int xp;
};

/*if(map[c->y][c->x - 1].tile == TOOL &&
		c->tool == false) {
	c->t = map[c->y][c->x - 1].t;
	c->tool = true;
	map[c->y][c->x - 1].tile = GROUND;
}
else if(map[c->y][c->x - 1].tile == WEAPON &&
					c->weapon == false) {
	c->w = map[c->y][c->x - 1].w;
	c->weapon = true;
	map[c->y][c->x - 1].tile = GROUND;
}*/
