#include "game.h"
#include <stdlib.h>

WINDOW *game_window;
char game_map[MAP_LENGTH][MAP_WIDTH];

int number_enemies = 7;
int number_obstacles = 5;

entity_info entity_infos;

time_t time1;

int lifepoints = 100;
int points = 0;

int is_collected = 0;

entity_info* getEntityInfo(int entity_type, int entity_id)
{
    entity_info * current_node = &entity_infos;
    // Iterate through the linked list to find and return the entry with the same type and same id
    while(current_node != NULL)
    {
        if(current_node->entity_type == entity_type && current_node->entity_id == entity_id)
        {
            return current_node;
        }
        current_node = current_node->next;
    }
    return NULL;
}

entity_info* getLocalPlayer()
{
    return getEntityInfo(TYPE_PLAYER, 0);
}

entity_info* getCollectible()
{
    return getEntityInfo(TYPE_COLLECTIBLE, 0);
}

void *playerLogic()
{
    entity_info* local_player = getLocalPlayer();

    game_map[local_player->pos.y_][local_player->pos.x_] = (char)TYPE_PLAYER;

    char direction = 'l';
    char prev_direction = 'l';

    int c;
    keypad(stdscr, TRUE);
    noecho();
    timeout(1);
    while (true)
    {
        c = getch();
        switch (c)
        {
            case 's':
                direction = 'd';
                break;
            case 'w':
                direction = 'u';
                break;
            case 'a':
                direction = 'l';
                break;
            case 'd':
                direction = 'r';
                break;
            default:
                break;
        }
        if (c == 'q')
        {
            lifepoints = 0;
            continue;
        }

        usleep(100000);

        if(getMapValue(local_player->pos.x_, local_player->pos.y_) == (char)TYPE_COLLECTIBLE)
        {
            is_collected = 1;
            points+=POINTS;
        }

        moveAndDrawPlayer(&direction, &prev_direction, &local_player->pos, TYPE_PLAYER);
    }
}

void *enemyObstacle(parameters *params)
{
    if (params->type_ != TYPE_ENEMY_OBSTACLE)
        return (void *)-1;

    position enemy_pos = {params->pos_x_, params->pos_y_};
    free(params);

    while (lifepoints > 0)
    {
        usleep(300000);
        drawEnemy(TYPE_ENEMY_OBSTACLE, enemy_pos.x_, enemy_pos.y_);
        drawEnemy(TYPE_ENEMY_OBSTACLE, enemy_pos.x_ + 1, enemy_pos.y_);
        drawEnemy(TYPE_ENEMY_OBSTACLE, enemy_pos.x_, enemy_pos.y_ + 1);
        drawEnemy(TYPE_ENEMY_OBSTACLE, enemy_pos.x_ + 1, enemy_pos.y_ + 1);
    }

    return (void *)999;
}

void *enemyBasic(parameters *params)
{
    if (params->type_ != TYPE_ENEMY_BASIC)
        return (void *)-1;

    position enemy_pos = {params->pos_x_, params->pos_y_};
    drawEnemy(TYPE_ENEMY_BASIC, enemy_pos.x_, enemy_pos.y_);
    
    free(params);

    int nr_direction = 0;
    char prev_direction = 'l';

    while (lifepoints > 0)
    {
        usleep(300000);
        
        if (rand() % 5 == 0)
        {
            nr_direction = rand() % 4;
        }
        char direction;
        switch (nr_direction)
        {
            case 0:
                direction = 'l';
                moveAndDrawPlayer(&direction, &prev_direction, &enemy_pos, (char) TYPE_ENEMY_BASIC);
                break;
            case 1:
                direction = 'r';
                moveAndDrawPlayer(&direction, &prev_direction, &enemy_pos, (char) TYPE_ENEMY_BASIC);
                break;
            case 2:
                direction = 'u';
                moveAndDrawPlayer(&direction, &prev_direction, &enemy_pos, (char) TYPE_ENEMY_BASIC);
                break;
            case 3:
                direction = 'd';
                moveAndDrawPlayer(&direction, &prev_direction, &enemy_pos, (char) TYPE_ENEMY_BASIC);
                break;
            default:
                break;
        }
    }
    return (void *)998;
}

void init_enemies(unsigned char type, int number_of_enemy_type)
{
    pthread_attr_t enemy_attr;
    pthread_attr_init(&enemy_attr);

    pthread_attr_setdetachstate(&enemy_attr, PTHREAD_CREATE_JOINABLE);

    for(int i = 0; i < number_of_enemy_type; i++)
    {
        entity_info* entity = getEntityInfo(type, i);

        if(!entity)
        {
            continue;
        }

        parameters *enemy_params = malloc(sizeof(parameters));
        enemy_params->pos_x_ = getRandPosX();
        enemy_params->pos_y_ = getRandPosY();
        if(type == TYPE_ENEMY_BASIC)
        {
            enemy_params->type_ = TYPE_ENEMY_BASIC;
            pthread_create(&entity->tid, &enemy_attr, (void*) enemyBasic, (parameters*) enemy_params);
        }
        else
        {
            enemy_params->type_ = TYPE_ENEMY_OBSTACLE;
            pthread_create(&entity->tid, &enemy_attr, (void*) enemyObstacle, (parameters*) enemy_params);
        }
        
    }

    pthread_attr_destroy(&enemy_attr);
}

void *placeCollectible(parameters *params)
{
    if (params->type_ != TYPE_COLLECTIBLE)
        return (void *)-1;

    unsigned char position_x = params->pos_x_;
    unsigned char position_y = params->pos_y_;
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {
        drawCollectible(position_x, position_y);
    }
}

int start_game(int argc, char* argv[])
{
    init_screen();
    srand((unsigned int)time(&time1));

    entity_info* current_node = &entity_infos;
    memset(current_node, 0, sizeof(entity_info));

    {
        current_node->entity_type = TYPE_PLAYER;
        current_node->entity_id = 0;
    }

    for(int i = 0; i < number_enemies + number_obstacles; i++)
    {
        //Allocate a new node and update the linked list with the new node
        entity_info* new_node = (entity_info*)malloc(sizeof(entity_info));
        memset(new_node, 0, sizeof(entity_info));

        current_node->next = new_node;
        current_node = new_node;

        if (i < number_enemies)
        {
            current_node->entity_type = TYPE_ENEMY_BASIC;
            current_node->entity_id = i; // IDs for enemies start from 0
        }
        else
        {
            current_node->entity_type = TYPE_ENEMY_OBSTACLE;
            current_node->entity_id = i - number_enemies; // IDs for obstacles start from 0
        }
        current_node->next = NULL;
    }
    

    init_map();

    entity_info* local_player = getLocalPlayer();

    local_player->pos.x_ = MAP_WIDTH / 2;
    local_player->pos.y_ = MAP_LENGTH - 2;

    pthread_create(&local_player->tid, NULL, playerLogic, (void*)local_player);

    init_enemies(TYPE_ENEMY_BASIC, number_enemies);
    init_enemies(TYPE_ENEMY_OBSTACLE, number_obstacles);

    refreshMap();

    entity_info* new_node = (entity_info*)malloc(sizeof(entity_info));

    current_node->next = new_node;
    current_node = new_node;
    
    current_node->next = NULL;
    current_node->entity_type = TYPE_COLLECTIBLE;
    current_node->entity_id = 0;

    parameters *colectable_params = malloc(sizeof(parameters));
    colectable_params->type_ = TYPE_COLLECTIBLE;
    colectable_params->pos_x_ = getRandPosX();
    colectable_params->pos_y_ = getRandPosY();
    
    pthread_create(&current_node->tid, NULL, (void*) placeCollectible, (parameters*) colectable_params);

    while (lifepoints > 0)
    {
        usleep(10000);

        if (is_collected == 1)
        {
            pthread_cancel(getCollectible()->tid);
            pthread_join(getCollectible()->tid, &getCollectible()->return_value);

            game_map[colectable_params->pos_y_][colectable_params->pos_x_] = (char)TYPE_EMPTY;
            
            colectable_params->pos_x_ = getRandPosX();
            colectable_params->pos_y_ = getRandPosY();
            
            pthread_create(&current_node->tid, NULL, (void*) placeCollectible, (parameters*) colectable_params);

            is_collected = 0;
        }

        refreshMap();
    }

    free(colectable_params);

    pthread_cancel(getLocalPlayer()->tid);
    pthread_cancel(getCollectible()->tid);
    
    current_node = getLocalPlayer();
    while (current_node != NULL)
    {
        pthread_join(current_node->tid, &current_node->return_value);
        current_node = current_node->next;
    }

    return end_game();
}