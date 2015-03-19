#include "tools-weapons.h"
#include "armor.h"

#define INVENTORYSIZE 3

struct mainCharacter {
	int health;
	int fixedHealth;
	char *name;
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
