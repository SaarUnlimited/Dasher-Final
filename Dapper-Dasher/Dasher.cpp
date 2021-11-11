#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime , int maxFrame)
{
    // Update running Time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    int windowDimensions[2];
    windowDimensions[0] = 518;
    windowDimensions[1] = 400;


    //Initialize The Window
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    //Acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};

    //nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    
    const int sizeOfNebulae{10};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
        nebulae[i].pos.x = windowDimensions[0] + i * 325;
    }

    float finishLine{ nebulae[sizeOfNebulae - 1].pos.x };

    //Nebula X Velocity (pixels/second)
    int nebVel{-244};

    // alex variables
    Texture2D alex = LoadTexture("textures/alex.png");
    AnimData alexData;
    alexData.rec.width = alex.width / 6;
    alexData.rec.height = alex.height;
    alexData.rec.x = 0;
    alexData.rec.y = 0;
    alexData.pos.x = windowDimensions[0] / 2 - alexData.rec.width / 2;
    alexData.pos.y = windowDimensions[1] - alexData.rec.height;
    alexData.frame = 0;
    alexData.updateTime = 1.0 / 12.0;
    alexData.runningTime = 0.0;

    // Is The Rectangle in The Air?
    bool isInAir{};
    // Jump  velocity (pixels/s)/s
    const int jumpVel{-600};

    int velocity{0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        //Delta Time (Time since last frame)
        const float dT{GetFrameTime()};

        //Start Drawing
        BeginDrawing();
        ClearBackground(WHITE);

        //Scroll Background
        bgX -= 20 * dT;
        if (bgX <= -background.width*2.1)
        {
           bgX = 0.0;
        }

        //Scroll Midground
        mgX -= 40 * dT;
        if (mgX <= -midground.width *2.1)
        {
            mgX = 0.0;
        }

        //Scroll Foreground
        fgX -= 80 * dT;
        if (fgX <= -foreground.width *2.1)
        {
            fgX = 0.0;
        }
        //Draw the Background
        Vector2 bgAPos{bgX, 0.0};
        DrawTextureEx(background, bgAPos, 0.0, 2.1, WHITE);
        Vector2 bgBPos{bgX + background.width*2.1, 0.0};
        DrawTextureEx(background, bgBPos, 0.0, 2.1, WHITE);

        //Draw the Midground
        Vector2 mgAPos{mgX, 0.0};
        DrawTextureEx(midground, mgAPos, 0.0, 2.1, WHITE);
        Vector2 mgBPos{mgX + midground.width*2.1, 0.0};
        DrawTextureEx(midground, mgBPos, 0.0, 2.1, WHITE);

        //Draw the Foreground
        Vector2 fgAPos{fgX, 0.0};
        DrawTextureEx(foreground, fgAPos, 0.0, 2.1, WHITE);
        Vector2 fgBPos{fgX + foreground.width*2.1, 0.0};
        DrawTextureEx(foreground, fgBPos, 0.0, 2.1, WHITE);

        //Perform Ground Check
        if (isOnGround(alexData, windowDimensions[1]))
        {
            // Rectangle is on the ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // rectangle is in the air
            velocity += gravity * dT;
            isInAir = true;
        }
        //Jump checking
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            //  Update each nebula Position
            nebulae[i].pos.x += nebVel * dT;
        }

        //Update finishLine
         finishLine += nebVel * dT;

        //Update alex Position
        alexData.pos.y += velocity * dT;

        //update alex's animation frame
        if (!isInAir)
        {
            alexData = updateAnimData(alexData, dT, 5);
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle alexRec{
                alexData.pos.x,
                alexData.pos.y,
                alexData.rec.width,
                alexData.rec.height
            };
            if (CheckCollisionRecs(nebRec, alexRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            // Loose Game
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 42, RED);
        }
        else if (alexData.pos.x >= finishLine)
        
        {
            // Win The Game
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 42, RED);
        }
        
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                //Draw nebula
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            //Draw alex
            DrawTextureRec(alex, alexData.rec, alexData.pos, WHITE);
        }

        //Stop Drawing
        EndDrawing();
    }
    UnloadTexture(alex);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
} 