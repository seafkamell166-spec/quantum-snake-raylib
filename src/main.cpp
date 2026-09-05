#include <iostream>
#include <raylib.h>
#include <deque>
#include "raymath.h"
using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};


Color neonGreen = { 0, 255, 0, 255 }; 
Color darkGreen2 = { 0, 150, 0, 255 };  

int gamestate = 0;      // 0 = Menu | 1 = levels | 2 = Game | 3 = Restart
int level;      // 1 = Easy | 2 = Medium | 3 = Insane

int cellcount = 25;
int cellsize = 30;

int score = 0;
int highScore = 0;

double lastUpdateTime = 0;

bool Playing = true;

bool ateFruit = false;

bool soundPlayed = false;
bool HighScorePlayed = false;

Sound eatSound;
Sound loseSound;
Sound newHighScoreSound;



bool ElementInDeque(Vector2 element , deque<Vector2> deque){ // checks if an element is inside of the deque
    for(unsigned int i = 0; i < deque.size(); i++){
        if(Vector2Equals(element , deque[i]))
            return true;
    }
    return false;
}

bool playing = true;

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Button{
public:
    Texture2D texture; // texture of button
    Vector2 position; // x and y of buttons

    Button(const char* imagePath , Vector2 imagePosition , float scale){ // constructor
        Image image = LoadImage(imagePath); // button image
        int newWidth = image.width * scale;
        int newHeight = image.width * scale;
        ImageResize(&image , newWidth, newHeight);

        texture = LoadTextureFromImage(image); // loads the image into the texture
        position = imagePosition; // sets the position as the given pos.
        UnloadImage(image);
    }

    ~Button(){UnloadTexture(texture);} // deconstructor to free memory

    void Draw(){ // function to draw the button
        DrawTextureV(texture , position , WHITE);
    }

    bool isPressed(Vector2 mousePos , bool mousePressed){ // function to check if button is pressed

        Rectangle rect = {position.x , position.y , (float)texture.width , (float)texture.height}; // invisible rectangle to check collision

        if(CheckCollisionPointRec(mousePos , rect) && mousePressed){
            return true;
        }
        return false;
    }
};

class Food{
public:
    Texture2D texture;
    Vector2 position;
    int randomFood = GetRandomValue(1 , 3); // to get a random food

    Food(deque<Vector2> snakeBody) {
        RandomizeTexture();
        position = GenerateRandomPosition(snakeBody);
    }

    ~Food(){UnloadTexture(texture);} // deconstructor to free memory

    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody){
        float x;
        float y;
        do{
            x = GetRandomValue(0 , cellcount - 1);
            y = GetRandomValue(0 , cellcount - 1);
        }while(ElementInDeque({x , y} , snakeBody)); // doesnt spawn food on snake
        return {x , y};
    }

    void RandomizeTexture(){
        randomFood = GetRandomValue(1 , 3);
        Image image;

        // To set an image to a random food
        if(randomFood == 1)
            image = LoadImage("src/food.png");
        else if (randomFood == 2)
            image = LoadImage("src/Food2.png");
        else
            image = LoadImage("src/Food3.png");

        ImageResize(&image, cellsize, cellsize);

        UnloadTexture(texture);          
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
    }

    void Draw() { // Draws the food
        DrawTextureV(texture , { position.x * cellsize, position.y * cellsize } , WHITE);
    }
    
};

class Snake{
public:
    deque<Vector2> body = {{7, 10} , {6 , 10} , {5 , 10}}; // deque to hold snake body  
    Vector2 direction = {1 ,0}; // vector2 to hold direction
    
    
    void Draw(){ // function to draw the snake
    for(int i = 0; i < body.size(); i++){

        float x = body[i].x; // x position of element in deque
        float y = body[i].y; // y position of element in deque

        Rectangle rect = {x * cellsize , y * cellsize , (float) cellsize , (float) cellsize};

        if (i == 0){ // if its the snake head
            DrawRectangleRounded(rect , 0.5 , 6 , neonGreen); // draw with different color other than tail
            
            // DRAW EYES ON SNAKE HEAD
            // We need to figure out where to put the eyes based on which way the snake is moving
            // The snake can move in 4 directions: right, left, up, down
            
            if(direction.x == 1){ // Moving RIGHT
                // When moving right, put eyes on the RIGHT side of the head
                // cellsize * 0.7 = 70% across the cell (toward the right)
                // cellsize * 0.3 = 30% down from top (top eye)
                // cellsize * 0.7 = 70% down from top (bottom eye)
                DrawCircle(x * cellsize + cellsize * 0.7f, y * cellsize + cellsize * 0.3f, 3, BLACK); // Top-right eye
                DrawCircle(x * cellsize + cellsize * 0.7f, y * cellsize + cellsize * 0.7f, 3, BLACK); // Bottom-right eye
            }
            else if(direction.x == -1){ // Moving LEFT
                // When moving left, put eyes on the LEFT side of the head
                // cellsize * 0.3 = 30% across the cell (toward the left)
                DrawCircle(x * cellsize + cellsize * 0.3f, y * cellsize + cellsize * 0.3f, 3, BLACK); // Top-left eye
                DrawCircle(x * cellsize + cellsize * 0.3f, y * cellsize + cellsize * 0.7f, 3, BLACK); // Bottom-left eye
            }
            else if(direction.y == -1){ // Moving UP
                // When moving up, put eyes on the TOP of the head
                DrawCircle(x * cellsize + cellsize * 0.3f, y * cellsize + cellsize * 0.3f, 3, BLACK); // Left-top eye
                DrawCircle(x * cellsize + cellsize * 0.7f, y * cellsize + cellsize * 0.3f, 3, BLACK); // Right-top eye
            }
            else{ // Moving DOWN (direction.y == 1)
                // When moving down, put eyes on the BOTTOM of the head
                DrawCircle(x * cellsize + cellsize * 0.3f, y * cellsize + cellsize * 0.7f, 3, BLACK); // Left-bottom eye
                DrawCircle(x * cellsize + cellsize * 0.7f, y * cellsize + cellsize * 0.7f, 3, BLACK); // Right-bottom eye
            }
        }
        else{ // if its the snake body (not head)
            DrawRectangleRounded(rect , 0.5 , 6 , darkGreen);
        }
    }
}

    void Update(){

        body.push_front(Vector2Add(body[0] , direction)); // adds the head of the snake in the desired direction 

        if(ateFruit){ // Checks if the snake just ate a fruit
            ateFruit = false; // sets it to false, so it starts adding to tail again
        }else{
            body.pop_back(); // removes an element from back of the tail
        }
    }
};

class Game{
public:
    Snake snake; // instant of snake
    Food food = Food(snake.body); // instant of food

    Game(){ // constructor to initialize sounds
        eatSound = LoadSound("src\\SnakeEat.mp3");
        loseSound = LoadSound("src\\SnakeLose.mp3");
        newHighScoreSound = LoadSound("src\\HighScore.mp3");
    }

    ~Game(){ // deconstructor to unload sounds
        UnloadSound(eatSound);
        UnloadSound(loseSound);
        UnloadSound(newHighScoreSound);
    }

    void Draw(){ // calls the draw functions of the snake and food
        snake.Draw();
        food.Draw();
    }

    void Update(){ // updates snake and checks for collisions
        snake.Update();
        CheckCollisionWithFood();
        CheckingCollisionWithEdge();
        CheckingCollisionWithTail();
    }

    void CheckCollisionWithFood(){
        if(Vector2Equals(snake.body[0] , food.position)){ // if head of snake in the same cell as the food position
            food.position = food.GenerateRandomPosition(snake.body); // generates another random food position
            ateFruit = true; // sets ateFruit to true

            /* adds different score depending on the food  */
            if(food.randomFood == 1)
                score++;
            else if(food.randomFood == 2)
                score += 2;
            else
                score += 3;

            PlaySound(eatSound); // play the snake eat sound effect
            food.RandomizeTexture(); // generates a random food texture

            if(score > highScore){ // checks if highscore is beaten
                highScore = score; // sets new high score to final score

                if(!HighScorePlayed){ // if its a new highscore, a sound effect is played (HO HO HO)
                    PlaySound(newHighScoreSound);
                    HighScorePlayed = true; // sets it to true so it doesnt keep playing every fruit eaten
                }
            }
        }
    }

    void CheckingCollisionWithEdge(){ // checks if snake head collides with wall
        if(snake.body[0].x < 0 || snake.body[0].x >= cellcount ||
           snake.body[0].y < 0 || snake.body[0].y >= cellcount){ // checks if head collides with any edge
            GameOver(); // game over function
        }
    }

    void CheckingCollisionWithTail(){ // checks collision with tail of snake
        deque<Vector2> tailOfSnake = snake.body; // set a new variable to the snake body right now

        tailOfSnake.pop_front(); // removes the head of the snake

        if(ElementInDeque(snake.body[0] , tailOfSnake)){ // checks if head is in same cell as any body part
            GameOver(); 
        }
    }

    void GameOver(){ // game over screen
        PlaySound(loseSound); // plays the game over sound effect
        Playing = false; // sets playing to false
        gamestate = 3; // sets gamestate to the game over screen
    }
};

int main()
{
    InitWindow(cellcount * cellsize, cellcount * cellsize, "Quantum Snake 2.0"); // create the window
    InitAudioDevice(); // starts audio devices (to play sounds)
    Image bg = LoadImage("src/Menu_Background.png"); // loads the menu background image

    Texture2D MenuBackground = LoadTextureFromImage(bg); // sets the image to texture

    Rectangle source = {0, 0, (float) MenuBackground.width, (float) MenuBackground.height}; // create a rectangele source for image
    Rectangle dest = {0, 0, (float) GetScreenWidth(), (float) GetScreenHeight()}; // create a rectangele destination for image
    UnloadImage(bg); // unload the image

    Music menuMusic = LoadMusicStream("src/Menu_music.mp3.mp3"); // loads the menu music
    PlayMusicStream(menuMusic);
    SetMusicVolume(menuMusic, 0.20f); // default volume
    bool soundMuted = false;


    /*           Creates the needed buttons           */
    Button startButton("src\\Start.png" , {375 , 140} , 0.5); 
    Button ExitButton("src\\Exit.png" , {375 , 370} , 0.5);

    Button level1("src\\Level_1.png" , {385 , 150} , 0.45);
    Button level2("src\\Level_2.png" , {385 , 320} , 0.45);
    Button insaneLevel("src\\Insane.png" , {385 , 490} , 0.45);

    Button SoundOn("src\\SoundOn.png" , {600 , 605} , 0.29);
    Button SoundOff("src\\SoundOff.png" , {600 , 605} , 0.29);
    /*           Creates the needed buttons           */



    Image bg2 = LoadImage("src\\Game_Background.png"); // loads the game background grid image
    Texture2D GameBackground = LoadTextureFromImage(bg2); // sets it to texure
    UnloadImage(bg2); // unload image (not needed anymore)
    
   
    Game game; // new instant of game class

    bool exit = false; // boolean for closing the window
    string playerName = ""; // player name string
    bool nameEntered = false; // checks if name is empty
    


    while (!WindowShouldClose() && exit == false) // GAME LOOP!!
    {
        UpdateMusicStream(menuMusic);
        Vector2 mousePositon = GetMousePosition(); // gets the mouse position
        BeginDrawing(); // starts drawing
        ClearBackground(WHITE);

        

        if(soundMuted) {
            SetMusicVolume(menuMusic, 0.0f);
            SetSoundVolume(eatSound, 0.0f);
            SetSoundVolume(loseSound, 0.0f);
            SetSoundVolume(newHighScoreSound, 0.0f);
        
        }else {
            SetMusicVolume(menuMusic, 0.20f);  
            SetSoundVolume(eatSound, 1.0f);
            SetSoundVolume(loseSound, 1.0f);
            SetSoundVolume(newHighScoreSound, 1.0f);
    }




        if (gamestate == 0) { // MENU
            DrawTexturePro(MenuBackground, source, dest, {0,0}, 0.0f, WHITE); // draws the background image menu
            if(!soundMuted){
                SoundOn.Draw();
                if(SoundOn.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                    soundMuted = true;
                }
            }else{
                SoundOff.Draw();
                if(SoundOff.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                    soundMuted = false;
                }
            }
            if(!nameEntered){ // checks if username is entered
                int key = GetCharPressed(); // gets input

                while (key > 0){ 
                    if ((key >= 32) && (key <= 125) && playerName.length() < 12) // checks if key is english alphabet , and less thean 12 characters
                        playerName += (char)key; // adds it to the playername string
                    key = GetCharPressed(); // sets key to the key input
                }

                if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty()) // to delete from playername if needed
                    playerName.pop_back();

                DrawRectangleRounded({200, 30, 400, 200},0.2f, 8,Fade(green, 0.9f));
                DrawText("ENTER YOUR NAME", 255, 45, 28, darkGreen);
                DrawRectangleRounded({240, 95, 320, 50} , 0.3f , 8 , Fade(WHITE, 0.95f));
                DrawRectangleRoundedLines( {240, 95, 320, 50} ,0.3f, 8 , darkGreen);

                DrawText(playerName.c_str(), 255, 110, 30, BLACK);
                DrawText("Press ENTER to continue", 245, 160, 20, DARKGRAY);

                if(IsKeyPressed(KEY_ENTER) && !playerName.empty())
                    nameEntered = true;
            }
            else{
                DrawRectangleRounded({30, 25, 280, 45},0.3f, 8,Fade(GREEN, 0.6f));
                DrawRectangleRoundedLines({30, 25, 280, 45},0.3f,8,BLACK);
                
                DrawText(TextFormat("Player: %s", playerName.c_str()),91, 37, 24,BLACK);
                

                startButton.Draw();
                ExitButton.Draw();
                

                if(startButton.isPressed(mousePositon, IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
                    gamestate = 1;

                if(ExitButton.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
                    exit = true;
            }
        }
        else if(gamestate == 1){ // levels

            DrawTexturePro(MenuBackground, source, dest, {0,0}, 0.0f, WHITE);

            DrawRectangleRounded({30, 25, 280, 45},0.3f, 8,Fade(GREEN, 0.6f));
            DrawRectangleRoundedLines({30, 25, 280, 45},0.3f,8,BLACK);
            DrawText(TextFormat("High Score: %i", highScore),80, 37, 30,BLACK);

            if(!soundMuted){
                SoundOn.Draw();
                if(SoundOn.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                    soundMuted = true;
                }
            }else{
                SoundOff.Draw();
                if(SoundOff.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                    soundMuted = false;
                }
            }

            level1.Draw();
            level2.Draw();
            insaneLevel.Draw();
            

            if(level1.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                level = 1;
                gamestate = 2;
            }
            if(level2.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                level = 2;
                gamestate = 2;
            }
            if(insaneLevel.isPressed(mousePositon , IsMouseButtonPressed(MOUSE_BUTTON_LEFT))){
                level = 3;
                gamestate = 2;
            }
        }
        else if (gamestate == 2){ // GAME

            DrawTexture(GameBackground , 0 , 0 , WHITE);
            DrawText(TextFormat("Score: %i", score), 20, 20, 30, BLACK);

            game.Draw();

            double speed = 0.2; 
            if (level == 2) speed = 0.11;
            if (level == 3) speed = 0.05;

            if (eventTriggered(speed))
                game.Update();

            if(IsKeyPressed(KEY_D) && game.snake.direction.x != -1)
                game.snake.direction = {1 , 0};
            if(IsKeyPressed(KEY_A) && game.snake.direction.x != 1)
                game.snake.direction = {-1 , 0};
            if(IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
                game.snake.direction = {0 , -1};
            if(IsKeyPressed(KEY_S) && game.snake.direction.y != -1)
                game.snake.direction = {0 , 1};
        }
        else{ // GAME OVER

            DrawTexture(GameBackground , 0 , 0 , WHITE);
            game.Draw();

            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.65f));

            DrawText("GAME OVER", 200, 200, 60, RED);
            DrawText(TextFormat("Player: %s", playerName.c_str()), 250, 260, 25, WHITE);
            DrawText(TextFormat("Final Score: %i", score), 250, 300, 30, WHITE);
            DrawText(TextFormat("High Score: %i", highScore), 250, 520, 30, WHITE);
            DrawText("Press R to Restart", 230, 360, 25, GRAY);
            DrawText("Press Esc to exit ", 230, 390, 20, BLACK);

            if(IsKeyPressed(KEY_R)){
                game.snake.body = {{6 , 9} , {5 , 9} , {4 , 9}};
                game.snake.direction = {1 , 0};
                game.food.position = game.food.GenerateRandomPosition(game.snake.body);
                score = 0;
                lastUpdateTime = 0;
                ateFruit = false;
                HighScorePlayed = false;
                gamestate = 0;
            }
        }

        EndDrawing();
    }

    UnloadMusicStream(menuMusic);
    UnloadTexture(MenuBackground);
    UnloadTexture(GameBackground);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
