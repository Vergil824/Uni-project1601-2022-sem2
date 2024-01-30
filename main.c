#include "stdio.h"
#include "stdlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "time.h"

#include "math.h"
#include "formulas.h"
#include "wall.h"
#include "robot.h"

void QUIT();

void LOAD();

void PLAY();

void MAZE1();

void MAZE2();

void MAZE3();

void PlayMusic(char *file);

void ClearCollection();

int done = 0;
SDL_Window *window;     // canvas
SDL_Renderer *renderer; // brush

SDL_Surface *MainBackGroundSurface = NULL;
SDL_Texture *MainBackGroundTexture = NULL;
SDL_Rect MainBackGroundRect;

SDL_Surface *PlayBackGroundSurface = NULL;
SDL_Texture *PlayBackGroundTexture = NULL;

struct Robot robot;
struct Wall_collection *head = NULL;
struct Wall_collection *end = NULL;
int front_centre_sensor, left_sensor, right_sensor = 0;
clock_t start_time, end_time;
int msec;
int crashed = 0;
int maze = 0;

int main(int argc, char *argv[])
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        return 1;
    }
    PlayMusic("BGM.MP3");
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Event MainEvent;
    LOAD();
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, MainBackGroundTexture, NULL, &MainBackGroundRect);
    SDL_RenderPresent(renderer);

    while (SDL_WaitEvent(&MainEvent))
    {
        switch (MainEvent.type)
        {
            // case SDL_QUIT:
            //     QUIT();
            //     break;

        case SDL_KEYDOWN:
            switch (MainEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                QUIT();
                break;

            case SDLK_RETURN:
                PLAY();
                break;

            default:
                break;
            }

        case SDL_MOUSEBUTTONDOWN:

            // printf("%d,%d\n", MainEvent.button.x, MainEvent.button.y);
            break;

        case SDL_MOUSEBUTTONUP:
            if (MainEvent.button.x > 49 && MainEvent.button.x < 210 && MainEvent.button.y > 184 && MainEvent.button.y < 232)
            {
                MAZE1();
                maze = 1;
                setup_robot(&robot); // changed from "setup_robot(&robot, OVERALL_WINDOW_WIDTH/2-50, OVERALL_WINDOW_HEIGHT-50);"
                PLAY();
            }

            else if (MainEvent.button.x > 49 && MainEvent.button.x < 210 && MainEvent.button.y > 245 && MainEvent.button.y < 291)
            {
                MAZE2();
                maze = 2;
                setup_robot(&robot); // changed from "setup_robot(&robot, OVERALL_WINDOW_WIDTH/2-50, OVERALL_WINDOW_HEIGHT-50);"
                PLAY();
            }

            else if (MainEvent.button.x > 49 && MainEvent.button.x < 210 && MainEvent.button.y > 307 && MainEvent.button.y < 354)
            {
                MAZE3();
                maze = 3;
                setup_robot(&robot); // changed from "setup_robot(&robot, OVERALL_WINDOW_WIDTH/2-50, OVERALL_WINDOW_HEIGHT-50);"
                PLAY();
            }

        default:
            break;
        }
    }
    QUIT();
}

//-------------------------------------------------------------------------------Functions------------------------------------------------------------------------------------------

void LOAD()
{
    MainBackGroundSurface = IMG_Load("MainBackGround.png");
    MainBackGroundTexture = SDL_CreateTextureFromSurface(renderer, MainBackGroundSurface);
    MainBackGroundRect.x = 0;
    MainBackGroundRect.y = 0;
    MainBackGroundRect.w = MainBackGroundSurface->w;
    MainBackGroundRect.h = MainBackGroundSurface->h;

    PlayBackGroundSurface = IMG_Load("PlayBackGround.png");
    PlayBackGroundTexture = SDL_CreateTextureFromSurface(renderer, PlayBackGroundSurface);
}

void QUIT()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("quit\n");
    SDL_Quit();
}

void PLAY()
{
    printf("Welcome to Astro Chariot! Enjoy it!\n");
    // SDL_RenderPresent(renderer);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while (!done)
    {
        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, PlayBackGroundTexture, NULL, NULL);

        // Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
            robotAutoMotorMove(&robot, front_centre_sensor, left_sensor, right_sensor);
        robotMotorMove(&robot, crashed);

        // Check if robot reaches endpoint. and check sensor values
        // checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT / 2 + 100, 10, 100) maze1
        // checkRobotReachedEnd(&robot, 600, 340, 50, 100) maze2
        // checkRobotReachedEnd(&robot, 600, 20, 50, 60) maze3
        if (checkRobotReachedEnd(&robot, 600, 20, 50, 60))
        {
            end_time = clock();
            msec = (end_time - start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        else if (crashed == 1 || checkRobotHitWalls(&robot, head))
        {
            robotCrash(&robot);
            crashed = 1;
        }
        // Otherwise compute sensor information
        else
        {
            front_centre_sensor = checkRobotSensorFrontCentreAllWalls(&robot, head);
            if (front_centre_sensor > 0)
                printf("Getting close on the centre. Score = %d\n", front_centre_sensor);

            left_sensor = checkRobotSensorLeftAllWalls(&robot, head);
            if (left_sensor > 0)
                printf("Getting close on the left. Score = %d\n", left_sensor);

            right_sensor = checkRobotSensorRightAllWalls(&robot, head);
            if (right_sensor > 0)
                printf("Getting close on the right. Score = %d\n", right_sensor);
        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if (state[SDL_SCANCODE_UP] && robot.direction != DOWN)
            {
                robot.direction = UP;
            }
            if (state[SDL_SCANCODE_DOWN] && robot.direction != UP)
            {
                robot.direction = DOWN;
            }
            if (state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT)
            {
                robot.direction = LEFT;
            }
            if (state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT)
            {
                robot.direction = RIGHT;
            }
            if (state[SDL_SCANCODE_SPACE])
            {
                setup_robot(&robot);

                // change from:
                //  if (maze == 1 || maze == 3){
                //      setup_robot(&robot, OVERALL_WINDOW_WIDTH/2-50, OVERALL_WINDOW_HEIGHT-50);
                //  } else if (maze == 2){
                //      setup_robot(&robot, 100, OVERALL_WINDOW_HEIGHT-50);
                //  }
            }
            if (state[SDL_SCANCODE_RETURN])
            {
                robot.auto_mode = 1;
                start_time = clock();
            }
            if (state[SDL_SCANCODE_ESCAPE])
            {
                SDL_RenderClear(renderer);
                ClearCollection();
                SDL_RenderCopy(renderer, MainBackGroundTexture, NULL, &MainBackGroundRect);
                SDL_RenderPresent(renderer);

                return;
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
    return;
}

void PlayMusic(char *file)
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *sound = Mix_LoadMUS(file);
    Mix_PlayMusic(sound, -1);
}

void ClearCollection()
{
    struct Wall_collection *temp = head;
    while (temp != NULL)
    {
        //    printf("%d\n",temp->a);
        struct Wall_collection *pt = temp;
        temp = temp->next;
        free(pt);
    }

    head = NULL;
    end = NULL;
}

void MAZE1()
{
    // Default maze
    int name_index = 0;
    insertAndSetFirstWall(&head, name_index++, 150, 450, 10, 50);
    insertAndSetFirstWall(&head, name_index++, 250, 350, 10, 150);
    insertAndSetFirstWall(&head, name_index++, 50, 450, 100, 10);
    insertAndSetFirstWall(&head, name_index++, 150, 350, 100, 10);
    insertAndSetFirstWall(&head, name_index++, 50, 230, 10, 220);
    insertAndSetFirstWall(&head, name_index++, 150, 330, 10, 30);
    insertAndSetFirstWall(&head, name_index++, 50, 230, 10, 170);
    insertAndSetFirstWall(&head, name_index++, 150, 330, 10, 30);
    insertAndSetFirstWall(&head, name_index++, 50, 230, 330, 10);
    insertAndSetFirstWall(&head, name_index++, 150, 330, 130, 10);
    insertAndSetFirstWall(&head, name_index++, 380, 230, 10, 100);
    insertAndSetFirstWall(&head, name_index++, 280, 330, 10, 100);
    insertAndSetFirstWall(&head, name_index++, 280, 430, 110, 10);
}

void MAZE2()
{
    insertAndSetFirstWall(&head, 2, 220, 400, 10, 80);
    insertAndSetFirstWall(&head, 2, 20, 400, 200, 10);
    insertAndSetFirstWall(&head, 2, 20, 50, 10, 350);
    insertAndSetFirstWall(&head, 2, 20, 50, 280, 10);
    insertAndSetFirstWall(&head, 2, 300, 50, 10, 100);
    insertAndSetFirstWall(&head, 2, 300, 150, 110, 10);
    insertAndSetFirstWall(&head, 2, 400, 50, 10, 100);
    insertAndSetFirstWall(&head, 2, 400, 50, 220, 10);
    insertAndSetFirstWall(&head, 2, 620, 50, 10, 290);
    insertAndSetFirstWall(&head, 2, 620, 340, 20, 10);

    insertAndSetFirstWall(&head, 1, 320, 300, 10, 180);
    insertAndSetFirstWall(&head, 2, 120, 300, 200, 10);
    insertAndSetFirstWall(&head, 2, 120, 150, 10, 150);
    insertAndSetFirstWall(&head, 2, 120, 150, 80, 10);
    insertAndSetFirstWall(&head, 2, 200, 150, 10, 100);
    insertAndSetFirstWall(&head, 2, 200, 250, 310, 10);
    insertAndSetFirstWall(&head, 2, 500, 150, 10, 100);
    insertAndSetFirstWall(&head, 2, 500, 150, 10, 100);
    insertAndSetFirstWall(&head, 2, 500, 150, 20, 10);
    insertAndSetFirstWall(&head, 2, 520, 150, 10, 290);
    insertAndSetFirstWall(&head, 2, 520, 440, 120, 10);
}

void MAZE3()
{
    int i;
    insertAndSetFirstWall(&head, 12, 120, 450, 10, 30);
    insertAndSetFirstWall(&head, 12, 220, 450, 10, 30);
    for (i = 0; i < 100; i++)
    {
        insertAndSetFirstWall(&head, i, 20 + i, 350 + i, 10, 10);       // 1
        insertAndSetFirstWall(&head, i, 20 + 100 + i, 350 + i, 10, 10); // 1
    }
    insertAndSetFirstWall(&head, 12, 20, 280, 10, 70);
    insertAndSetFirstWall(&head, 12, 120, 280, 10, 70);
    for (i = 0; i < 180; i++)
    {
        insertAndSetFirstWall(&head, i, 20 + 190 - i / 2, 100 + i, 10, 10); // 1
    }
    for (i = 0; i < 105; i++)
    {
        insertAndSetFirstWall(&head, i, 20 + 105 / 2 - i / 2, 175 + i, 10, 10); // 1
    }
    insertAndSetFirstWall(&head, 2, 20, 175, 105 / 2, 10);
    insertAndSetFirstWall(&head, 2, 20, 20, 10, 155);
    insertAndSetFirstWall(&head, 2, 20, 20, 300, 10);
    insertAndSetFirstWall(&head, 2, 320, 20, 10, 60);
    insertAndSetFirstWall(&head, 2, 80, 100, 130, 10);
    insertAndSetFirstWall(&head, 2, 80, 80, 10, 20);
    insertAndSetFirstWall(&head, 2, 80, 80, 160, 10);

    double j;
    for (i = 0; i < 50; i++)
    {
        j = i;
        insertAndSetFirstWall(&head, i + 1,
                              // the most important bit is below.
                              // increase the 20 for a tighter bend
                              // descrease for a more meandering flow
                              320 + 30 * sin(10 * j * M_PI / 180),
                              // increase the 5 for a spacier curve
                              (i * 5) + 80,
                              10, 10);
    }
    for (i = 0; i < 75; i++)
    {
        j = i;
        insertAndSetFirstWall(&head, i + 1,
                              // the most important bit is below.
                              // increase the 20 for a tighter bend
                              // descrease for a more meandering flow
                              240 + 30 * sin(10 * j * M_PI / 180),
                              // increase the 5 for a spacier curve
                              (i * 5) + 80,
                              10, 10);
    }
    insertAndSetFirstWall(&head, 2, 345, 330, 105, 10);
    insertAndSetFirstWall(&head, 2, 450, 190, 10, 150);
    insertAndSetFirstWall(&head, 2, 380, 190, 70, 10);
    insertAndSetFirstWall(&head, 2, 380, 20, 10, 170);
    insertAndSetFirstWall(&head, 2, 380, 20, 260, 10);

    insertAndSetFirstWall(&head, 2, 255, 455, 345, 10);
    insertAndSetFirstWall(&head, 2, 600, 100, 10, 365);
    insertAndSetFirstWall(&head, 2, 530, 100, 70, 10);
    insertAndSetFirstWall(&head, 2, 530, 80, 10, 20);
    insertAndSetFirstWall(&head, 2, 530, 80, 110, 10);
}
