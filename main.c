#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define MAP_WIDTH 15
#define MAP_HEIGHT 10
#define MAX_ROOMS 5
#define MAX_TEXT_LENGTH 256

// ASCII sanatını ayrı dizilerde sakla
const char *octopusArt[] = {
    "   _____     ",
    "  /     \\    ",
    " | () () |   ",
    "(_   O   _)  ",
    "   \\_-_/     "
};

const char *goblinArt[] = {
    "  ,      ,   ",
    " /(.-\"\"-.)\\  ",
    "|  \\_o_o_/ | ",
    " \\   '-'   / ",
    "  '--._.--'  "
};

const char *oldManArt[] = {
    "   _____     ",
    "  /     \\    ",
    " /  O O  \\   ",
    "|    >     | ",
    " \\___===__/  "
};

const char *slimeKingArt[] = {
    "     ^^^      ",
    "    /_#_\\     ",
    "   _____      ",
    "  _/     \\_   ",
    " /         \\  ",
    "|  >_ _<   |  ",
    " \\_________/  "
};

// Canavarları tanımlama
Monster monsters[MAX_ROOMS - 1] = {
    {"Octopus", octopusArt, 75, 10, 0, 0},
    {"Goblin", goblinArt, 100, 25, 0, 0},
    {"Old Man", oldManArt, 125, 30, 0, 0},
    {"Slime King", slimeKingArt, 200, 35, 0, 0}
};
void displayWinScreen() {
    printf("\n  _______________________\n");
    printf(" |                       |\n");
    printf(" |       YOU WIN!        |\n");
    printf(" |   Congratulations!    |\n");
    printf(" |                       |\n");
    printf(" |   Thanks for Playing  |\n");
    printf(" |_______________________|\n");
}


// Haritayı oluşturma
char** initializeMap(int width, int height, int *entryX, int *entryY, int *exitX, int *exitY, int currentRoom) {
    char **map = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++) {
        map[i] = (char*)malloc(width * sizeof(char));
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                map[i][j] = '#'; // Walls
            } else {
                map[i][j] = '.'; // Empty space
            }
        }
    }

    // Entry and exit points
    *entryX = 0; *entryY = height / 2;
    *exitX = width - 1; *exitY = height / 2;
    map[*entryY][*entryX] = 'E';
    map[*exitY][*exitX] = 'X';

    // Add market in the room before the boss
    if (currentRoom == MAX_ROOMS - 1) {
        map[height / 2][width / 2] = 'M'; // Market symbol
    }

    return map;
}

// Haritayı ekrana yazdırma
void printMap(char **map, int width, int height, Player player, const char *roomText) {
    printf("Map:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Oyuncunun pozisyonu
            if (i == player.y && j == player.x) {
                printf("P "); // Oyuncu konumu
            } else {
                printf("%c ", map[i][j]); // Harita verisi
            }
        }
        printf("\n");
    }

    // Diğer bilgiler
    printf("\nRoom Description:\n%s\n", roomText);
    printf("\nStatus:\nHealth: %d/100\nMana: %d/100\nGold: %d\n", player.health, player.mana, player.gold);
}


// Metni dosyadan yükleme
void loadRoomText(const char *filename, char *buffer) {
    FILE *file = fopen(filename, "r");
    if (file) {
        buffer[0] = '\0'; // Buffer'ı temizle
        char line[MAX_TEXT_LENGTH]; // Geçici bir tampon satır okumak için

        while (fgets(line, sizeof(line), file)) {
            // Eğer tampon boyutunu aşmak üzereyse, kalan yer kadarını ekle ve çık
            if (strlen(buffer) + strlen(line) >= MAX_TEXT_LENGTH - 1) {
                strncat(buffer, line, MAX_TEXT_LENGTH - strlen(buffer) - 1);
                break;
            }
            // Satırı buffer'a ekle
            strncat(buffer, line, MAX_TEXT_LENGTH - strlen(buffer) - 1);
        }

        fclose(file);
    } else {
        strcpy(buffer, "Room description not available.");
    }
}


void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Tampondaki karakterleri temizler
}
void inventoryMenu(Player *player) {
    while (1) {
        printf("\nInventory:\n");

        for (int i = 0; i < player->inventoryCount; i++) {
            printf("%d - %s\n", i + 1, player->inventory[i]);
        }

        printf("%d - Use Health Potion (%d left)\n", player->inventoryCount + 1, player->potions);
        printf("%d - Use Mana Potion (%d left)\n", player->inventoryCount + 2, player->manaPotions);
        printf("%d - Exit Inventory\n", player->inventoryCount + 3);

        printf("\nEnter your choice: ");
        int inventoryChoice;
        if (scanf("%d", &inventoryChoice) != 1 || inventoryChoice < 1 || inventoryChoice > player->inventoryCount + 3) {
            printf("\nInvalid inventory choice. Try again.\n");
            clearInputBuffer();
            continue;
        }

        if (inventoryChoice <= player->inventoryCount) {
            const char *selectedItem = player->inventory[inventoryChoice - 1];

            if (strstr(selectedItem, "Trident")) {
                strcpy(player->weapon, selectedItem);
                printf("\nYou equipped %s. Attack damage set to 25.\n", selectedItem);
            } else if (strstr(selectedItem, "Bow")) {
                strcpy(player->weapon, selectedItem);
                printf("\nYou equipped %s. Attack damage set to 35.\n", selectedItem);
            } else if (strstr(selectedItem, "Armor")) {
                player->armorDamageReduction = 15;
                strcpy(player->armor, selectedItem);
                printf("\nYou equipped %s. Damage reduction set to 15.\n", selectedItem);
            }
        } else if (inventoryChoice == player->inventoryCount + 1) {
            if (player->potions > 0 && player->health < 100) {
                player->health += 20;
                if (player->health > 100) player->health = 100;
                player->potions--;
                printf("\nYou used a health potion. Health: %d.\n", player->health);
            } else {
                printf("\nYou can't use a health potion now.\n");
            }
        } else if (inventoryChoice == player->inventoryCount + 2) {
            if (player->manaPotions > 0 && player->mana < 100) {
                player->mana += 20;
                if (player->mana > 100) player->mana = 100;
                player->manaPotions--;
                printf("\nYou used a mana potion. Mana: %d.\n", player->mana);
            } else {
                printf("\nYou can't use a mana potion now.\n");
            }
        } else {
            break;
        }
    }
}





int calculateGoldDrop(const char *monsterName) {
    if (strcmp(monsterName, "Octopus") == 0) {
        return rand() % 21 + 20; // 20-40 arası
    } else if (strcmp(monsterName, "Goblin") == 0) {
        return rand() % 26 + 75; // 75-100 arası
    } else if (strcmp(monsterName, "Old Man") == 0) {
        return rand() % 26 + 90; // 90-115 arası
    }
    return 0; // Eğer tanımlanmamış bir canavar adı verilirse
}

void openMarket(Player *player) {
    printf("\nWelcome to the Market!\n");
    printf("\n  _______________________\n");
    printf(" |                       |\n");
    printf(" |  The Merchant         |\n");
    printf(" |  'What do you need?'  |\n");
    printf(" |_______________________|\n");

    while (1) {
        printf("\nMarket Menu:\n");
        printf("1 - Dragon's Fury (220 Gold) - Instantly kill any enemy (Costs 100 mana)\n");
        printf("2 - Health Potion (15 Gold) - Restores 20 health points.\n");
        printf("3 - Mana Potion (15 Gold) - Restores 20 mana points.\n");
        printf("4 - Exit Market\n");
        printf("\nGold: %d\n", player->gold);
        printf("\nEnter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 4) {
            printf("\nInvalid choice. Try again.\n");
            clearInputBuffer();
            continue;
        }

        if (choice == 1) { // Dragon's Fury
            if (player->gold >= 220) {
                player->hasDragonsFury = 1; // Yeteneği aktif et
                player->gold -= 220;
                printf("\nYou have acquired Dragon's Fury! A mighty skill has been unlocked.\n");
            } else {
                printf("\nNot enough gold!\n");
            }
        } else if (choice == 2) { // Health Potion
            if (player->gold >= 15) {
                player->potions++;
                player->gold -= 15;
                printf("\nYou purchased a health potion!\n");
            } else {
                printf("\nNot enough gold!\n");
            }
        } else if (choice == 3) { // Mana Potion
            if (player->gold >= 15) {
                player->manaPotions++;
                player->gold -= 15;
                printf("\nYou purchased a mana potion!\n");
            } else {
                printf("\nNot enough gold!\n");
            }
        } else if (choice == 4) { // Exit Market
            break;
        }
    }
}


void addItemDrop(Player *player, Monster *monster) {
    if (monster->isDefeated) return; // Canavar zaten yenilmişse hiçbir şey yapma

    // Altın hesaplama
    int gold = calculateGoldDrop(monster->name);
    printf("Gold dropped by %s: %d\n", monster->name, gold); // Debug için

    // Mevcut altına ekleme
    player->gold += gold;
    printf("Player's total gold after adding: %d\n", player->gold); // Debug için

    // Ödül kontrolü ve ekleme
    if (strcmp(monster->name, "Octopus") == 0) {
        strcpy(player->inventory[player->inventoryCount++], "Trident of the Abyss");
        player->potions += 5;
        snprintf(player->recentDrops, sizeof(player->recentDrops),
                 "You gained: Trident of the Abyss and 5 health potions! Gold: %d", gold);
    } else if (strcmp(monster->name, "Goblin") == 0) {
        strcpy(player->inventory[player->inventoryCount++], "Goblin Bow");
        player->potions += 5;
        snprintf(player->recentDrops, sizeof(player->recentDrops),
                 "You gained: Goblin Bow and 5 health potions! Gold: %d", gold);
    } else if (strcmp(monster->name, "Old Man") == 0) {
        strcpy(player->inventory[player->inventoryCount++], "Knight Armor");
        snprintf(player->recentDrops, sizeof(player->recentDrops),
                 "You gained: Knight Armor! Gold: %d", gold);
    } else {
        snprintf(player->recentDrops, sizeof(player->recentDrops), "You gained: %d gold!", gold);
    }

    // Canavarın yenildiğini işaretle
    monster->isDefeated = 1;
}


void useSkill(Player *player, Monster *monster) {
    int skillChoice;
    printf("\nSelect your skill:\n");

    // Dragon's Fury kontrolü
    if (player->hasDragonsFury) {
        printf("1 - Dragon's Fury: Instantly kill any enemy (Cost: 100 mana)\n");
    }

    // Trident yetenekleri
    if (strcmp(player->weapon, "Trident of the Abyss") == 0) {
        printf("2 - Terror of the Seas: Deal 50 damage (Cost: 50 mana)\n");
        printf("3 - Blessing of the Seas: Heal for 40 HP (Cost: 50 mana)\n");
    }

    // Goblin Bow yetenekleri
    if (strcmp(player->weapon, "Goblin Bow") == 0) {
        printf("2 - Tremor Strike: Deal 70 damage (Cost: 70 mana)\n");
        printf("3 - Serpent's Wrath: Inflict poison (Cost: 25 mana)\n");
    }

    // Default yetenek
    if (!player->hasDragonsFury && strcmp(player->weapon, "Trident of the Abyss") != 0 &&
        strcmp(player->weapon, "Goblin Bow") != 0) {
        printf("1 - Gatling Punch: Deal 30 damage (Cost: 30 mana)\n");
    }

    printf("Enter your choice: ");
    if (scanf("%d", &skillChoice) != 1) {
        printf("\nInvalid skill choice.\n");
        clearInputBuffer();
        return;
    }

    // Dragon's Fury kontrolü
    if (player->hasDragonsFury && skillChoice == 1) {
        if (player->mana >= 100) {
            printf("\nYou used Dragon's Fury! 'Unleashing the power of the dragon!'\n");
            player->mana -= 100;
            monster->health = 0; // Canavarı direkt öldür
            printf("\nThe %s has been obliterated!\n", monster->name);
            monster->isDefeated = 1; // Canavarın yenildiğini işaretle
            addItemDrop(player, monster); // Ödülleri ekle
        } else {
            printf("\nNot enough mana!\n");
        }
        return;
    }

    // Trident yetenekleri
    if (strcmp(player->weapon, "Trident of the Abyss") == 0) {
        if (skillChoice == 2 && player->mana >= 50) {
            printf("\nYou used Terror of the Seas! 'The waves tremble with your might!'\n");
            player->mana -= 50;
            monster->health -= 50;
        } else if (skillChoice == 3 && player->mana >= 50) {
            printf("\nYou used Blessing of the Seas! 'The ocean heals your wounds!'\n");
            player->mana -= 50;
            player->health += 40;
            if (player->health > 100) player->health = 100;
        } else {
            printf("\nNot enough mana!\n");
        }
        return;
    }

    // Goblin Bow yetenekleri
    if (strcmp(player->weapon, "Goblin Bow") == 0) {
        if (skillChoice == 2 && player->mana >= 70) {
            printf("\nYou used Tremor Strike! 'The ground shakes violently!'\n");
            player->mana -= 70;
            monster->health -= 70;
        } else if (skillChoice == 3 && player->mana >= 25) {
            printf("\nYou used Serpent's Wrath! 'The poison creeps through their veins!'\n");
            player->mana -= 25;
            monster->poisonEffectTurns = 3; // Zehir etkisi 3 tur devam eder
            printf("\n%s is now poisoned for 3 turns!\n", monster->name);
        } else {
            printf("\nNot enough mana!\n");
        }
        return;
    }

    // Default yetenek
    if (skillChoice == 1 && player->mana >= 30) {
        printf("\nYou used Gatling Punch! 'A barrage of punches lands on your enemy!'\n");
        player->mana -= 30;
        monster->health -= 30;
    } else {
        printf("\nNot enough mana!\n");
    }

    // Zehir etkisi kontrolü
    if (monster->poisonEffectTurns > 0) {
        printf("\nThe %s suffers poison damage!\n", monster->name);
        monster->health -= 10;
        monster->poisonEffectTurns--;

        if (monster->health <= 0) {
            printf("\nThe poison has killed the %s!\n", monster->name);
            monster->isDefeated = 1;
            addItemDrop(player, monster);
        }
    }
}





// Savaş sistemini çalıştırma


void battle(Player *player, Monster *monster) {
    if (monster->isDefeated) return;

    int choice;
    while (monster->health > 0 && player->health > 0) {
        // Canavar ASCII sanatını göster
        printf("\n  _______________________\n");
        printf(" |                       |\n");
        printf(" |      (%d HP)         |\n", monster->health > 0 ? monster->health : 0);
        for (int i = 0; i < 5; i++) {
            char buffer[22] = {0};
            snprintf(buffer, 22, "%-21s", monster->asciiArt[i]);
            printf(" | %s|\n", buffer);
        }
        printf(" |_______________________|\n");

        // Battle Menüsü
        printf("\nBattle Menu:\n");
        printf("1 - Attack\n");
        printf("2 - Defence\n");
        printf("3 - Skill\n");
        printf("4 - Inventory\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1 || (choice < 1 || choice > 4)) {
            printf("\nInvalid choice. Try again.\n");
            clearInputBuffer();
            continue;
        }

        if (choice == 1) {
            // Silaha göre hasar belirleme
            int damage = 10; // Default damage
            if (strstr(player->weapon, "Trident")) {
                damage = 25;
            } else if (strstr(player->weapon, "Bow")) {
                damage = 35;
            }
            printf("\nYou attack the %s for %d damage!\n", monster->name, damage);
            monster->health -= damage;
        } else if (choice == 2) {
            // Savunma mesajı
            printf("\nYou brace yourself for the %s's attack!\n", monster->name);
        } else if (choice == 3) {
            // Skill kullanımı
            useSkill(player, monster);
        } else if (choice == 4) {
            // Envanter menüsüne erişim
            inventoryMenu(player);
        }

        // Zehir etkisini kontrol et
        if (monster->poisonEffectTurns > 0) {
            printf("\nThe %s suffers poison damage!\n", monster->name);
            monster->health -= 10; // Zehir hasarı
            monster->poisonEffectTurns--;

            if (monster->health <= 0) { // Zehir hasarıyla canavar ölürse
                printf("\nThe poison has killed the %s!\n", monster->name);
                addItemDrop(player, monster); // Ödülleri ver
                break;
            }
        }

        // Canavarın saldırısı
        if (monster->health > 0) {
            int actualDamage = monster->damage - player->armorDamageReduction;
            actualDamage = actualDamage < 0 ? 0 : actualDamage; // Negatif hasar olmaz
            printf("\nThe %s attacks you for %d damage (after armor reduction)!\n", monster->name, actualDamage);
            player->health -= actualDamage;

            if (player->health <= 0) {
                printf("\nYou have been defeated by the %s...\n", monster->name);
                exit(0);
            }
        }

        // Oyuncu sağlık ve mana durumu
        printf("\nPlayer Health: %d, Mana: %d\n", player->health, player->mana);
    }

    if (monster->health <= 0 && !monster->isDefeated) {
        printf("\nYou have defeated the %s!\n", monster->name);
        addItemDrop(player, monster);
    }
}



// Haritayı temizleme
void freeMap(char **map, int height) {
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}
void printInventory(Player *player) {
    printf("\nYour Inventory:\n");
    for (int i = 0; i < player->inventoryCount; i++) {
        printf("%d - %s\n", i + 1, player->inventory[i]);
    }
    printf("\nHealth Potions: %d\n", player->potions);
    printf("Mana Potions: %d\n", player->manaPotions);
}

void printMonsterEncounter(Monster monster) {
    printf("\n  _______________________\n");
    printf(" |                       |\n");

    printf(" |      (%d HP)         |\n", monster.health); // Canavarın canı
    for (int i = 0; i < 5; i++) {
        char buffer[22] = {0}; // Her satır için sabit genişlikte bir tampon
        snprintf(buffer, 22, "%-21s", monster.asciiArt[i]); // ASCII sanatını düzgün hizala
        printf(" | %s|\n", buffer); // Sabit genişlikte hizalanmış satır yazdır
    }

    printf(" |_______________________|\n");
    printf("\nYou have encountered a %s!\n", monster.name);
    printf("Prepare to fight!\n");
}
void updateMapWithMonster(char **map, int mapWidth, int mapHeight, Monster monster) {
    int monsterStartX = mapWidth / 2 - 5; // Canavar ASCII sanatını merkeze hizalamak için
    int monsterStartY = mapHeight / 2 - 2;

    for (int i = 0; i < 5; i++) {
        if (monsterStartY + i >= 0 && monsterStartY + i < mapHeight) {
            for (int j = 0; j < 10; j++) {
                if (monsterStartX + j >= 0 && monsterStartX + j < mapWidth &&
                    monster.asciiArt[i][j] != ' ') { // Boşluk olmayan yerleri yaz
                    map[monsterStartY + i][monsterStartX + j] = monster.asciiArt[i][j];
                    }
            }
        }
    }
}

// Oyuncunun hareketini işleme
int movePlayer(Player *player, char input, char **map, int width, int height, int exitX, int exitY, int *currentRoom) {
    int newX = player->x;
    int newY = player->y;
    static char previousSymbol = '.'; // Oyuncunun eski konumundaki sembolü saklamak için

    // Yeni pozisyonu hesapla
    switch (input) {
        case 'w': newY--; break; // Yukarı
        case 's': newY++; break; // Aşağı
        case 'a': newX--; break; // Sol
        case 'd': newX++; break; // Sağ
        default:
            printf("\nInvalid input! Use W, A, S, D to move.\n");
        return 0;
    }

    // Duvar ve harita sınırları kontrolü
    if (newX < 0 || newX >= width || newY < 0 || newY >= height || map[newY][newX] == '#') {
        printf("\nYou can't move further in that direction! It's a wall.\n");
        return 0;
    }

    // Eğer oyuncu Market'e gidiyorsa
    if (map[newY][newX] == 'M') {
        printf("\nYou have entered the Market!\n");
        openMarket(player); // Marketi aç
    }

    // Eski konumu temizle
    map[player->y][player->x] = previousSymbol;

    // Yeni pozisyondaki sembolü sakla
    previousSymbol = map[newY][newX];

    // Oyuncunun konumunu güncelle
    player->x = newX;
    player->y = newY;

    // Çıkış kontrolü
    if (player->x == exitX && player->y == exitY) {
        return 1; // Sonraki odaya geç
    }

    // Önceki odaya dönüş kontrolü
    if (player->x == 0 && player->y == height / 2 && *currentRoom > 1) {
        (*currentRoom)--;
        return -1;
    }

    return 0; // Devam
}


// Rastgele boyut oluşturma
void generateRandomDimensions(int *width, int *height) {
    *width = (rand() % (MAP_WIDTH * 2 - MAP_WIDTH / 2 + 1)) + MAP_WIDTH / 2;
    *height = (rand() % (MAP_HEIGHT * 2 - MAP_HEIGHT / 2 + 1)) + MAP_HEIGHT / 2;
}
// Save fonksiyonu
int saveGame(Player *player, int currentRoom, char **map, int width, int height, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("\nError: Could not save game.\n");
    }

    // Oyuncu verilerini kaydet
    fprintf(file, "%d %d %d %d %d %d %d\n", player->x, player->y, player->health, player->mana, player->gold, currentRoom, player->inventoryCount);
    for (int i = 0; i < player->inventoryCount; i++) {
        fprintf(file, "%s\n", player->inventory[i]);
    }
    fprintf(file, "%s\n", player->armor);
    fprintf(file, "%s\n", player->weapon);

    // Canavarların durumunu kaydet
    for (int i = 0; i < MAX_ROOMS - 1; i++) {
        fprintf(file, "%d ", monsters[i].isDefeated);
    }
    fprintf(file, "\n");

    // Harita boyutlarını ve harita verisini kaydet
    fprintf(file, "%d %d\n", width, height);
    for (int i = 0; i < height; i++) {
        fprintf(file, "%s\n", map[i]);
    }

    fclose(file);
    printf("\nGame Saved Successfully!\n");
    return 1;
}


void cleanMap(char **map, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Yalnızca geçerli harita karakterlerini tut: '#', '.', 'E', 'X', 'M'
            if (map[i][j] != '#' && map[i][j] != '.' && map[i][j] != 'E' && map[i][j] != 'X' && map[i][j] != 'M') {
                map[i][j] = '.'; // Geçersiz karakterleri boş alanla değiştir
            }
        }
    }
}
void placePlayerOnMap(char **map, Player *player) {
    map[player->y][player->x] = 'P'; // Oyuncu konumuna P harfini yerleştir
}
void cleanAndReloadMap(char **map, int width, int height, Player *player) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (map[i][j] != '#') map[i][j] = '.'; // Haritayı temizle
        }
    }
    placePlayerOnMap(map, player);
}

// Dosyadan haritayı yükleme fonksiyonu
void loadMapFromFile(char **map, int width, int height, FILE *file) {
    char buffer[256]; // Satır tamponu
    for (int i = 0; i < height; i++) {
        if (fgets(buffer, sizeof(buffer), file)) {
            for (int j = 0; j < width; j++) {
                map[i][j] = buffer[j]; // Harita karakterlerini yükle
            }
        }
    }
}

// Oyunu Yükleme Fonksiyonu
int loadGame(Player *player, int *currentRoom, char ***map, int *width, int *height, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("\nError: Could not load game.\n");
        return 0;
    }

    // Oyuncu verilerini yükle
    fscanf(file, "%d %d %d %d %d %d %d", &player->x, &player->y, &player->health, &player->mana, &player->gold, currentRoom, &player->inventoryCount);

    for (int i = 0; i < player->inventoryCount; i++) {
        fscanf(file, "%s", player->inventory[i]);
    }

    fscanf(file, "%s", player->armor);
    fscanf(file, "%s", player->weapon);

    // Canavarların durumunu yükle
    for (int i = 0; i < MAX_ROOMS - 1; i++) {
        fscanf(file, "%d", &monsters[i].isDefeated);
    }

    // Harita boyutlarını ve verisini yükle
    fscanf(file, "%d %d", width, height);

    *map = (char **)malloc((*height) * sizeof(char *));
    for (int i = 0; i < *height; i++) {
        (*map)[i] = (char *)malloc((*width + 1) * sizeof(char)); // +1 for null terminator
        fscanf(file, "%s", (*map)[i]);
    }

    fclose(file);
    printf("\nGame Loaded Successfully!\n");
    return 1;
}


int main() {
    srand(time(NULL)); // Rastgele sayı üretimi için başlatıcı

    int entryX, entryY, exitX, exitY;
    int currentRoom = 1;
    const char *saveFile = "savegame.txt"; // Save dosyasının adı

    char roomText[MAX_TEXT_LENGTH];
    const char *roomFiles[MAX_ROOMS] = {"room1.txt", "room2.txt", "room3.txt", "room4.txt", "room5.txt"};

    // Harita boyutları
    int width = MAP_WIDTH;
    int height = MAP_HEIGHT;

    // Oyuncu ve harita başlangıç ayarları
    char **map = initializeMap(width, height, &entryX, &entryY, &exitX, &exitY, currentRoom);
    Player player = {entryX + 1, entryY, 100, 100, 0, "", "", 0, 0, 5, 5, {""}, 0, "", 0};
    int marketVisited = 0; // Market kontrolü

    // Oyun döngüsü
    while (1) {
        // Savaş sistemi
        if (currentRoom > 1) {
            battle(&player, &monsters[currentRoom - 2]);
        }

        // Oda metnini yükle ve haritayı yazdır
        loadRoomText(roomFiles[currentRoom - 1], roomText);
        cleanMap(map, width, height); // Haritayı temizle
        placePlayerOnMap(map, &player); // Oyuncuyu haritaya yerleştir
        printMap(map, width, height, player, roomText);

        // Kullanıcıya seçenekler
        printf("\nUse WASD keys to move, 'i' to open inventory, 's' to save, 'l' to load, and 'q' to quit.\n");
        char input = getch();

        if (input == 'q') {
            printf("\nExiting the game... Goodbye!\n");
            break; // Oyundan çıkış
        }
        if (input == 'i') {
            inventoryMenu(&player); // Envanter menüsü
            continue;
        }
        if (input == 's') {
            saveGame(&player, currentRoom, map, width, height, saveFile); // Oyunu kaydet
            continue;
        }
        if (input == 'l') {
            if (loadGame(&player, &currentRoom, &map, &width, &height, saveFile)) {
                printf("\nGame Loaded! Back to where you left off.\n");
            }
            continue;
        }

        // Market kontrolü
        if (map[player.y][player.x] == 'M') {
            printf("\nYou have entered the Market!\n");
            openMarket(&player); // Market fonksiyonunu çağır
            map[player.y][player.x] = 'M'; // Market sembolünü koru
            continue;
        }


        // Oyuncu hareketi
        int moveResult = movePlayer(&player, input, map, width, height, exitX, exitY, &currentRoom);

        if (moveResult == 1) { // Sonraki odaya geçiş
            if (currentRoom == MAX_ROOMS) {
                displayWinScreen(); // Kazanma ekranı
                break;
            }

            currentRoom++;
            freeMap(map, height);
            generateRandomDimensions(&width, &height);
            map = initializeMap(width, height, &entryX, &entryY, &exitX, &exitY, currentRoom);
            cleanMap(map, width, height); // Harita verisini temizle
            player.x = entryX + 1;
            player.y = entryY;
            marketVisited = 0;
        } else if (moveResult == -1) { // Önceki odaya dönüş
            freeMap(map, height);
            generateRandomDimensions(&width, &height);
            map = initializeMap(width, height, &entryX, &entryY, &exitX, &exitY, currentRoom);
            player.x = exitX - 1;
            player.y = entryY;
            marketVisited = 0;
        } else {
            // Marketten ayrılma kontrolü
            if (map[player.y][player.x] != 'M') {
                marketVisited = 0;
            }
        }
    }

    // Belleği temizle
    freeMap(map, height);
    return 0;
}
