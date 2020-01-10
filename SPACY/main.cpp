#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "objects.h"


// globals

const int WIDTH = 800;
const int HEIGHT = 400;
const int NUM_BULLETS = 5;
const int NUM_COMETS = 10;
const int NUM_EXPLOSIONS = 5;

const char FONT_DAUNPENH[] = "daunpenh.ttf";

enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = { false, false, false, false, false };

// prototypes
void InitBg(Background &bg, int x, int y, int width, int height, ALLEGRO_BITMAP *image);
void DrawBg(Background &bg);

void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image);
void ResetShipAnimation(SpaceShip &ship, int position);
void DrawShip(SpaceShip &ship);
void MoveShipUp(SpaceShip &ship);
void MoveShipDown(SpaceShip &ship);
void MoveShipLeft(SpaceShip &ship);
void MoveShipRight(SpaceShip &ship);

void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size);
void FireBullet(Bullet bullet[], int size, SpaceShip &ship);
void UpdateBullet(Bullet bullet[], int size);
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize);

void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *image);
void DrawComet(Comet comets[], int size);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size);
void CollideComet(Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize);

void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image);
void DrawExplosions(Explosion explosions[], int size);
void StartExplosions(Explosion explosions[], int size, int x, int y);
void UpdateExplosions(Explosion explosions[], int size);

int main(int argc, char **argv) {

    // primitive variable
    bool done = false;
    bool redraw = true;
    const int FPS = 60;
    bool isGameOver = false;
    int menu =1;
    Background Menu;

    //Main Menu
    int state = 1;

    // object variables
    SpaceShip ship;
    Bullet bullets[NUM_BULLETS];
    Comet comets[NUM_COMETS];
    Explosion explosions[NUM_EXPLOSIONS];

    // allegro variables
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font25 = NULL;
    ALLEGRO_BITMAP *shipImage;
    ALLEGRO_BITMAP *cometImage;
    ALLEGRO_BITMAP *expImage;
    ALLEGRO_BITMAP *mnImage;
    ALLEGRO_SAMPLE *sample = NULL;
    ALLEGRO_SAMPLE *sample2 = NULL;
    ALLEGRO_SAMPLE *sample3 = NULL;
    ALLEGRO_SAMPLE_INSTANCE *instance1 = NULL;
    ALLEGRO_SAMPLE_INSTANCE *instance2 = NULL;
    ALLEGRO_SAMPLE_INSTANCE *instance3 = NULL;

    // check allegro initialize
    if(!al_init())
        return -1;

    // create display
    display = al_create_display(WIDTH, HEIGHT);

    // check display
    if(!display)
        return -1;

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    shipImage = al_load_bitmap("spaceship_sprites.png");
    al_convert_mask_to_alpha(shipImage, al_map_rgb(255, 0, 255));

    cometImage = al_load_bitmap("meteor_sprites.png");
    al_convert_mask_to_alpha(cometImage, al_map_rgb(0, 0, 0));

    expImage = al_load_bitmap("explosion.png");

    mnImage = al_load_bitmap("mnBg.png");

    srand(time(NULL));
    InitShip(ship, shipImage);
    InitBg(Menu, 0, 0, 800, 400, mnImage);
    InitBullet(bullets, NUM_BULLETS);
    InitComet(comets, NUM_COMETS, cometImage);
    InitExplosions(explosions, NUM_EXPLOSIONS, expImage);

    font25 = al_load_font(FONT_DAUNPENH, 25, 0);

    al_reserve_samples(10);

    sample = al_load_sample("horn.ogg");
    sample2 = al_load_sample("pistol.ogg");
    sample3 = al_load_sample("gotta.ogg");

    instance1 = al_create_sample_instance(sample);
    instance2 = al_create_sample_instance(sample2);
    instance3 = al_create_sample_instance(sample3);

    al_attach_sample_instance_to_mixer(instance1, al_get_default_mixer());
    al_attach_sample_instance_to_mixer(instance2, al_get_default_mixer());
    al_attach_sample_instance_to_mixer(instance3, al_get_default_mixer());

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);
    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;
            if(keys[UP])
                MoveShipUp(ship);
            else if(keys[DOWN])
                MoveShipDown(ship);
            else
                ResetShipAnimation(ship, 1);

            if(keys[LEFT])
                MoveShipLeft(ship);
            else if(keys[RIGHT])
                MoveShipRight(ship);
            else
                ResetShipAnimation(ship, 2);

            switch(menu){
            case 1 :
            break;
            case 2 :
                UpdateExplosions(explosions, NUM_EXPLOSIONS);
                UpdateBullet(bullets, NUM_BULLETS);
                StartComet(comets, NUM_COMETS);
                UpdateComet(comets, NUM_COMETS);
                CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, ship, explosions, NUM_EXPLOSIONS);
                CollideComet(comets, NUM_COMETS, ship, explosions, NUM_EXPLOSIONS);
                if(ship.lives <= 0) {
                    ship.lives = 5;
                    menu = 3;
                }
                al_play_sample_instance(instance2);
            break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }
        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
                case ALLEGRO_KEY_UP:
                    keys[UP] = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    keys[DOWN] = true;
                    break;
                case ALLEGRO_KEY_LEFT:
                    keys[LEFT] = true;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    keys[RIGHT] = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                    keys[SPACE] = true;
                    FireBullet(bullets, NUM_BULLETS, ship);
                    break;
                case ALLEGRO_KEY_ENTER:
                    if(menu == 1){
                        menu = 2;
                    } else {
                        menu = 1;
                    }
                    break;
            }
        }
        if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                    keys[UP] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    keys[DOWN] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    keys[LEFT] = false;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    keys[RIGHT] = false;
                    break;
                case ALLEGRO_KEY_SPACE:
                    keys[SPACE] = false;
                    break;
            }
        }


        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            switch(menu){
            case 1 :
            DrawBg(Menu);
            al_play_sample_instance(instance3);
            break;

            case 2:
                DrawShip(ship);
                DrawBullet(bullets, NUM_BULLETS);
                DrawComet(comets, NUM_COMETS);
                DrawExplosions(explosions, NUM_EXPLOSIONS);

                al_draw_textf(font25, al_map_rgb(200, 0, 255), 5, 5, 0, "Player has %i lives left. Player has destroyed %i objects", ship.lives, ship.score);
                al_draw_textf(font25, al_map_rgb(200, 0, 255), 520, 5, 0, "Press space to shoot");
                al_draw_textf(font25, al_map_rgb(200, 0, 255), 520, 25, 0, "Press up,down,left,right to move");
                al_draw_textf(font25, al_map_rgb(200, 0, 255), 10, 380, 0, "Press ESC to exit");
            break;

            case 3:
                al_draw_textf(font25, al_map_rgb(0, 255, 255), 320, 150, 0, "GAME OVER");
                al_draw_textf(font25, al_map_rgb(0, 255, 255), 320, 200,0, "Final Score  %i.", ship.score);
                al_draw_textf(font25, al_map_rgb(0, 255, 255), 320, 250, 0, "Press Enter To Retry");
                al_play_sample_instance(instance1);
            break;
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }

    al_destroy_bitmap(expImage);
    al_destroy_bitmap(shipImage);
    al_destroy_bitmap(cometImage);
    al_destroy_sample_instance(instance1);
    al_destroy_sample(sample);
    al_destroy_sample_instance(instance2);
    al_destroy_sample(sample2);
    al_destroy_sample_instance(instance3);
    al_destroy_sample(sample3);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font25);
    al_destroy_display(display);

    return 0;
}

void InitShip(SpaceShip &ship, ALLEGRO_BITMAP *image)
{
    ship.x = 20;
    ship.y = HEIGHT / 2;
    ship.ID = PLAYER;
    ship.lives = 5;
    ship.speed = 6;
    ship.boundx = 10;
    ship.boundy = 15;
    ship.score = 0;

    ship.maxFrame = 3;
    ship.curFrame = 0;
    ship.frameCount = 0;
    ship.frameDelay = 50;
    ship.frameWidth = 44;
    ship.frameHeight = 44;
    ship.animationColums = 3;
    ship.animationDirection = 1;

    ship.animationRow = 1;

    ship.image = image;
}
void ResetShipAnimation(SpaceShip &ship, int position)
{
    if(position == 1)
        ship.animationRow = 1;
    else
        ship.curFrame = 0;
}
void DrawShip(SpaceShip &ship) {
    int fx = (ship.curFrame % ship.animationColums) * ship.frameWidth;
    int fy = ship.animationRow * ship.frameHeight;

    al_draw_bitmap_region(ship.image, fx, fy, ship.frameWidth,
        ship.frameHeight, ship.x - ship.frameWidth / 2, ship.y - ship.frameHeight / 2, 0);
}
void MoveShipUp(SpaceShip &ship)
{
    ship.animationRow = 0;
    ship.y -= ship.speed;
    if(ship.y < 0)
        ship.y = 0;
}
void MoveShipDown(SpaceShip &ship)
{
    ship.animationRow = 2;
    ship.y += ship.speed;
    if(ship.y > HEIGHT)
        ship.y = HEIGHT;
}
void MoveShipLeft(SpaceShip &ship)
{
    ship.curFrame = 2;
    ship.x -= ship.speed;
    if(ship.x < 0)
        ship.x = 0;
}
void MoveShipRight(SpaceShip &ship)
{
    ship.curFrame = 1;
    ship.x += ship.speed;
    if(ship.x > 300)
        ship.x = 300;
}

void InitBullet(Bullet bullet[], int size)
{
    for(int i = 0; i < size; i++)
    {
        bullet[i].ID = BULLET;
        bullet[i].speed = 10;
        bullet[i].live = false;
    }
}
void DrawBullet(Bullet bullet[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(bullet[i].live)
            al_draw_filled_circle(bullet[i].x, bullet[i].y, 2, al_map_rgb(255, 255, 255));
    }
}
void FireBullet(Bullet bullet[], int size, SpaceShip &ship)
{
    for(int i = 0; i < size; i++)
    {
        if(!bullet[i].live)
        {
            bullet[i].x = ship.x + 17;
            bullet[i].y = ship.y;
            bullet[i].live = true;
            break;
        }
    }
}
void UpdateBullet(Bullet bullet[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(bullet[i].live)
        {
            bullet[i].x += bullet[i].speed;
            if(bullet[i].x > WIDTH)
                bullet[i].live = false;
        }
    }
}
void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize)
{
    for(int i = 0; i < bSize; i++)
    {
        if(bullet[i].live)
        {
            for(int j = 0; j < cSize; j++)
            {
                if(comets[j].live)
                {
                    if(bullet[i].x > (comets[j].x - comets[j].boundx) &&
                       bullet[i].x < (comets[j].x + comets[j].boundx) &&
                       bullet[i].y > (comets[j].y - comets[j].boundy) &&
                       bullet[i].y < (comets[j].y + comets[j].boundy))
                    {
                        bullet[i].live = false;
                        comets[j].live = false;

                        ship.score++;

                        StartExplosions(explosions, eSize, bullet[i].x, bullet[i].y);
                    }
                }
            }
        }
    }
}

void InitComet(Comet comets[], int size, ALLEGRO_BITMAP *image)
{
    for(int i = 0; i < size; i++)
    {
        comets[i].ID = ENEMY;
        comets[i].live = false;
        comets[i].speed = 3;
        comets[i].boundx = 15;
        comets[i].boundy = 15;

        comets[i].maxFrame = 10;
        comets[i].curFrame = 0;
        comets[i].frameCount = 0;
        comets[i].frameDelay = 5;
        comets[i].frameWidth = 38;
        comets[i].frameHeight = 38;
        comets[i].animationColums = 10;

        comets[i].animationDirection = 1;

        comets[i].image = image;
    }
}
void DrawComet(Comet comets[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(comets[i].live)
        {
            int fx = (comets[i].curFrame % comets[i].animationColums) * comets[i].frameWidth;
            int fy = (comets[i].curFrame / comets[i].animationColums) * comets[i].frameWidth;

            al_draw_bitmap_region(comets[i].image, fx, fy, comets[i].frameWidth,
                comets[i].frameHeight, comets[i].x - comets[i].frameWidth / 2, comets[i].y - comets[i].frameHeight / 2, 0);
        }
    }
}
void StartComet(Comet comets[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(!comets[i].live)
        {
            if(rand() % 500 == 0)
            {
                comets[i].live = true;
                comets[i].x = WIDTH;
                comets[i].y = 30 + rand() % (HEIGHT - 60);

                break;
            }
        }
    }
}
void UpdateComet(Comet comets[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(comets[i].live)
        {
            if(++comets[i].frameCount >= comets[i].frameDelay)
            {
                comets[i].curFrame += comets[i].animationDirection;
                if(comets[i].curFrame >= comets[i].maxFrame)
                    comets[i].curFrame = 0;
                else if(comets[i].curFrame <= 0)
                    comets[i].curFrame = comets[i].maxFrame;

                comets[i].frameCount = 0;
            }

            comets[i].x -= comets[i].speed;

        }
    }
}
void CollideComet(Comet comets[], int cSize, SpaceShip &ship, Explosion explosions[], int eSize)
{
    for(int i = 0; i < cSize; i++)
    {
        if(comets[i].live)
        {
            if(comets[i].x - comets[i].boundx < ship.x + ship.boundx && comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
                comets[i].y - comets[i].boundy < ship.y + ship.boundy && comets[i].y + comets[i].boundy > ship.y - ship.boundy)
            {
                ship.lives--;
                comets[i].live = false;
            }
            else if(comets[i].x < 0)
            {
                comets[i].live = false;
                ship.lives--;
            }

            if(!comets[i].live)
            {
                StartExplosions(explosions, eSize, comets[i].x, comets[i].y);
            }
        }
    }
}

void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image)
{
    for(int i = 0; i < size; i++)
    {
        explosions[i].live = false;

        explosions[i].maxFrame = 10;
        explosions[i].curFrame = 0;
        explosions[i].frameCount = 0;
        explosions[i].frameDelay = 1;
        explosions[i].frameWidth = 128;
        explosions[i].frameHeight = 128;
        explosions[i].animationColums = 10;
        explosions[i].animationDirection = 1;

        explosions[i].image = image;

    }
}
void DrawExplosions(Explosion explosions[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(explosions[i].live)
        {
            int fx = (explosions[i].curFrame % explosions[i].animationColums) * explosions[i].frameWidth;
            int fy = (explosions[i].curFrame / explosions[i].animationColums) * explosions[i].frameHeight;

            al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth,
                                  explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2,
                                  explosions[i].y - explosions[i].frameHeight / 2, 0);
        }
    }
}
void StartExplosions(Explosion explosions[], int size, int x, int y)
{
    for(int i = 0; i < size; i++)
    {
        if(!explosions[i].live)
        {
            explosions[i].live = true;
            explosions[i].x = x;
            explosions[i].y = y;

            break;
        }
    }
}
void UpdateExplosions(Explosion explosions[], int size)
{
    for(int i = 0; i < size; i++)
    {
        if(explosions[i].live)
        {
            if(++explosions[i].frameCount >= explosions[i].frameDelay)
            {
                explosions[i].curFrame += explosions[i].animationDirection;
                if(explosions[i].curFrame >= explosions[i].maxFrame)
                {
                    explosions[i].curFrame = 0;
                    explosions[i].live = false;
                }

                explosions[i].frameCount = 0;
            }
        }
    }
}

void InitBg(Background &bg, int x, int y, int width, int height, ALLEGRO_BITMAP *image)
{
    bg.x = x;
    bg.y = y;
    bg.width = width;
    bg.height = height;
    bg.image = image;
}
void DrawBg(Background &bg)
{
    al_draw_bitmap(bg.image, bg.x, bg.y, 0);
}
