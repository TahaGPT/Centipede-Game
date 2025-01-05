#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;    // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// function protoypes
void drawPlayer(RenderWindow &window, float player[], Sprite &playerSprite);
void moveBullet(float bullet[], Clock &bulletClock, float player[]);
void drawBullet(RenderWindow &window, float bullet[], Sprite &bulletSprite);
void drawBody(RenderWindow &window, float body[][12][3], Sprite bodySprite[], int bodySize);
void drawmushroom(RenderWindow &window, float bullet[], float body[][12][3], int &score, float mushroom[][3], Sprite &mushroomSprite, int &nm);
void mcollision(float mushroom[][3], float bullet[], int &nm, int &score);
void moveBody(float body[][12][3], Clock &bodyClock, float mushroom[][3], int &nm, float player[]);
bool bmcollision(float body[][12][3], float mushroom[][3], int &nm);
int bcollision(float bullet[], float body[][12][3], int &score);
int death(float body[12][12][3], float player[]);

int main()
{
    srand(time(0));

    // Declaring RenderWindow.
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);

    // Used to resize your window if it's too big or too small. Use according to your needs.
    window.setSize(Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
    // window.setSize(Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
    // window.setSize(Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.

    // Used to position your window on every launch. Use according to your needs.
    window.setPosition(Vector2i(100, 0));

    // Initializing Background Music.
    Music bgMusic;
    bgMusic.openFromFile("Music/DJVI.ogg");
    bgMusic.play();
    bgMusic.setVolume(50);

    // Initializing shooting Music.
    Music shoot;
    shoot.openFromFile("Sound Effects/fire1.wav");

    // score initialization and setting
    int score = 0;
    Font font;
    font.loadFromFile("pacifico/Pacifico.ttf");
    Text scoreT;
    scoreT.setFont(font);
    scoreT.setCharacterSize(30);
    scoreT.setFillColor(Color::Black);
    scoreT.setPosition(25 * boxPixelsX, 1 * boxPixelsY); // Adjust the position as needed
    scoreT.setString("Score: ");

    // Initializing Background.
    Texture backgroundTexture;
    Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/k.jpg");
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setColor(Color(255, 255, 255, 255 * 0.5)); // Reduces Opacity to 25%

    // Initializing Player and Player Sprites.
    float player[2] = {};
    player[x] = (gameColumns / 2) * boxPixelsX;
    player[y] = (25) * boxPixelsY;
    Texture playerTexture;
    Sprite playerSprite;
    playerTexture.loadFromFile("Textures/player.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    // Initializing Bullet and Bullet Sprites.
    float bullet[3] = {};
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
    Clock bulletClock;
    Texture bulletTexture;
    Sprite bulletSprite;
    bulletTexture.loadFromFile("Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    // Initializing mushroom and mushroom Sprites.
    int nm = 20;
    float mushroom[nm + 12][3] = {};
    for (int i = 0; i < nm; i++)
    {
        mushroom[i][x] = (rand() % 26 + 3) * boxPixelsX; // random places
        mushroom[i][y] = (rand() % 24 + 1) * boxPixelsY;
        if (mushroom[i][exists] == false)
        {
            continue;
        }
        mushroom[i][exists] = true;
    }
    for (int i = nm; i < nm + 12; i++)
    {
        mushroom[i][x] = 89 * boxPixelsX; // random places
        mushroom[i][y] = 89 * boxPixelsY;
        if (mushroom[i][exists] == false)
        {
            continue;
        }
        mushroom[i][exists] = true;
    }
    Clock mushroomClock;
    Texture mushroomTexture;
    Sprite mushroomSprite;
    mushroomTexture.loadFromFile("Textures/mushroom.png");
    mushroomSprite.setTexture(mushroomTexture);
    mushroomSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    // Initializing Body and Body Sprites.
    const int bodySize = 12;
    float body[12][bodySize][3] = {}; // hit points

    // for head

    const int st = rand() % 29;
    body[0][0][x] = (st)*boxPixelsX;
    body[0][0][y] = 1;
    // for body
    for (int i = 1; i < bodySize; i++)
    {
        body[0][i][x] = (st + i) * boxPixelsX;
        body[0][i][y] = 1;
    }

    Clock bodyClock;
    Texture bodyTexture[bodySize];
    Sprite bodySprite[bodySize];
    // for head
    bodyTexture[0].loadFromFile("Textures/head.png");
    bodySprite[0].setTexture(bodyTexture[0]);
    // for body
    for (int i = 1; i < bodySize; i++)
    {
        bodyTexture[i].loadFromFile("Textures/body.png");
        bodySprite[i].setTexture(bodyTexture[i]);
        bodySprite[i].setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    }

    while (window.isOpen())
    {

        ///////////////////////////////////////////////////////////////
        //                                                           //
        // Call Your Functions Here. Some have been written for you. //
        // Be vary of the order you call them, SFML draws in order.  //
        //                                                           //
        ///////////////////////////////////////////////////////////////

        // calling functions
        window.draw(backgroundSprite);

        drawPlayer(window, player, playerSprite);

        drawBody(window, body, bodySprite, bodySize);

        drawmushroom(window, bullet, body, score, mushroom, mushroomSprite, nm);

        moveBody(body, bodyClock, mushroom, nm, player);

        mcollision(mushroom, bullet, nm, score);

        // bullet shooting condition
        if (bullet[exists] == true)
        {
            moveBullet(bullet, bulletClock, player);
            drawBullet(window, bullet, bulletSprite);
        }

        string scoreStr = "Score: " + to_string(score);
        scoreT.setString(scoreStr);

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                return 0;
            }
            // player controls
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (player[x] < (29 * boxPixelsX))
                {
                    player[x] += boxPixelsX;
                }
            }

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (player[x] > 0)
                {
                    player[x] -= boxPixelsX;
                }
            }

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if (player[y] > (25 * boxPixelsY))
                {
                    player[y] -= boxPixelsY;
                }
            }

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                if (player[y] < (29 * boxPixelsY))
                {
                    player[y] += boxPixelsY;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                if (bullet[exists] == false)
                {
                    shoot.stop();
                    shoot.play();
                    shoot.setVolume(50);
                    bullet[exists] = true;
                    bullet[x] = player[x];
                    bullet[y] = player[y] - boxPixelsY;
                }
            }
        }
        if (death(body, player))
        {
            bgMusic.stop();
            window.clear();
            scoreT.setCharacterSize(80);
            scoreT.setFillColor(Color::White);
            scoreT.setPosition(10 * boxPixelsX, 10 * boxPixelsY); // Adjust the position as needed
            scoreT.setString("Score: ");
            window.close();
        }
        window.draw(scoreT);
        window.display();
        window.clear();
    }
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Write your functions definitions here. Some have been written for you. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// player displaying definiton
void drawPlayer(RenderWindow &window, float player[], Sprite &playerSprite)
{
    playerSprite.setPosition(player[x], player[y]);
    window.draw(playerSprite);
}

// bullet movement definition
void moveBullet(float bullet[], Clock &bulletClock, float player[])
{
    // Change the speed factor to control the bullet's speed
    const float bulletSpeed = 4.0f;

    if (bulletClock.getElapsedTime().asMicroseconds() < 1)
        return;

    bulletClock.restart();
    bullet[y] -= bulletSpeed; // Increase the speed by multiplying with a speed factor

    if (bullet[y] < -boxPixelsY)
    {
        bullet[exists] = false;
    }
}

// bullet displaying definition
void drawBullet(RenderWindow &window, float bullet[], Sprite &bulletSprite)
{
    bulletSprite.setPosition(bullet[x], bullet[y]);
    window.draw(bulletSprite);
}

// body displaying function
void drawBody(RenderWindow &window, float body[][12][3], Sprite bodySprite[], int bodySize)
{

    for (int i = 0; i < bodySize; i++)
    {

        bodySprite[i].setPosition(body[0][i][x], body[0][i][y]);

        window.draw(bodySprite[i]);
    }
}

int death(float body[12][12][3], float player[])
{
    for (int i = 0; i < 12; i++)
    {
        // FloatRect bodyCo(body[0][i][x], body[0][i][y], boxPixelsX, boxPixelsY); // setting a body function for the collision factor
        //
        // FloatRect bulletCo(bullet[x], bullet[y], boxPixelsX, boxPixelsY); // setting a bullet function for the collision factor
        if (player[x] >= body[0][i][x] && player[y] == body[0][i][y]) // bodyCo.intersects(bulletCo))                                  // checking collsion
        {

            cout << "player : " << player[x] << ", " << player[y] << endl;
            cout << "body : " << body[0][i][x] << ", " << body[0][i][y] << endl;
            return 1;
        }
    }
    return 0;
}

// body and mushroom collision function
bool bmcollision(float body[][12][3], float mushroom[][3], int &nm)
{
    bool collision = 0;
    for (int i = 0; i < 12; i++)
    {
        //  FloatRect bodyCo(body[0][i][x], body[0][i][y], boxPixelsX, boxPixelsY); // setting a body function for the collision factor
        for (int j = 0; j < nm; j++)
        {
            //      FloatRect mushroomCo(mushroom[j][x], mushroom[j][y], boxPixelsX, boxPixelsY); // setting a mushroom function for the collision factor
            if (body[0][i][x] >= mushroom[j][x] && body[0][i][x] < mushroom[j][x] + boxPixelsX &&
                body[0][i][y] == mushroom[j][y] && body[0][i][y] < mushroom[j][y] + boxPixelsY) // bodyCo.intersects(mushroomCo))                                            // checking collsion
            {
                collision = 1;
            }
        }
    }
    return collision;
}

int bcollision(float bullet[], float body[][12][3], int &score)
{
    for (int i = 0; i < 12; i++)
    {
        // FloatRect bodyCo(body[0][i][x], body[0][i][y], boxPixelsX, boxPixelsY); // setting a body function for the collision factor
        //
        // FloatRect bulletCo(bullet[x], bullet[y], boxPixelsX, boxPixelsY); // setting a bullet function for the collision factor
        if (body[0][i][x] >= bullet[x] && body[0][i][x] < bullet[x] + boxPixelsX &&
            body[0][i][y] == bullet[y] && body[0][i][y] < bullet[y] + boxPixelsY) // bodyCo.intersects(bulletCo))                                  // checking collsion
        {
            cout << "bullet : " << bullet[x] << ", " << bullet[y] << endl;
            cout << "body : " << body[0][i][x] << ", " << body[0][i][y] << endl;

            bullet[exists] = false;

            // int temp[3];
            // temp[x] = body[0][i][x];
            // temp[y] = body[0][i][y];
            // body[0][i][exists] =  mushroom[nm + i][x];
            // body[0][i][y] = mushroom[nm + i][y];
            // mushroom[nm + i][x] = temp[x];
            // mushroom[nm + i][y] = temp[y];
            //
            // mushroomSprite.setPosition(mushroom[i][x], mushroom[i][y]);
            // window.draw(mushroomSprite);

            score += 10;
            return i;
        }
    }
    return -1;
}

// mushroom displaying function
void drawmushroom(RenderWindow &window, float bullet[], float body[][12][3], int &score, float mushroom[][3], Sprite &mushroomSprite, int &nm)
{

    for (int i = 0; i < nm; i++)
    {
        mushroomSprite.setPosition(mushroom[i][x], mushroom[i][y]);
        window.draw(mushroomSprite);
    }

    int p = bcollision(bullet, body, score);
    if (p != -1)
    {
        body[0][p][exists] = 89 * boxPixelsX;

        mushroomSprite.setPosition(mushroom[p][x], mushroom[p][y]);
        window.draw(mushroomSprite);
    }
}

// mushroom colisiion with the bullet
void mcollision(float mushroom[][3], float bullet[], int &nm, int &score)
{
    for (int i = 0; i < nm; i++)
    {
        // bullet hits the boundaries of the mushroom

        if (bullet[x] >= mushroom[i][x] && bullet[x] < mushroom[i][x] + boxPixelsX &&
            bullet[y] == mushroom[i][y] && bullet[y] < mushroom[i][y] + boxPixelsY)
        {
            // Collision check
            cout << "bullet: " << bullet[x] << ", " << bullet[y] << endl;
            cout << "Mushroom: " << mushroom[i][x] << ", " << mushroom[i][y] << endl;

            mushroom[i][y] = 89 * boxPixelsY; // mushrooom fly
            bullet[exists] = false;           // Bullet not exist
            score++;
        }
    }
}

// body movement definition
void moveBody(float body[][12][3], Clock &bodyClock, float mushroom[][3], int &nm, float player[])
{

    if (bodyClock.getElapsedTime().asMilliseconds() < 5)
        return;

    bodyClock.restart();

    /* for (int step = 1; step <= 30; ++step)
     {
         for (int i = 0; i < 12; ++i)
         {
             if (body[0][i][x] == 1 && body[0][i][y] == step * boxPixelsY)
             {
                 body[0][i][y] = (step + 1) * boxPixelsY;
             }
             else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == step * boxPixelsY))
             {
                 body[0][i][y] = (step + 1) * boxPixelsY;
             }

             if (body[0][i][y] == (step + 1) * boxPixelsY)
             {
                 body[0][i][x]++;
             }
         }

         for (int i = 0; i < 12; ++i)
         {
             if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == (step + 1) * boxPixelsY)
             {
                 body[0][i][y] = (step + 2) * boxPixelsY;
             }
             else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == (step + 1) * boxPixelsY))
             {
                 body[0][i][y] = (step + 2) * boxPixelsY;
             }

             if (body[0][i][y] == (step + 2) * boxPixelsY)
             {
                 body[0][i][x]--;
             }
         }
     }*/
    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][y] == 1)
        {

            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 1)
        {

            body[0][i][y] = 1 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 1))
        {
            body[0][i][y] = 1 * boxPixelsY;
        }
        if (body[0][i][y] == 1 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if ((body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 1 * boxPixelsY))
        {
            body[0][i][y] = 2 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 1 * boxPixelsY))
        {
            body[0][i][y] = 2 * boxPixelsY;
        }
        if (body[0][i][y] == 2 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 2 * boxPixelsY)
        {

            body[0][i][y] = 3 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 2 * boxPixelsY))
        {
            body[0][i][y] = 3 * boxPixelsY;
        }
        if (body[0][i][y] == 3 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 3 * boxPixelsY)
        {
            body[0][i][y] = 4 * boxPixelsY;
        }

        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 3 * boxPixelsY))
        {
            body[0][i][y] = 4 * boxPixelsY;
        }
        if (body[0][i][y] == 4 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 4 * boxPixelsY)
        {

            body[0][i][y] = 5 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 4 * boxPixelsY))
        {
            body[0][i][y] = 5 * boxPixelsY;
        }
        if (body[0][i][y] == 5 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 5 * boxPixelsY)
        {
            body[0][i][y] = 6 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 5 * boxPixelsY))
        {
            body[0][i][y] = 6 * boxPixelsY;
        }
        if (body[0][i][y] == 6 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 6 * boxPixelsY)
        {

            body[0][i][y] = 7 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 6 * boxPixelsY))
        {
            body[0][i][y] = 7 * boxPixelsY;
        }
        if (body[0][i][y] == 7 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 7 * boxPixelsY)
        {
            body[0][i][y] = 8 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 7 * boxPixelsY))
        {
            body[0][i][y] = 8 * boxPixelsY;
        }
        if (body[0][i][y] == 8 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 8 * boxPixelsY)
        {

            body[0][i][y] = 9 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 8 * boxPixelsY))
        {
            body[0][i][y] = 9 * boxPixelsY;
        }
        if (body[0][i][y] == 9 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 9 * boxPixelsY)
        {
            body[0][i][y] = 10 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 9 * boxPixelsY))
        {
            body[0][i][y] = 10 * boxPixelsY;
        }
        if (body[0][i][y] == 10 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 10 * boxPixelsY)
        {

            body[0][i][y] = 11 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 10 * boxPixelsY))
        {
            body[0][i][y] = 11 * boxPixelsY;
        }
        if (body[0][i][y] == 11 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 11 * boxPixelsY)
        {
            body[0][i][y] = 12 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 11 * boxPixelsY))
        {
            body[0][i][y] = 12 * boxPixelsY;
        }
        if (body[0][i][y] == 12 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 12 * boxPixelsY)
        {

            body[0][i][y] = 13 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 12 * boxPixelsY))
        {
            body[0][i][y] = 13 * boxPixelsY;
        }
        if (body[0][i][y] == 13 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 13 * boxPixelsY)
        {
            body[0][i][y] = 14 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 13 * boxPixelsY))
        {
            body[0][i][y] = 14 * boxPixelsY;
        }
        if (body[0][i][y] == 14 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 14 * boxPixelsY)
        {

            body[0][i][y] = 15 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 14 * boxPixelsY))
        {
            body[0][i][y] = 15 * boxPixelsY;
        }
        if (body[0][i][y] == 15 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 15 * boxPixelsY)
        {
            body[0][i][y] = 16 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 15 * boxPixelsY))
        {
            body[0][i][y] = 16 * boxPixelsY;
        }
        if (body[0][i][y] == 16 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 16 * boxPixelsY)
        {

            body[0][i][y] = 17 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 16 * boxPixelsY))
        {
            body[0][i][y] = 17 * boxPixelsY;
        }
        if (body[0][i][y] == 17 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 17 * boxPixelsY)
        {
            body[0][i][y] = 18 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 17 * boxPixelsY))
        {
            body[0][i][y] = 18 * boxPixelsY;
        }
        if (body[0][i][y] == 18 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 18 * boxPixelsY)
        {

            body[0][i][y] = 19 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 18 * boxPixelsY))
        {
            body[0][i][y] = 19 * boxPixelsY;
        }
        if (body[0][i][y] == 19 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 19 * boxPixelsY)
        {
            body[0][i][y] = 20 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 19 * boxPixelsY))
        {
            body[0][i][y] = 20 * boxPixelsY;
        }
        if (body[0][i][y] == 20 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 20 * boxPixelsY)
        {

            body[0][i][y] = 21 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 20 * boxPixelsY))
        {
            body[0][i][y] = 21 * boxPixelsY;
        }
        if (body[0][i][y] == 21 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }
    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 21 * boxPixelsY)
        {
            body[0][i][y] = 22 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 21 * boxPixelsY))
        {
            body[0][i][y] = 22 * boxPixelsY;
        }
        if (body[0][i][y] == 22 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }
    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 22 * boxPixelsY)
        {

            body[0][i][y] = 23 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 22 * boxPixelsY))
        {
            body[0][i][y] = 23 * boxPixelsY;
        }
        if (body[0][i][y] == 23 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }
    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 23 * boxPixelsY)
        {
            body[0][i][y] = 24 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 23 * boxPixelsY))
        {
            body[0][i][y] = 24 * boxPixelsY;
        }
        if (body[0][i][y] == 24 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }
    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 24 * boxPixelsY)
        {

            body[0][i][y] = 25 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 24 * boxPixelsY))
        {
            body[0][i][y] = 25 * boxPixelsY;
        }
        if (body[0][i][y] == 25 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }
    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 25 * boxPixelsY)
        {
            body[0][i][y] = 26 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 25 * boxPixelsY))
        {
            body[0][i][y] = 26 * boxPixelsY;
        }
        if (body[0][i][y] == 26 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }
    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 26 * boxPixelsY)
        {

            body[0][i][y] = 27 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 26 * boxPixelsY))
        {
            body[0][i][y] = 27 * boxPixelsY;
        }
        if (body[0][i][y] == 27 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 27 * boxPixelsY)
        {
            body[0][i][y] = 28 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 27 * boxPixelsY))
        {
            body[0][i][y] = 28 * boxPixelsY;
        }
        if (body[0][i][y] == 28 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 28 * boxPixelsY)
        {

            body[0][i][y] = 29 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 28 * boxPixelsY))
        {
            body[0][i][y] = 29 * boxPixelsY;
        }
        if (body[0][i][y] == 29 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }
    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 29 * boxPixelsY)
        {
            body[0][i][y] = 30 * boxPixelsY;
        }
        else if (bmcollision(body, mushroom, nm) && (body[0][i][y] == 29 * boxPixelsY))
        {
            body[0][i][y] = 30 * boxPixelsY;
        }
        if (body[0][i][y] == 30 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    // while (!death(body, player))
    // {
    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 30 * boxPixelsY)
        {

            body[0][i][x]++;
        }
        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 30 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }
    // }
}
/*void moveBody(float body[][12][3], Clock &bodyClock)
{

    if (bodyClock.getElapsedTime().asMilliseconds() < 5)
        return;

    bodyClock.restart();

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][y] == 1)
        {

            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 1)
        {

            body[0][i][y] = 1 * boxPixelsY;
        }
        if (body[0][i][y] == 1 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 1 * boxPixelsY)
        {
            body[0][i][y] = 2 * boxPixelsY;
        }
        if (body[0][i][y] == 2 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 2 * boxPixelsY)
        {

            body[0][i][y] = 3 * boxPixelsY;
        }
        if (body[0][i][y] == 3 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 3 * boxPixelsY)
        {
            body[0][i][y] = 4 * boxPixelsY;
        }
        if (body[0][i][y] == 4 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 4 * boxPixelsY)
        {

            body[0][i][y] = 5 * boxPixelsY;
        }
        if (body[0][i][y] == 5 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 5 * boxPixelsY)
        {
            body[0][i][y] = 6 * boxPixelsY;
        }
        if (body[0][i][y] == 6 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 6 * boxPixelsY)
        {

            body[0][i][y] = 7 * boxPixelsY;
        }
        if (body[0][i][y] == 7 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 7 * boxPixelsY)
        {
            body[0][i][y] = 8 * boxPixelsY;
        }
        if (body[0][i][y] == 8 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 8 * boxPixelsY)
        {

            body[0][i][y] = 9 * boxPixelsY;
        }
        if (body[0][i][y] == 9 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 9 * boxPixelsY)
        {
            body[0][i][y] = 10 * boxPixelsY;
        }
        if (body[0][i][y] == 10 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 10 * boxPixelsY)
        {

            body[0][i][y] = 11 * boxPixelsY;
        }
        if (body[0][i][y] == 11 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 11 * boxPixelsY)
        {
            body[0][i][y] = 12 * boxPixelsY;
        }
        if (body[0][i][y] == 12 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 12 * boxPixelsY)
        {

            body[0][i][y] = 13 * boxPixelsY;
        }
        if (body[0][i][y] == 13 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 13 * boxPixelsY)
        {
            body[0][i][y] = 14 * boxPixelsY;
        }
        if (body[0][i][y] == 14 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 14 * boxPixelsY)
        {

            body[0][i][y] = 15 * boxPixelsY;
        }
        if (body[0][i][y] == 15 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 15 * boxPixelsY)
        {
            body[0][i][y] = 16 * boxPixelsY;
        }
        if (body[0][i][y] == 16 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 16 * boxPixelsY)
        {

            body[0][i][y] = 17 * boxPixelsY;
        }
        if (body[0][i][y] == 17 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 17 * boxPixelsY)
        {
            body[0][i][y] = 18 * boxPixelsY;
        }
        if (body[0][i][y] == 18 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 18 * boxPixelsY)
        {

            body[0][i][y] = 19 * boxPixelsY;
        }
        if (body[0][i][y] == 19 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    for (int i = 0; i < 12; i++)
    {

        if (body[0][i][x] == 29 * boxPixelsX && body[0][i][y] == 19 * boxPixelsY)
        {
            body[0][i][y] = 20 * boxPixelsY;
        }
        if (body[0][i][y] == 20 * boxPixelsY)
        {
            body[0][i][x]--;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        if (body[0][i][x] == 1 && body[0][i][y] == 20 * boxPixelsY)
        {

            body[0][i][y] = 21 * boxPixelsY;
        }
        if (body[0][i][y] == 21 * boxPixelsY)
        {
            body[0][i][x]++;
        }
    }

    if (body[0][0][x] == 1 && body[0][y][0] == 2 * boxPixelsY)
    {
        body[0][0][y] = 3 * boxPixelsY;
    }
    if (body[0][0][y] == 3 * boxPixelsY)
    {
        body[0][0][x]++;
    }
    if (body[0][0][x] == 23 * boxPixelsX && body[0][0][y] == 3 * boxPixelsY)
    {
        body[0][0][y] = 4 * boxPixelsY;
    }
    if (body[0][0][y] == 4 * boxPixelsY)
    {
        body[0][0][x]--;
    }
    if (body[0][0][x] == 1 && body[0][0][y] == 4 * boxPixelsY)
    {
        body[0][0][y] = 5 * boxPixelsY;
    }
    if (body[0][0][y] == 5 * boxPixelsY)
    {
        body[0][0][x]++;
    }
    if (body[0][0][x] == 23 * boxPixelsX && body[0][0][y] == 5 * boxPixelsY)
    {
        body[0][0][y] = 6 * boxPixelsY;
    }
    if (body[0][0][y] == 6 * boxPixelsY)
    {
        body[0][0][x]--;
    }
    if (body[0][0][x] == 1 && body[0][0][y] == 6 * boxPixelsY)
    {
        body[0][0][y] = 7 * boxPixelsY;
    }
    if (body[0][0][y] == 7 * boxPixelsY)
    {
        body[0][0][x]++;
    }
    if (body[0][0][x] == 23 * boxPixelsX && body[0][0][y] == 7 * boxPixelsY)
    {
        body[0][0][y] = 8 * boxPixelsY;
    }
    }
    */
