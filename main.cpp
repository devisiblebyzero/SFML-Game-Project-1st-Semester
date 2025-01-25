#include <SFML/Graphics.hpp> // SFML Version 2.6.1
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

// Game state variables
bool startScreen = true;
bool tutorialScreen = false;
bool isGameRunning = false;
bool isGamePaused = false;
bool isGameOver = false;

bool gameOverSoundPlayed = false;

// Game variables
int highScore = 0;
int currentScore = 0;
int bulletCount = 1000; // Number of bullets
int bulletRemaining = bulletCount; // Remaining bullets
int lives = 3; // Number of lives

// For background frames handling
int frame = 1;
int pausedFrame = 1;

float playerRadius = 0; // For collision detection
float playerSpeed = 250.0f; // Pixels per second
float rotationSpeed = 250.0f; // Degrees per second
float angle = 0.0f; // Initial rotation angle of the player (in degrees)
float radians = 0.0f; // Initial rotation angle of the player (in radians)

float asteroidSpeed = 150.0f; // Pixels per second
float asteroidRotationSpeed = 0.f; // Degrees per second (will be randomized later)
float asteroidScale = 0.f; // Scale of the asteroid (will be randomized later)

float bulletSpeed = 700.0f; // Pixels per second

// Window dimensions
float windowWidth = static_cast<float>(VideoMode::getDesktopMode().width);
float windowHeight = static_cast<float>(VideoMode::getDesktopMode().height);

// Vectors
vector<pair<Sprite, float>> asteroids;
vector<pair<CircleShape, Vector2f>> bullets;

// Clocks and clocks variables
Clock movementClock; // For player, bullet, asteroid movement
Clock asteroidSpawnClock; // For asteroid spawning
Clock scoreClock; // For score incrementation
Clock bulletCooldownClock; // For bullet cooldown

float deltaTime = 0.0f; // For player, bullet, asteroid movement
float asteroidSpawnTime = 1.5f; // Spawn an asteroid every 1.5 second
float bulletCooldownTime = 0.1f; // Cooldown time between bullets

int main() {

    srand(static_cast<unsigned int>(time(0)));

    RenderWindow window(VideoMode(static_cast<unsigned int>(windowWidth), static_cast<unsigned int>(windowHeight)), "Game Project", Style::Fullscreen);
    window.setFramerateLimit(60);

    Font font; // Load font
    if (!font.loadFromFile("Assets/Rushton.ttf")) {
        cerr << "Error loading font" << endl;
    }

    // High score
    fstream highScoreFile("highscore.txt", ios::in | ios::out);
    if (!highScoreFile) {
        highScoreFile.open("highscore.txt", ios::out);
        highScoreFile << 0;
        highScoreFile.close();
        highScore = 0;
    }
    else {
        highScoreFile >> highScore;
        highScoreFile.close();
    }

    // ------------------------------------------------------- TEXT OBJECTS -------------------------------------------------------

    // ---------------------------------------------- Start screen texts ----------------------------------------------
    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setString("High Score: " + to_string(highScore));
    highScoreText.setCharacterSize(80);
    highScoreText.setFillColor(Color::White);
    highScoreText.setOutlineThickness(1.0f);
    highScoreText.setOutlineColor(Color::Red);
    highScoreText.setOrigin(highScoreText.getLocalBounds().width / 2, highScoreText.getLocalBounds().height / 2);
    highScoreText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 - 50));

    Text startText;
    startText.setFont(font);
    startText.setString("Start");
    startText.setCharacterSize(50);
    startText.setFillColor(Color::White);
    startText.setOutlineThickness(1.0f);
    startText.setOutlineColor(Color::Red);
    startText.setOrigin(startText.getLocalBounds().width / 2, startText.getLocalBounds().height / 2);
    startText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

    Text tutorialText;
    tutorialText.setFont(font);
    tutorialText.setString("Tutorial");
    tutorialText.setCharacterSize(50);
    tutorialText.setFillColor(Color::White);
    tutorialText.setOutlineThickness(1.0f);
    tutorialText.setOutlineColor(Color::Red);
    tutorialText.setOrigin(tutorialText.getLocalBounds().width / 2, tutorialText.getLocalBounds().height / 2);
    tutorialText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 50));

    Text quitText;
    quitText.setFont(font);
    quitText.setString("Quit");
    quitText.setCharacterSize(50);
    quitText.setFillColor(Color::White);
    quitText.setOutlineThickness(1.0f);
    quitText.setOutlineColor(Color::Red);
    quitText.setOrigin(quitText.getLocalBounds().width / 2, quitText.getLocalBounds().height / 2);
    quitText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 100));

    Text creditsText;
    creditsText.setFont(font);
    creditsText.setString("Made By:\nAbdullah Al Maruf\nMohammed Rahbar Tahsin Hoque\nPuspak Barua");
    creditsText.setCharacterSize(50);
    creditsText.setFillColor(Color::White);
    creditsText.setOutlineThickness(1.0f);
    creditsText.setOutlineColor(Color(rand() % 256, rand() % 256, rand() % 256));
    creditsText.setOrigin(0, creditsText.getLocalBounds().height);
    creditsText.setPosition(Vector2f(25, windowHeight - 25));

    // ---------------------------------------------- Tutorial screen texts ----------------------------------------------
    Text tutorialContentText;
    tutorialContentText.setFont(font);
    tutorialContentText.setString("Use A & D to control direction.\nPress spacebar to shoot bullets.\nPress Esc during gameplay to pause.\n\n10 points are awarded when asteroids collide with each other.\n\nShooting the asteroids awards 15 points.\n\nYou also get 1 point for every second you survive.");
    tutorialContentText.setCharacterSize(50);
    tutorialContentText.setFillColor(Color::White);
    tutorialContentText.setOutlineThickness(1.0f);
    tutorialContentText.setOutlineColor(Color::Red);
    tutorialContentText.setOrigin(tutorialContentText.getLocalBounds().width / 2, tutorialContentText.getLocalBounds().height / 2);
    tutorialContentText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

    Text returnText;
    returnText.setFont(font);
    returnText.setString("Return");
    returnText.setCharacterSize(50);
    returnText.setFillColor(Color::White);
    returnText.setOutlineThickness(1.0f);
    returnText.setOutlineColor(Color::Red);
    returnText.setOrigin(0, returnText.getLocalBounds().height);
    returnText.setPosition(Vector2f(25, windowHeight - 25));

    // ---------------------------------------------- Pause screen texts ----------------------------------------------
    Text resumeText;
    resumeText.setFont(font);
    resumeText.setString("Resume");
    resumeText.setCharacterSize(50);
    resumeText.setFillColor(Color::White);
    resumeText.setOutlineThickness(1.0f);
    resumeText.setOutlineColor(Color::Red);
    resumeText.setOrigin(resumeText.getLocalBounds().width / 2, resumeText.getLocalBounds().height / 2);
    resumeText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

    Text quitToMenuText;
    quitToMenuText.setFont(font);
    quitToMenuText.setString("Quit to Menu");
    quitToMenuText.setCharacterSize(50);
    quitToMenuText.setFillColor(Color::White);
    quitToMenuText.setOutlineThickness(1.0f);
    quitToMenuText.setOutlineColor(Color::Red);
    quitToMenuText.setOrigin(quitToMenuText.getLocalBounds().width / 2, quitToMenuText.getLocalBounds().height / 2);
    quitToMenuText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 50));

    // ---------------------------------------------- Game screen texts ----------------------------------------------
    Text currentScoreText;
    currentScoreText.setFont(font);
    currentScoreText.setString("Score: " + to_string(currentScore)); // Must update after every frame
    currentScoreText.setCharacterSize(50);
    currentScoreText.setFillColor(Color::White);
    currentScoreText.setOutlineThickness(1.0f);
    currentScoreText.setOutlineColor(Color::Red);
    currentScoreText.setPosition(Vector2f(25, 10));

    // Add highScoreText to the game screen and reposition, resize it

    Text bulletCountText;
    bulletCountText.setFont(font);
    bulletCountText.setString("Bullets: " + to_string(bulletRemaining)); // Must update after every frame
    bulletCountText.setCharacterSize(50);
    bulletCountText.setFillColor(Color::White);
    bulletCountText.setOutlineThickness(1.0f);
    bulletCountText.setOutlineColor(Color::Red);
    bulletCountText.setOrigin(bulletCountText.getLocalBounds().width, 0);
    bulletCountText.setPosition(Vector2f(windowWidth - 25, 10));

    Text livesText;
    livesText.setFont(font);
    livesText.setString("Lives: " + to_string(lives)); // Must update after player collision
    livesText.setCharacterSize(70);
    livesText.setFillColor(Color::White);
    livesText.setOutlineThickness(2.0f);
    livesText.setOutlineColor(Color::Red);
    livesText.setOrigin(livesText.getLocalBounds().width / 2, livesText.getLocalBounds().height / 2);
    livesText.setPosition(Vector2f(windowWidth / 2, 20));

    // ---------------------------------------------- Game over texts ----------------------------------------------
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(100);
    gameOverText.setFillColor(Color::White);
    gameOverText.setOutlineThickness(2.0f);
    gameOverText.setOutlineColor(Color::Red);
    gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2, gameOverText.getLocalBounds().height / 2);
    gameOverText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 - 100));

    Text yourScoreText;
    yourScoreText.setFont(font);
    yourScoreText.setString("Your Score: " + to_string(currentScore)); // Must update after game over
    yourScoreText.setCharacterSize(50);
    yourScoreText.setFillColor(Color::White);
    yourScoreText.setOutlineThickness(1.0f);
    yourScoreText.setOutlineColor(Color::Red);
    yourScoreText.setOrigin(yourScoreText.getLocalBounds().width / 2, yourScoreText.getLocalBounds().height / 2);
    yourScoreText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

    // Add highScoreText to the game over screen and reposition, resize it

    Text playAgainText;
    playAgainText.setFont(font);
    playAgainText.setString("Play Again");
    playAgainText.setCharacterSize(50);
    playAgainText.setFillColor(Color::White);
    playAgainText.setOutlineThickness(1.0f);
    playAgainText.setOutlineColor(Color::Red);
    playAgainText.setOrigin(playAgainText.getLocalBounds().width / 2, playAgainText.getLocalBounds().height / 2);
    playAgainText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 100));


    Text backToMenuText;
    backToMenuText.setFont(font);
    backToMenuText.setString("Back to Menu");
    backToMenuText.setCharacterSize(70);
    backToMenuText.setFillColor(Color::White);
    backToMenuText.setOutlineThickness(1.0f);
    backToMenuText.setOutlineColor(Color::Red);
    backToMenuText.setOrigin(backToMenuText.getLocalBounds().width / 2, backToMenuText.getLocalBounds().height / 2);
    backToMenuText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 150));


    // ------------------------------------------------------- SPRITE OBJECTS -------------------------------------------------------
    Texture playerTexture;
    if (!playerTexture.loadFromFile("Assets/player.png")) {
        cerr << "Error loading player texture" << endl;
    }

    Sprite player;
    player.setTexture(playerTexture);
    player.setScale(0.5f, 0.5f);
    player.setOrigin(Vector2f(player.getLocalBounds().width / 2, player.getLocalBounds().height / 2));
    player.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

    playerRadius = player.getLocalBounds().width * player.getScale().x / 2;

    Texture asteroidTexture;
    if (!asteroidTexture.loadFromFile("Assets/asteroid.png")) {
        cerr << "Error loading asteroid texture" << endl;
    }

    Sprite asteroid;
    asteroid.setTexture(asteroidTexture);
    asteroid.setOrigin(Vector2f(asteroid.getLocalBounds().width / 2, asteroid.getLocalBounds().height / 2));

    // Background
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Background/" + to_string(frame) + ".jpg")) {
        cerr << "Error loading background texture" << endl;
    }

    Sprite background;
    background.setTexture(backgroundTexture);
    background.setScale(Vector2f(windowWidth / background.getGlobalBounds().width, windowHeight / background.getGlobalBounds().height));

    // ------------------------------------------------------- Sounds -------------------------------------------------------
    // Game over sound
    SoundBuffer gameOverBuffer;
    if (!gameOverBuffer.loadFromFile("Assets/gameover.wav")) {
        cerr << "Error loading game over sound" << endl;
    }
    Sound gameOverSound;
    gameOverSound.setBuffer(gameOverBuffer);

    // Shooting sound
    SoundBuffer shootingBuffer;
    if (!shootingBuffer.loadFromFile("Assets/shooting.wav")) {
        cerr << "Error loading shooting sound" << endl;
    }
    Sound shootingSound;
    shootingSound.setBuffer(shootingBuffer);

    // Asteroid - asteroid collision sound
    SoundBuffer asteroidCollisionBuffer;
    if (!asteroidCollisionBuffer.loadFromFile("Assets/asteroidcollision.wav")) {
        cerr << "Error loading asteroid collision sound" << endl;
    }
    Sound asteroidCollisionSound;
    asteroidCollisionSound.setBuffer(asteroidCollisionBuffer);
    asteroidCollisionSound.setVolume(10.0f);

    // Asteroid - bullet collision sound
    SoundBuffer bulletCollisionBuffer;
    if (!bulletCollisionBuffer.loadFromFile("Assets/bulletcollision.wav")) {
        cerr << "Error loading bullet collision sound" << endl;
    }
    Sound bulletCollisionSound;
    bulletCollisionSound.setBuffer(bulletCollisionBuffer);
    bulletCollisionSound.setVolume(10.0f);

    // Player damage sound
    SoundBuffer playerDamageBuffer;
    if (!playerDamageBuffer.loadFromFile("Assets/playerdamage.wav")) {
        cerr << "Error loading player damage sound" << endl;
    }
    Sound playerDamageSound;
    playerDamageSound.setBuffer(playerDamageBuffer);


    // ------------------------------------------------------- GAME LOOP -------------------------------------------------------
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }

        deltaTime = movementClock.restart().asSeconds();

        Vector2i mousePosInt = Mouse::getPosition(window);
        Vector2f mousePos(static_cast<float>(mousePosInt.x), static_cast<float>(mousePosInt.y));
        Vector2f playerPos = player.getPosition();

        // Start screen
        if (startScreen) {

            highScoreText.setOrigin(highScoreText.getLocalBounds().width / 2, highScoreText.getLocalBounds().height / 2);
            highScoreText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 - 75));
            window.clear(Color::Black);
            window.draw(background);
            window.draw(highScoreText);
            window.draw(startText);
            window.draw(tutorialText);
            window.draw(quitText);
            window.draw(creditsText);
            window.display();

            // Start game
            if (Mouse::isButtonPressed(Mouse::Left) && startText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                currentScore = 0;
                bulletRemaining = bulletCount;
                lives = 3;
                frame = 1;
                livesText.setString("Lives: " + to_string(lives));
                player.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));
                player.setColor(Color::White);
                bullets.clear();
                asteroids.clear();

                startScreen = false;
                isGamePaused = false;
                isGameRunning = true;
                isGameOver = false;
            }

            // Quit game
            else if (Mouse::isButtonPressed(Mouse::Left) && quitText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.close();
                return 0;
            }

            // Tutorial
            else if (Mouse::isButtonPressed(Mouse::Left) && tutorialText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                startScreen = false;
                tutorialScreen = true;
            }
        }

        // Tutorial screen
        else if (tutorialScreen) {
            window.clear(Color::Black);
            window.draw(background);
            window.draw(tutorialContentText);
            window.draw(returnText);
            window.display();
            // Return to start screen
            if (Mouse::isButtonPressed(Mouse::Left) && returnText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                tutorialScreen = false;
                startScreen = true;
            }
        }
        // Playing game
        else if (isGameRunning) {

            if (scoreClock.getElapsedTime().asSeconds() >= 1.0f && isGameRunning) { // Increments 1 point every second
                currentScore += 1;
                scoreClock.restart();
            }

            // Player movement & rotation
            if (Keyboard::isKeyPressed(Keyboard::A)) angle -= rotationSpeed * deltaTime;
            if (Keyboard::isKeyPressed(Keyboard::D)) angle += rotationSpeed * deltaTime;
            player.setRotation(angle);

            radians = (angle - 90.0f) * 3.14159f / 180.0f;
            Vector2f direction(cos(radians), sin(radians));
            player.move(direction * playerSpeed * deltaTime);

            // Bounds checking
            if (playerPos.x < 0) player.setPosition(Vector2f(windowWidth, playerPos.y)); // Left
            if (playerPos.x > windowWidth) player.setPosition(Vector2f(0, playerPos.y)); // Right
            if (playerPos.y < 0) player.setPosition(Vector2f(playerPos.x, windowHeight)); // Top
            if (playerPos.y > windowHeight) player.setPosition(Vector2f(playerPos.x, 0)); // Bottom

            // Spawn bullets
            if (bulletRemaining > 0 && Keyboard::isKeyPressed(Keyboard::Space) && bulletCooldownClock.getElapsedTime().asSeconds() > bulletCooldownTime) {
                CircleShape bullet;
                bullet.setRadius(5.0f);
                bullet.setOutlineThickness(0.5f);
                bullet.setOutlineColor(Color::Red);
                bullet.setOrigin(Vector2f(bullet.getLocalBounds().width / 2, bullet.getLocalBounds().height / 2));
                bullet.setFillColor(Color(rand() % 256, rand() % 256, rand() % 256));
                bullet.setPosition(Vector2f(playerPos.x, playerPos.y));
                Vector2f bulletDirection(cos(radians), sin(radians));
                bullets.push_back(make_pair(bullet, bulletDirection));
                --bulletRemaining;
                shootingSound.play();
                bulletCooldownClock.restart();
            }
            // Bullet movement
            for (auto& bullet : bullets) {
                bullet.first.move(bullet.second * bulletSpeed * deltaTime);
            }
            // Out of bounds bullet deletion
            bullets.erase(remove_if(bullets.begin(), bullets.end(), [&](const pair<CircleShape, Vector2f>& bullet) {
                Vector2f bulletPos = bullet.first.getPosition();
                return bulletPos.x < 0 || bulletPos.x > windowWidth || bulletPos.y < 0 || bulletPos.y > windowHeight;
                }), bullets.end());

            // Spawn asteroids
            if (asteroidSpawnClock.getElapsedTime().asSeconds() > asteroidSpawnTime) {
                int edge = rand() % 4;
                if (edge == 0) asteroid.setPosition(Vector2f(static_cast<float>(rand() % static_cast<int>(windowWidth)), -50.0f)); // Top
                else if (edge == 1) asteroid.setPosition(Vector2f(static_cast<float>(rand() % static_cast<int>(windowWidth)), windowHeight + 50)); // Bottom
                else if (edge == 2) asteroid.setPosition(Vector2f(-50.0f, static_cast<float>(rand() % static_cast<int>(windowHeight)))); // Left
                else if (edge == 3) asteroid.setPosition(Vector2f(windowWidth + 50.0f, static_cast<float>(rand() % static_cast<int>(windowHeight)))); // Right

                asteroidScale = static_cast<float>((rand() % 4 + 2)) / 10.0f;
                asteroid.setScale(asteroidScale, asteroidScale);

                asteroidRotationSpeed = static_cast<float>((rand() % 181) - 90);

                asteroids.push_back(make_pair(asteroid, asteroidRotationSpeed));
                asteroidSpawnClock.restart();
            }

            // Asteroid movement
            for (auto& asteroid : asteroids) {
                Vector2f direction = playerPos - asteroid.first.getPosition();
                float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
                direction = Vector2f(direction.x / distance, direction.y / distance);
                asteroid.first.move(direction * asteroidSpeed * deltaTime);
                asteroid.first.setRotation(asteroid.first.getRotation() + asteroid.second * deltaTime);
            }

            // ------------------------------------------------------- Collision Detection -------------------------------------------------------

            // Asteroid - Asteroid Collision
            for (size_t i = 0; i < asteroids.size();) {
                bool asteroidRemoved = false;
                for (size_t j = i + 1; j < asteroids.size();) {
                    Vector2f asteroidPos1 = asteroids[i].first.getPosition();
                    Vector2f asteroidPos2 = asteroids[j].first.getPosition();
                    float distance = sqrt(static_cast<float>(pow(asteroidPos1.x - asteroidPos2.x, 2)) + static_cast<float>(pow(asteroidPos1.y - asteroidPos2.y, 2)));

                    float combinedRadii = (asteroids[i].first.getLocalBounds().width * asteroids[i].first.getScale().x / 2) + (asteroids[j].first.getLocalBounds().width * asteroids[j].first.getScale().x / 2);
                    if (distance <= combinedRadii) {
                        asteroids.erase(asteroids.begin() + j);
                        asteroids.erase(asteroids.begin() + i);
                        asteroidRemoved = true;
                        currentScore += 10;
                        asteroidCollisionSound.play();
                        break;
                    }
                    else {
                        ++j;
                    }
                }
                if (!asteroidRemoved) ++i;
            }

            // Asteroid - Bullet Collision
            for (size_t i = 0; i < asteroids.size();) {
                bool asteroidRemoved = false;
                for (size_t j = 0; j < bullets.size();) {
                    Vector2f asteroidPos = asteroids[i].first.getPosition();
                    Vector2f bulletPos = bullets[j].first.getPosition();
                    float distance = sqrt(static_cast<float>(pow(bulletPos.x - asteroidPos.x, 2)) + static_cast<float>(pow(bulletPos.y - asteroidPos.y, 2)));
                    float combinedRadii = (asteroids[i].first.getLocalBounds().width * asteroids[i].first.getScale().x / 2) + bullets[j].first.getRadius();
                    if (distance <= combinedRadii) {
                        asteroids.erase(asteroids.begin() + i);
                        bullets.erase(bullets.begin() + j);
                        currentScoreText.setString("Score: " + to_string(currentScore)); // Update score text
                        asteroidRemoved = true;
                        currentScore += 15;
                        bulletCollisionSound.play();
                        break;
                    }
                    else {
                        ++j;
                    }
                }
                if (!asteroidRemoved) ++i;
            }

            // Player - Asteroid Collision
            for (size_t i = 0; i < asteroids.size();) {
                Vector2f asteroidPos = asteroids[i].first.getPosition();
                float distance = sqrt(static_cast<float>(pow(playerPos.x - asteroidPos.x, 2)) + static_cast<float>(pow(playerPos.y - asteroidPos.y, 2)));
                float combinedRadii = playerRadius + (asteroids[i].first.getLocalBounds().width * asteroids[i].first.getScale().x / 2);
                if (distance <= combinedRadii) {
                    asteroids.erase(asteroids.begin() + i);
                    --lives;
                    if (lives > 0) playerDamageSound.play();

                    livesText.setString("Lives: " + to_string(lives));
                    if (lives == 0) {
                        gameOverSound.play();
                        player.setColor(Color::Red);
                        isGameRunning = false;
                        isGameOver = true;

                        fstream highScoreFile("highscore.txt", ios::in | ios::out);
                        if (currentScore >= highScore) {
                            highScore = currentScore;
                            highScoreFile << highScore;
                        }
                    }
                }
                else {
                    ++i;
                }
            }

            // Update high score if current score exceeds it
            if (currentScore > highScore) highScore = currentScore;

            currentScoreText.setString("Score: " + to_string(currentScore));
            bulletCountText.setString("Bullets: " + to_string(bulletRemaining));
            highScoreText.setString("High Score: " + to_string(highScore));
            highScoreText.setOrigin(highScoreText.getLocalBounds().width, highScoreText.getLocalBounds().height);
            highScoreText.setPosition(Vector2f(windowWidth - 10, windowHeight - 25));

            // Press Esc to pause
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                pausedFrame = frame;
                isGamePaused = true;
                isGameRunning = false;
            }

            // Display contents
            window.clear(Color::Black);
            window.draw(background);

            window.draw(player);
            for (auto& bullet : bullets) window.draw(bullet.first);
            for (auto& asteroid : asteroids) window.draw(asteroid.first);

            window.draw(currentScoreText);
            window.draw(bulletCountText);
            window.draw(livesText);
            window.draw(highScoreText);

            window.display();
        }

        // Pause screen
        else if (isGamePaused) {
            backgroundTexture.loadFromFile("Background/" + to_string(pausedFrame) + ".jpg");
            background.setTexture(backgroundTexture);

            window.clear(Color::Black);
            window.draw(background);
            window.draw(player);
            window.draw(currentScoreText);
            window.draw(bulletCountText);
            window.draw(livesText);
            window.draw(highScoreText);
            for (auto& bullet : bullets) window.draw(bullet.first);
            for (auto& asteroid : asteroids) window.draw(asteroid.first);
            window.draw(resumeText);
            window.draw(quitToMenuText);


            window.display();

            // Resume game
            if (Mouse::isButtonPressed(Mouse::Left) && resumeText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
				frame = pausedFrame;
                isGameRunning = true;
                isGamePaused = false;
            }
            // Quit to menu
            else if (Mouse::isButtonPressed(Mouse::Left) && quitToMenuText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isGameRunning = false;
                isGamePaused = false;
                startScreen = true;
            }
        }

        // Game over screen
        else if (isGameOver) {

            yourScoreText.setString("Your Score: " + to_string(currentScore));
            yourScoreText.setOrigin(yourScoreText.getLocalBounds().width / 2, yourScoreText.getLocalBounds().height / 2);
            yourScoreText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));

            highScoreText.setCharacterSize(50);
            highScoreText.setOrigin(highScoreText.getLocalBounds().width / 2, highScoreText.getLocalBounds().height / 2);
            highScoreText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 60));

            playAgainText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 120));

            backToMenuText.setPosition(Vector2f(windowWidth / 2, windowHeight / 2 + 200));

            angle = 0.0f;

            // Erase all bullets and asteroids
            bullets.clear();
            asteroids.clear();

            window.clear(Color::Black);

            window.draw(player);
            window.draw(gameOverText);
            window.draw(yourScoreText);
            window.draw(highScoreText);
            window.draw(playAgainText);
            window.draw(backToMenuText);

            window.display();
            // Back to menu
            if (Mouse::isButtonPressed(Mouse::Left) && backToMenuText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isGameOver = false;
                startScreen = true;
            }
            // Play again
            else if (Mouse::isButtonPressed(Mouse::Left) && playAgainText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                currentScore = 0;
                bulletRemaining = bulletCount;
                lives = 3;
                frame = 1;
                livesText.setString("Lives: " + to_string(lives));
                player.setPosition(Vector2f(windowWidth / 2, windowHeight / 2));
                player.setColor(Color::White);
                bullets.clear();
                asteroids.clear();

                startScreen = false;
                isGamePaused = false;
                isGameRunning = true;
                isGameOver = false;
            }

        }
        frame++;
        if (frame == 904) frame = 1;
        backgroundTexture.loadFromFile("Background/" + to_string(frame) + ".jpg");
        background.setTexture(backgroundTexture);
    }
    return 0;
}
