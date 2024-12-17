#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define MAP_WIDTH 15
#define MAP_HEIGHT 10
#define MAX_ROOMS 5
#define MAX_TEXT_LENGTH 256

// Oyuncu Yapısı
typedef struct {
    int x;
    int y;
    int health;
    int mana;
    int gold; // Altın
    char armor[20];
    char weapon[20];
    int weaponDamage; // Direkt verilen hasar
    int armorDamageReduction;
    int potions;
    int manaPotions;
    char inventory[20][50];
    int inventoryCount;
    char recentDrops[256];
    int poisonEffectTurns; // Zehir efekti
    int hasDragonsFury;    // Dragon's Fury yeteneğine sahip mi?
} Player;

// Canavar Yapısı
typedef struct {
    const char *name;
    const char **asciiArt;
    int health;
    int damage;
    int poisonEffectTurns;
    int isDefeated;
} Monster;

// Prototipler
// Map Fonksiyonları
char** initializeMap(int width, int height, int *entryX, int *entryY, int *exitX, int *exitY, int currentRoom);
void printMap(char **map, int width, int height, Player player, const char *roomText);
int movePlayer(Player *player, char input, char **map, int width, int height, int exitX, int exitY, int *currentRoom);
void openMarket(Player *player);
void battle(Player *player, Monster *monster);
int saveGame(Player *player, int currentRoom, char **map, int width, int height, const char *filename);
int loadGame(Player *player, int *currentRoom, char ***map, int *width, int *height, const char *filename);
void cleanMap(char **map, int width, int height);
// Save ve Load Fonksiyonları
int saveGame(Player *player, int currentRoom, char **map, int width, int height, const char *saveFile);
int loadGame(Player *player, int *currentRoom, char ***map, int *width, int *height, const char *saveFile);
void cleanMap(char **map, int width, int height);
void placePlayerOnMap(char **map, Player *player);

// Oyuncu Fonksiyonları
void inventoryMenu(Player *player);
void battle(Player *player, Monster *monster);
void addItemDrop(Player *player, Monster *monster);
void useSkill(Player *player, Monster *monster);
void openMarket(Player *player);
void clearInputBuffer();
void displayWinScreen();

// Canavar ve Savaş Fonksiyonları
void battle(Player *player, Monster *monster);
void addItemDrop(Player *player, Monster *monster);
void useSkill(Player *player, Monster *monster);

// Market Fonksiyonları
void openMarket(Player *player);

// Yardımcı Fonksiyonlar
void clearInputBuffer();
void displayWinScreen();

#endif // MAIN_H
