#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include "characters.h"

#define NAME_LENGTH 20
#define MAP_X 100
#define MAP_Y 40
#define DISTANCE 6
#define HEALTH 10
#define WALL '='
#define PLAYER '@'
#define GROUND '.'
#define STRUCTURE 'x'
#define TOOL 't'
#define WEAPON 'w'
#define ARMOR 'a'
#define GOBLIN 'g'
#define ORC 'O'
#define DEMON 'D'
#define CONSTX 16
#define CONSTY 8
#define FIRSTLEVEL 10

struct tiles {
		char tile;
		int x;
		int y;
		int durability;
		struct tool t;
		struct weapon w;
		struct monster m;
		struct armor a;
		bool tool;
		bool weapon;
		bool armor;
		bool monster;
		bool movedTo;
};
struct monster initMonster() {
	struct monster m;
	int x = rand() % 3;
	switch(x) {
		case 0:
			m.name = "GOBLIN";
			m.health = 10;
			m.attack = 2;
			m.tile = GOBLIN;
			m.xp = 3;
			break;
		case 1:
			m.name = "ORC";
			m.health = 10;
			m.attack = 5;
			m.tile = ORC;
			m.xp = 30;
			break;
		case 2:
			m.name = "DEMON";
			m.health = 40;
			m.attack = 1;
			m.tile = DEMON;
			m.xp = 7;
			break;
		default:
			//make default monster, or don't and create ghost monster
			break;
	}
	return m;
}
int nextLevel(int next) {
	next += next;
	return next;
}
void attackPlayer(struct mainCharacter *c, struct tiles map[][MAP_X], int x, int y) {
	if(c->armor[0]) {
		c->health -= (map[y][x].m.attack - c->a_invent[0].reduction);
		c->a_invent[0].durability--;
		if(c->a_invent[0].durability == 0) {
			c->armor[0] = false;
			c->size_armor--;
		}
	}
	else {
		c->health -= map[y][x].m.attack;
	}
	if(c->weapon[0]) {
		map[y][x].m.health -= c->attack + c->w_invent[0].attack;
		c->w_invent[0].durability--;
		if(c->w_invent[0].durability == 0) {
			c->weapon[0] = false;
			c->size_weapon--;
		}
	}
	else {
		map[y][x].m.health -= c->attack;
	}
	if(c->health <= 0) {
		clear();
		mvprintw(20, 20, "GAME OVER");
		refresh();
		sleep(5);
		endwin();
		exit(1);
	}
	else if(map[y][x].m.health <= 0) {
		c->xp += map[y][x].m.xp;
		if(c->xp > c->nextLevel) {
			c->fixedHealth += 2;
			c->nextLevel = nextLevel(c->nextLevel);
			c->level++;
		}
		map[y][x].monster = false;
		map[y][x].tile = GROUND;
	}
}
void moveMonster(struct mainCharacter *c, struct tiles map[][MAP_X], int x, int y) {
	if((x > c->x && x < c->x + DISTANCE) && (y > c->y && y < c->y + DISTANCE) && map[y][x].monster == true) { //player in lower left
		if(c->x == x - 1 && c->y == y - 1) {
			attackPlayer(c, map, x, y);
		}
		else if(map[y - 1][x - 1].tile == GROUND &&
				map[y][x].movedTo != true) {
			map[y - 1][x - 1].m = map[y][x].m;
			map[y - 1][x - 1].movedTo = true;
			map[y - 1][x - 1].monster = true;
			map[y - 1][x - 1].tile = map[y - 1][x - 1].m.tile;
			map[y - 1][x - 1].m.x = x - 1;
			map[y - 1][x - 1].m.y = y - 1;
			map[y][x].monster = false;
			if(map[y][x].tool != true && map[y][x].weapon != true) {
				map[y][x].tile = GROUND;
			}
		}
	}
	else if((x < c->x && x > c->x - DISTANCE) && (y > c->y && y < c->y + DISTANCE) && map[y][x].monster == true) { //player in lower right
		if(c->x == x + 1 && c->y == y - 1) {
			attackPlayer(c, map, x, y);
		}
		else if(map[y - 1][x + 1].tile == GROUND &&
				map[y][x].movedTo != true) {
			map[y - 1][x + 1].m = map[y][x].m;
			map[y - 1][x + 1].movedTo = true;
			map[y - 1][x + 1].monster = true;
			map[y - 1][x + 1].tile = map[y - 1][x + 1].m.tile;
			map[y - 1][x + 1].m.x = x + 1;
			map[y - 1][x + 1].m.y = y - 1;
			map[y][x].monster = false;
			if(map[y][x].tool != true && map[y][x].weapon != true) {
				map[y][x].tile = GROUND;
			}
		}
	}
	else if((x > c->x && x < c->x + DISTANCE) && (y < c->y && y > c->y - DISTANCE) && map[y][x].monster == true) { //player in upper left
		if(c->x == x - 1 && c->y == y + 1) {
			attackPlayer(c, map, x, y);
		}
		else if(map[y + 1][x - 1].tile == GROUND &&
				map[y][x].movedTo != true) {
			map[y + 1][x - 1].m = map[y][x].m;
			map[y + 1][x - 1].movedTo = true;
			map[y + 1][x - 1].monster = true;
			map[y + 1][x - 1].tile = map[y + 1][x - 1].m.tile;
			map[y + 1][x - 1].m.x = x - 1;
			map[y + 1][x - 1].m.y = y + 1;
			map[y][x].monster = false;
			if(map[y][x].tool != true && map[y][x].weapon != true) {
				map[y][x].tile = GROUND;
			}
		}
	}
	else if((x < c->x && x > c->x - DISTANCE) && (y < c->y && y > c->y - DISTANCE) && map[y][x].monster == true) { //player in upper right
		if(c->x == x + 1 && c->y == y + 1) {
			attackPlayer(c, map, x, y);
		}
		else if(map[y + 1][x + 1].tile == GROUND &&
				map[y][x].movedTo != true) {
			map[y + 1][x + 1].m = map[y][x].m;
			map[y + 1][x + 1].movedTo = true;
			map[y + 1][x + 1].monster = true;
			map[y + 1][x + 1].tile = map[y + 1][x + 1].m.tile;
			map[y + 1][x + 1].m.x = x + 1;
			map[y + 1][x + 1].m.y = y + 1;
			map[y][x].monster = false;
			if(map[y][x].tool != true && map[y][x].weapon != true) {
				map[y][x].tile = GROUND;
			}
		}
	}
}
void initCharacter(struct mainCharacter *c) {
	int i;
	c->attack = 10;
	c->health = HEALTH;
	c->fixedHealth = HEALTH;
	c->tile = PLAYER;
	c->xp = 0;
	c->level = 1;
	c->nextLevel = FIRSTLEVEL;
	for(i = 0; i < INVENTORYSIZE; i++) {
		c->tool[i] = false;
		c->weapon[i] = false;
		c->armor[i] = false;
	}
	c->size_tool = 0;
	c->size_weapon = 0;
	c->size_armor = 0;
	//xy character starting positions
	c->x = rand() % MAP_X;
	if(c->x <= 0) {
			c->x += 5;
	}
	if(c->x >= MAP_X - 1) {
			c->x -= 5;
	}
	c->y = rand() % MAP_Y;
	if(c->y <= 0) {
			c->y += 5;
	}
	if(c->y >= MAP_Y - 1) {
			c->y -= 5;
	}
}
void initTool(struct tiles map[][MAP_X], int x, int y, char id) {
	int z = rand() % 10;
	switch(z) {
		case 0:
		case 1:
		case 2:
		case 3:
			map[y][x].t.name = "Stone PickAxe";
			map[y][x].t.durability = (rand() % 19) + 1;
			map[y][x].t.x = x;
			map[y][x].t.y = y;
			map[y][x].t.id = id;
			map[y][x].tool = true;
			map[y][x].weapon = false;
			map[y][x].tile = TOOL;
			break;
		case 4:
		case 5:
		case 6:
			map[y][x].t.name = "Silver PickAxe";
			map[y][x].t.durability = (rand() % 19) + 20;
			map[y][x].t.x = x;
			map[y][x].t.y = y;
			map[y][x].t.id = id;
			map[y][x].tool = true;
			map[y][x].weapon = false;
			map[y][x].tile = TOOL;
			break;
		case 7:
		case 8:
			map[y][x].t.name = "Golden PickAxe";
			map[y][x].t.durability = (rand() % 19) + 40;
			map[y][x].t.x = x;
			map[y][x].t.y = y;
			map[y][x].t.id = id;
			map[y][x].tool = true;
			map[y][x].weapon = false;
			map[y][x].tile = TOOL;
			break;
		case 9:
			map[y][x].t.name = "Diamond PickAxe";
			map[y][x].t.durability = (rand() % 19) + 60;
			map[y][x].t.x = x;
			map[y][x].t.y = y;
			map[y][x].t.id = id;
			map[y][x].tool = true;
			map[y][x].weapon = false;
			map[y][x].tile = TOOL;
			break;
	}
}
void initWeapon(struct tiles map[][MAP_X], int x, int y, char id) {
	map[y][x].w.name = "Sword";
	map[y][x].w.durability = (rand() % 19) + 1;
	map[y][x].w.attack = (rand() % 9) + 1;
	map[y][x].w.id = id;
	map[y][x].w.x = x;
	map[y][x].w.y = y;
	map[y][x].tool = false;
	map[y][x].weapon = true;
	map[y][x].tile = WEAPON;
}
void initArmor(struct tiles map[][MAP_X], int x, int y, char id) {
	map[y][x].a.name = "Chestplate";
	map[y][x].a.durability = (rand() % 19) + 1;
	map[y][x].a.reduction = (rand() % 3) + 1;
	map[y][x].a.id = id;
	map[y][x].armor = true;
	map[y][x].tool = false;
	map[y][x].weapon = false;
	map[y][x].tile = ARMOR;
}
void pickUpItem(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	if(map[c->y][c->x].tool == true && c->size_tool != INVENTORYSIZE) {
		int i, spot = -1;
		for(i = INVENTORYSIZE - 1; i >= 0; i--) {
			if(!c->tool[i]) {
				spot = i;
			}
		}
		//c->t = map[c->y][c->x].t;
		if(spot != -1) {
			c->t_invent[spot] = map[c->y][c->x].t;
			c->size_tool++;
			c->tool[spot] = true;
			map[c->y][c->x].tool = false;
			map[c->y][c->x].tile = GROUND;
		}
	}
	else if(map[c->y][c->x].weapon == true && c->size_weapon != INVENTORYSIZE) {
		int i, spot = -1;
		for(i = INVENTORYSIZE - 1; i >= 0; i--) {
			if(!c->weapon[i]) {
				spot = i;
			}
		}
		if(spot != -1) {
			c->w_invent[spot] = map[c->y][c->x].w;
			c->size_weapon++;
			c->weapon[spot] = true;
			map[c->y][c->x].weapon = false;
			map[c->y][c->x].tile = GROUND;
		}
	}
	else if(map[c->y][c->x].armor == true && c->size_armor != INVENTORYSIZE) {
		int i, spot = -1;
		for(i = INVENTORYSIZE - 1; i >= 0; i--) {
			if(!c->armor[i]) {
				spot = i;
			}
		}
		if(spot != -1) {
			c->a_invent[spot] = map[c->y][c->x].a;
			c->size_armor++;
			c->armor[spot] = true;
			map[c->y][c->x].armor = false;
			map[c->y][c->x].tile = GROUND;
		}
	}
}
void shrinkToolInventory(struct mainCharacter *c, struct tiles map[][MAP_X], int spot) {
	int i;
	for(i = spot; i < INVENTORYSIZE - 1; i++) {
		if(c->tool[i + 1]) {
			c->t_invent[i] = c->t_invent[i + 1];
			c->tool[i] = true;
			c->tool[i + 1] = false;
		}
	}
}
void shrinkWeaponInventory(struct mainCharacter *c, struct tiles map[][MAP_X], int spot) {
	int i;
	for(i = spot; i < INVENTORYSIZE - 1; i++) {
		if(c->weapon[i + 1]) {
			c->w_invent[i] = c->w_invent[i + 1];
			c->weapon[i] = true;
			c->weapon[i + 1] = false;
		}
	}
}
void shrinkArmorInventory(struct mainCharacter *c, struct tiles map[][MAP_X], int spot) {
	int i;
	for(i = spot; i < INVENTORYSIZE - 1; i++) {
		if(c->armor[i + 1]) {
			c->a_invent[i] = c->a_invent[i + 1];
			c->armor[i] = true;
			c->armor[i + 1] = false;
		}
	}
}
void dropItem(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	char a = getch();
	bool toolFound = false, weaponFound = false, armorFound = false;
	int i, spot;
	for(i = 0; i < INVENTORYSIZE; i++) {
		if(c->t_invent[i].id == a) {
			toolFound = true;
			spot = i;
		}
		else if(c->w_invent[i].id == a) {
			weaponFound = true;
			spot = i;
		}
		else if(c->a_invent[i].id == a) {
			armorFound = true;
			spot = i;
		}
	}
	if(toolFound && c->tool[spot] && map[c->y][c->x].tool == false) {
		map[c->y][c->x].t = c->t_invent[spot];
		map[c->y][c->x].tile = TOOL;
		map[c->y][c->x].tool = true;
		c->tool[spot] = false;
		c->size_tool--;
		if(spot != 0 && spot != INVENTORYSIZE - 1 && c->tool[spot + 1]) {
			shrinkToolInventory(c, map, spot);
		}
	}
	else if(weaponFound && c->weapon[spot] && map[c->y][c->x].weapon == false) {
		map[c->y][c->x].w = c->w_invent[spot];
		map[c->y][c->x].tile = WEAPON;
		map[c->y][c->x].weapon = true;
		c->weapon[spot] = false;
		c->size_weapon--;
		if(spot != 0 && spot != INVENTORYSIZE - 1 && c->weapon[spot + 1]) {
			shrinkWeaponInventory(c, map, spot);
		}
	}
	else if(armorFound && c->armor[spot] && map[c->y][c->x].armor == false) {
		map[c->y][c->x].a = c->a_invent[spot];
		map[c->y][c->x].tile = ARMOR;
		map[c->y][c->x].armor = true;
		c->armor[spot] = false;
		c->size_armor--;
		if(spot != 0 && spot != INVENTORYSIZE - 1 && c->armor[spot + 1]) {
			shrinkArmorInventory(c, map, spot);
		}
	}
}
void equipItem(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	char a = getch();
	bool toolFound = false, weaponFound = false, armorFound = false;;
	int i, spot;
	for(i = 1; i < INVENTORYSIZE; i++) {
		if(c->t_invent[i].id == a) {
			toolFound = true;
			spot = i;
		}
		else if(c->w_invent[i].id == a) {
			weaponFound = true;
			spot = i;
		}
		else if(c->a_invent[i].id == a) {
			armorFound = true;
			spot = i;
		}
	}
	if(toolFound && c->tool[spot] && c->tool[0]) {
		struct tool t = c->t_invent[spot];
		c->t_invent[spot] = c->t_invent[0];
		c->t_invent[0] = t;
	}
	else if(toolFound && c->tool[spot] && !c->tool[0]) {
		c->t_invent[0] = c->t_invent[spot];
		c->tool[spot] = false;
		c->tool[0] = true;
		if(spot != INVENTORYSIZE - 1 && c->tool[spot + 1]) {
			shrinkToolInventory(c, map, spot);
		}
	}
	else if(weaponFound && c->weapon[spot] && c->weapon[0]) {
		struct weapon w = c->w_invent[spot];
		c->w_invent[spot] = c->w_invent[0];
		c->w_invent[0] = w;
	}
	else if(weaponFound && c->weapon[spot] && !c->weapon[0]) {
		c->w_invent[0] = c->w_invent[spot];
		c->weapon[spot] = false;
		c->weapon[0] = true;
		if(spot != INVENTORYSIZE - 1 && c->weapon[spot + 1]) {
			shrinkWeaponInventory(c, map, spot);
		}
	}
	else if(armorFound && c->armor[spot] && c->armor[0]) {
		struct armor a = c->a_invent[spot];
		c->a_invent[spot] = c->a_invent[0];
		c->a_invent[0] = a;
	}
	else if(armorFound && c->armor[spot] && !c->armor[0]) {
		c->a_invent[0] = c->a_invent[spot];
		c->armor[spot] = false;
		c->armor[0] = true;
		if(spot != INVENTORYSIZE - 1 && c->armor[spot + 1]) {
			shrinkArmorInventory(c, map, spot);
		}
	}
}
void movePlayerWest(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	if(map[c->y][c->x - 1].tile != WALL &&
			map[c->y][c->x - 1].tile != STRUCTURE) {
		//map[c->y][c->x].tile = GROUND;
		c->x--;
		//map[c->y][c->x].tile = c->tile;
	}
	else if(map[c->y][c->x - 1].tile == STRUCTURE &&
						c->tool[0] != false) {
		map[c->y][c->x - 1].durability--;
		c->t_invent[0].durability--;
		if(map[c->y][c->x - 1].durability == 0)
			map[c->y][c->x - 1].tile = GROUND;
		if(c->t_invent[0].durability == 0) {
			c->tool[0] = false;
			c->size_tool--;
		}
	}
}
void movePlayerEast(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	if(map[c->y][c->x + 1].tile != WALL &&
			map[c->y][c->x + 1].tile != STRUCTURE) {
		//map[c->y][c->x].tile = GROUND;
		c->x++;
		//map[c->y][c->x].tile = c->tile;
	}
	else if(map[c->y][c->x + 1].tile == STRUCTURE &&
						c->tool[0] != false) {
		map[c->y][c->x + 1].durability--;
		c->t_invent[0].durability--;
		if(map[c->y][c->x + 1].durability == 0)
			map[c->y][c->x + 1].tile = GROUND;
		if(c->t_invent[0].durability == 0) {
			c->tool[0] = false;
			c->size_tool--;
		}
	}
}
void movePlayerNorth(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	if(map[c->y + 1][c->x].tile != WALL &&
			map[c->y + 1][c->x].tile != STRUCTURE) {
		//map[c->y][c->x].tile = GROUND;
		c->y++;
		//map[c->y][c->x].tile = c->tile;
	}
	else if(map[c->y + 1][c->x].tile == STRUCTURE &&
						c->tool[0] != false) {
		map[c->y + 1][c->x].durability--;
		c->t_invent[0].durability--;
		if(map[c->y + 1][c->x].durability == 0)
			map[c->y + 1][c->x].tile = GROUND;
		if(c->t_invent[0].durability == 0) {
			c->tool[0] = false;
			c->size_tool--;
		}
	}
}
void movePlayerSouth(struct mainCharacter *c, struct tiles map[][MAP_X]) {
	if(map[c->y - 1][c->x].tile != WALL &&
			map[c->y - 1][c->x].tile != STRUCTURE) {
		//map[c->y][c->x].tile = GROUND;
		c->y--;
		//map[c->y][c->x].tile = c->tile;
	}
	else if(map[c->y - 1][c->x].tile == STRUCTURE &&
						c->tool[0] != false) {
		map[c->y - 1][c->x].durability--;
		c->t_invent[0].durability--;
		if(map[c->y - 1][c->x].durability == 0)
			map[c->y - 1][c->x].tile = GROUND;
		if(c->t_invent[0].durability == 0) {
			c->tool[0] = false;
			c->size_tool--;
		}
	}
}
void smallStructure(struct tiles map[][MAP_X]) {
	int x = rand() % MAP_X;
	int y = rand() % (MAP_Y - 4);
	if(x + 2 < MAP_X && x - 2 > 0 && y + 3 < MAP_Y && y - 3 > 0) {
		map[y][x - 1].tile = STRUCTURE;
		map[y][x + 1].tile = STRUCTURE;
		map[y + 1][x + 1].tile = STRUCTURE;
		map[y + 1][x - 1].tile = STRUCTURE;
		map[y + 2][x + 1].tile = STRUCTURE;
		map[y + 2][x - 1].tile = STRUCTURE;
		map[y + 2][x].tile = STRUCTURE;
	}
}
void mediumStructure(struct tiles map[][MAP_X]) {
	int x = rand() % MAP_X;
	int y = rand() % (MAP_Y - 7);
	if(x + 5 < MAP_X && x - 5 > 0 && y + 7 < MAP_Y && y - 2 > 0 && map[y][x].tile != STRUCTURE) {
		map[y][x - 1].tile = STRUCTURE;
		map[y][x - 2].tile = STRUCTURE;
		map[y][x - 3].tile = STRUCTURE;
		map[y + 1][x - 3].tile = STRUCTURE;
		map[y + 2][x - 3].tile = STRUCTURE;
		map[y + 3][x - 3].tile = STRUCTURE;
		map[y + 4][x - 3].tile = STRUCTURE;
		map[y][x + 1].tile = STRUCTURE;
		map[y][x + 2].tile = STRUCTURE;
		map[y][x + 3].tile = STRUCTURE;
		map[y + 1][x + 3].tile = STRUCTURE;
		map[y + 2][x + 3].tile = STRUCTURE;
		map[y + 3][x + 3].tile = STRUCTURE;
		map[y + 4][x + 3].tile = STRUCTURE;
		map[y + 4][x + 2].tile = STRUCTURE;
		map[y + 4][x + 1].tile = STRUCTURE;
		map[y + 4][x].tile = STRUCTURE;
		map[y + 4][x - 1].tile = STRUCTURE;
		map[y + 4][x - 2].tile = STRUCTURE;
		map[y + 4][x - 3].tile = STRUCTURE;
	}
}
void printInventory(struct mainCharacter *c, int constX, int constY) {
	int y = 0, i;
	mvprintw(y++, constX * 2 + 3, "-----------Inventory-----------");
	mvprintw(y++, constX * 2 + 3, "Tools:");
	if(c->tool[0]) {
		mvprintw(y++, constX * 2 + 3, "EQ: %c - %s | D - %d", c->t_invent[0].id, c->t_invent[0].name, c->t_invent[0].durability);
	}
	for(i = 1; i < INVENTORYSIZE; i++) {
		if(c->tool[i]) {
			mvprintw(y++, constX * 2 + 3, "    %c - %s | D - %d", c->t_invent[i].id, c->t_invent[i].name, c->t_invent[i].durability);
		}
	}
	mvprintw(y++, constX * 2 + 3, "Weapons:");
	if(c->weapon[0]) {
		mvprintw(y++, constX * 2 + 3, "EQ: %c - %s | A - %d | D - %d", c->w_invent[0].id, c->w_invent[0].name, c->w_invent[0].attack, c->w_invent[0].durability);
	}
	for(i = 1; i < INVENTORYSIZE; i++) {
		if(c->weapon[i]) {
			mvprintw(y++, constX * 2 + 3, "    %c - %s | A - %d | D - %d", c->w_invent[i].id, c->w_invent[i].name, c->w_invent[i].attack, c->w_invent[i].durability);
		}
	}
	mvprintw(y++, constX * 2 + 3, "Armor:");
	if(c->armor[0]) {
		mvprintw(y++, constX * 2 + 3, "EQ: %c - %s | R - %d | D - %d", c->a_invent[0].id, c->a_invent[0].name, c->a_invent[0].reduction, c->a_invent[0].durability);
	}
	for(i = 1; i < INVENTORYSIZE; i++) {
		if(c->armor[i]) {
			mvprintw(y++, constX * 2 + 3, "    %c - %s | R - %d | D - %d", c->a_invent[i].id, c->a_invent[i].name, c->a_invent[i].reduction, c->a_invent[i].durability);
		}
	}
}

int main(int argc, const char * argv[]) {
	int x = 0, y = 0, yCord = 0, xCord = 0, max_x = 0, max_y = 0, i, j, rest = 0;
	char id = 33;
	srand((unsigned)time(NULL));
	struct tiles map[MAP_Y][MAP_X];
	struct mainCharacter character;
	char c[20];
	initscr();
	noecho();
	curs_set(FALSE);
	mvprintw(0, 0, "What's your name? ");
	getnstr(c, sizeof(c) - 1);
	initCharacter(&character);
	character.name = c;
	//creates map tiles
	for(i = 0; i < MAP_Y; i++) {
		for(j = 0; j < MAP_X; j++) {
			int m = rand() % 500;
			int r = rand() % 200;
			if(i == MAP_Y - 1 || j == MAP_X - 1 || i == 0 || j == 0){
				map[i][j].tile = WALL;
				map[i][j].tool = false;
				map[i][j].weapon = false;
				map[i][j].monster = false;
			}
			else if(m == 24) {
				map[i][j].m = initMonster();
				map[i][j].m.x = j;
				map[i][j].m.y = i;
				map[i][j].tile = map[i][j].m.tile;
				map[i][j].monster = true;
			}
			else if(r == 36 && id < 'z') {
				initTool(map, j, i, id++);
			}
			else if(r == 40 && id < 'z') {
				initWeapon(map, j, i, id++);
			}
			else if(r == 44 && id < 'z') {
				initArmor(map, j, i, id++);
			}
			else if(r == 48) {
				smallStructure(map);
				map[i][j].tile = GROUND;
			}
			else if(r == 60) {
				mediumStructure(map);
				map[i][j].tile = GROUND;
			}
			else {
				map[i][j].tile = GROUND;
				map[i][j].tool = false;
				map[i][j].weapon = false;
				map[i][j].armor = false;
				map[i][j].monster = false;
			}
			map[i][j].durability = (rand() % 4) + 1;
			map[i][j].x = j;
			map[i][j].y = i;
			map[i][j].movedTo = false;
		}
	}
	//game runs here | prints, getch/switch, etc...
	while(1) {
		int constX = CONSTX, constY = CONSTY;
		int xup = constX, xdown = constX, yup = constY, ydown = constY;
		int printY = 1, health = HEALTH;
		bool foundItem = false;
		//used for printing out screen | gets the x and y lenghts
		if(character.y + constY > MAP_Y - 1) {
			yup = MAP_Y - character.y - 1;
			ydown += (constY - yup);
		}
		if(character.y - constY < 0) {
			ydown = character.y;
			yup += (constY - ydown);
		}
		if(character.x + constX > MAP_X - 1) {
			xup = MAP_X - character.x - 1;
			xdown += (constX - xup);
		}
		if(character.x - constX < 0) {
			xdown = character.x;
			xup += (constX - xdown);
		}
		for(i = character.y + yup; i >= character.y - ydown; i--) {
			for(j = character.x - xdown; j < character.x + xup + 1; j++) {
				if(map[i][j].monster == true && map[i][j].movedTo == false) {
					moveMonster(&character, map, j, i);
				}
			}
		}
		clear();
		//prints screen based on x and y up/down coords
		for(i = character.y + yup; i >= character.y - ydown; i--) {
			for(j = character.x - xdown; j < character.x + xup + 1; j++) {
				if(i == character.y && j == character.x) {
					mvprintw(yCord, xCord++, "%c", character.tile);
				}
				else {
					//might mess with how monsters are printed
					if(map[i][j].tool == true) {
						map[i][j].tile = TOOL;
					}
					else if(map[i][j].weapon == true) {
						map[i][j].tile = WEAPON;
					}
					mvprintw(yCord, xCord++, "%c", map[i][j].tile);
				}
				//move monsters here...can't actually do this, some monsters will teleport, fixed with bool movedTo
				map[i][j].movedTo = false;
			}
			xCord = 0;
			mvprintw(i, j, "\n");
			yCord++;
		}
		yCord = 0;
		xCord = 0;
		mvprintw(constY * 2 + printY++, 0, "Name = %s Atk = %d Hlth = %d/%d", character.name, character.attack, character.health, character.fixedHealth);
		mvprintw(constY * 2 + printY++, 0, "Level: %d | XP = %d/%d", character.level, character.xp, character.nextLevel);
		if(character.tool[0]) {
			mvprintw(constY * 2 + printY++, 0, "Tool = %c - %s | D = %d", character.t_invent[0].id, character.t_invent[0].name, character.t_invent[0].durability);
		}
		if(character.weapon[0]) {
			mvprintw(constY * 2 + printY++, 0, "Weapon = %c - %s | A = %d | D = %d", character.w_invent[0].id, character.w_invent[0].name, character.w_invent[0].attack, character.w_invent[0].durability);
		}
		if(character.armor[0]) {
			mvprintw(constY * 2 + printY++, 0, "Armor = %c - %s | R = %d | D = %d", character.a_invent[0].id, character.a_invent[0].name, character.a_invent[0].reduction, character.a_invent[0].durability);
		}
		printInventory(&character, constX, constY);
		refresh();
		switch(getch()) {
			case 'a': //west
				movePlayerWest(&character, map);
				break;
			case 'd': //east
				movePlayerEast(&character, map);
				break;
			case 'w': //north
				movePlayerNorth(&character, map);
				break;
			case 's': //south
				movePlayerSouth(&character, map);
				break;
			case 'p': //pick up item
				pickUpItem(&character, map);
				break;
			case 'P': //drop item
				for(i = 0; i < INVENTORYSIZE; i++) {
					if(character.tool[i]) {
						foundItem = true;
					}
					else if(character.weapon[i]) {
						foundItem = true;
					}
					else if(character.armor[i]) {
						foundItem = true;
					}
				}
				if(foundItem) {
					mvprintw(constY * 2 + 5, 0, "Drop which item?");
					dropItem(&character, map);
				}
				break;
			case 'e': //equip item
				mvprintw(constY * 2 + 5, 0, "Equip which item?");
				equipItem(&character, map);
				break;
			case 'h':
				mvprintw(0, constX * 2 + 38, "--------------Help--------------");
				mvprintw(1, constX * 2 + 38, "'a' - Left");
				mvprintw(2, constX * 2 + 38, "'d' - Right");
				mvprintw(3, constX * 2 + 38, "'w' - Up");
				mvprintw(4, constX * 2 + 38, "'s' - Down");
				mvprintw(5, constX * 2 + 38, "'p' - Pick Up Item");
				mvprintw(6, constX * 2 + 38, "'P' and '(Item ID)' - Drop Item");
				mvprintw(7, constX * 2 + 38, "'e' and '(Item ID)' - Equip Item");
				refresh();
				getch();
				break;
			case '.': //for debugging
				mvprintw(0, constX * 2 + 33, "tile = %c", map[character.y][character.x].tile);
				mvprintw(1, constX * 2 + 33, "weapon = %s", map[character.y][character.x].weapon ? "true" : "false");
				mvprintw(2, constX * 2 + 33, "tool = %s", map[character.y][character.x].tool ? "true" : "false");
				mvprintw(3, constX * 2 + 33, "durability = %d", map[character.y][character.x].durability);
				refresh();
				getch();
				break;
		}
		rest++;
		if(rest % 15 == 0 && character.health < character.fixedHealth) {
			character.health++;
		}
		refresh();
		clear();
	}
	endwin();
}
