#include <ncurses.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "string.h"
#include <time.h>

#define VERSION 1

// Map geometry
#define MAP_WIDTH 50
#define MAP_LENGTH 50

// Field Types
#define TYPE_EMPTY 1
#define TYPE_PLAYER 2
#define TYPE_COLLECTIBLE 3
#define TYPE_ENEMY_BASIC 6
#define TYPE_ENEMY_OBSTACLE 7

// ncurses constants
#define SPACE 51
#define PLAYER_COLOR 52
#define ENEMY_COLOR 53
#define COLLECTABLE_COLOR 54
#define OBSTACLE_COLOR 55

// Game settings
#define POINTS 1000

#define INVALID_TID ((pthread_t)(-1))

#ifndef SNP_TUTOR_TEST_1_H
#define SNP_TUTOR_TEST_1_H

typedef struct
{
    unsigned char x_;
    unsigned char y_;
} position;

typedef struct
{
    unsigned char pos_x_;
    unsigned char pos_y_;
    unsigned char type_;
} parameters;

typedef struct entity_info_t
{
    struct entity_info_t* next;
    void* return_value;
    pthread_t tid;
    int entity_type;
    int entity_id;
    // Unused except for local player
    position pos;
} entity_info;

extern entity_info entity_infos;

extern WINDOW *game_window;
extern char game_map[MAP_LENGTH][MAP_WIDTH];

extern int lifepoints;
extern int points;
extern time_t time1;
extern int is_collected;

extern int number_enemies;
extern int number_obstacles;

void init_map();
void init_screen();

void drawCollectible(int x_pos, int y_pos);
void drawEnemy(char type, int x_pos, int y_pos);
char getMapValue(int x_pos, int y_pos);

void refreshMap();

int start_game(int argc, char** argv);
int end_game();

void freeNodes(entity_info* node);
entity_info* getEntityInfo(int entity_type, int entity_id);
entity_info* getLocalPlayer();
entity_info* getCollectible();

void *playerLogic();

void *enemyObstacle(parameters *params);
void *enemyBasic(parameters *params);

void *placeCollectible(parameters *params);

void init_enemies(unsigned char type, int number_of_enemy_type);

void moveAndDrawPlayer(char *direction, char *prev_direction, position *pos, char type);

char getRandPosX();

char getRandPosY();

#endif