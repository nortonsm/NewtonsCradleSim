#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_BALLS = 5;
const float GRAVITY = 980.0f; // Adjusted gravity
const float PI = 3.14159265359f;

// Global variables
std::mutex physicsMutex;
std::atomic<bool> running(true);
int draggedBallIndex = -1; // Index of the ball being dragged (-1 if none)

// Ball structure
struct Ball {
    float mass;
    float length; // Length of the string in pixels
    float angle;  // Current angle from the vertical (in radians)
    float angularVelocity;
    float angularAcceleration;

    float x; // Current x position
    float y; // Current y position

    sf::CircleShape shape;
    sf::Vertex line[2]; // Line representing the string
};

// Function to initialize the balls
void initializeBalls(std::vector<Ball>& balls, float startX, float startY, float ballRadius, float spacing) {
    for (int i = 0; i < NUM_BALLS; ++i) {
        Ball ball;
        ball.mass = 2.0f; // Default mass
        ball.length = 200.0f; // String length
        ball.angle = 0.0f;
        ball.angularVelocity = 0.0f;
        ball.angularAcceleration = 0.0f;

        ball.shape.setRadius(ballRadius);
        ball.shape.setOrigin(ballRadius, ballRadius);
        ball.shape.setFillColor(sf::Color::Blue);

        // Position the balls horizontally with slight spacing
        float x = startX + i * (ballRadius * 2 + spacing);
        float y = startY + ball.length * std::cos(ball.angle);

        ball.x = x;
        ball.y = y;

        ball.shape.setPosition(ball.x, ball.y);

        // Initialize the string (line)
        ball.line[0] = sf::Vertex(sf::Vector2f(x, startY), sf::Color::Black);
        ball.line[1] = sf::Vertex(ball.shape.getPosition(), sf::Color::Black);

        balls.push_back(ball);
    }

    // Pull the first ball back to start the simulation
    balls[0].angle = -PI / 4.0f; // Negative 45 degrees
    balls[0].x = balls[0].line[0].position.x + balls[0].length * std::sin(balls[0].angle);
    balls[0].y = balls[0].line[0].position.y + balls[0].length * std::cos(balls[0].angle);

    balls[0].shape.setPosition(balls[0].x, balls[0].y);
    balls[0].line[1].position = balls[0].shape.getPosition();
    balls[0].angularVelocity = 0.0f;
}

// Physics calculations
void physicsThread(std::vector<Ball>& balls) {
    sf::Clock clock;
    while (running) {
        float deltaTime = clock.restart().asSeconds();

        // Adjust timeScale to control simulation speed
        float timeScale = 2.0f; // Lower value slows down the simulation
        deltaTime *= timeScale;

        {
            std::lock_guard<std::mutex> lock(physicsMutex);

            // Update each ball's physics
            for (size_t i = 0; i < balls.size(); ++i) {
                if (draggedBallIndex == static_cast<int>(i)) {
                    continue; // Skip physics update for the dragged ball
                }
                Ball& ball = balls[i];

                // Simple pendulum motion equations
                ball.angularAcceleration = (-GRAVITY / ball.length) * std::sin(ball.angle);
                ball.angularVelocity += ball.angularAcceleration * deltaTime;
                ball.angle += ball.angularVelocity * deltaTime;

                // Damping to simulate energy loss (optional)
                ball.angularVelocity *= 0.9999f;

                // Update ball position
                ball.x = ball.line[0].position.x + ball.length * std::sin(ball.angle);
                ball.y = ball.line[0].position.y + ball.length * std::cos(ball.angle);
                ball.shape.setPosition(ball.x, ball.y);
                ball.line[1].position = ball.shape.getPosition();
            }

            // Collision detection and response
            for (int i = 0; i < NUM_BALLS - 1; ++i) {
                float dx = balls[i + 1].x - balls[i].x;
                float dy = balls[i + 1].y - balls[i].y;
                float distance = std::sqrt(dx * dx + dy * dy);
                float minDistance = balls[i].shape.getRadius() * 2.0f - 0.1f; // Slightly less to prevent sticking

                // If balls are touching or overlapping
                if (distance < minDistance) {
                    // Correct positions to prevent overlap
                    float overlap = 0.5f * (minDistance - distance + 0.01f);
                    float nx = dx / distance;
                    float ny = dy / distance;

                    balls[i].x -= overlap * nx;
                    balls[i].y -= overlap * ny;
                    balls[i + 1].x += overlap * nx;
                    balls[i + 1].y += overlap * ny;

                    // Update angles based on new positions
                    balls[i].angle = std::atan2(balls[i].x - balls[i].line[0].position.x, balls[i].y - balls[i].line[0].position.y);
                    balls[i + 1].angle = std::atan2(balls[i + 1].x - balls[i + 1].line[0].position.x, balls[i + 1].y - balls[i + 1].line[0].position.y);

                    // Compute velocities
                    float v1 = balls[i].angularVelocity;
                    float v2 = balls[i + 1].angularVelocity;

                    // Simple elastic collision equations
                    float m1 = balls[i].mass;
                    float m2 = balls[i + 1].mass;

                    float newV1 = ((m1 - m2) / (m1 + m2)) * v1 + ((2.0f * m2) / (m1 + m2)) * v2;
                    float newV2 = ((2.0f * m1) / (m1 + m2)) * v1 + ((m2 - m1) / (m1 + m2)) * v2;

                    balls[i].angularVelocity = newV1;
                    balls[i + 1].angularVelocity = newV2;
                }
            }

            // Update positions after collision response
            for (auto& ball : balls) {
                ball.shape.setPosition(ball.x, ball.y);
                ball.line[1].position = ball.shape.getPosition();
            }
        }

        // Sleep briefly to prevent high CPU usage
        sf::sleep(sf::milliseconds(1));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Newton's Cradle Simulation");
    window.setFramerateLimit(60);

    // Adjust initial settings here
    float ballRadius = 20.0f;
    float spacing = 0.5f; // Slight spacing to prevent visual overlap
    float totalWidth = NUM_BALLS * (ballRadius * 2 + spacing);
    float startX = (WINDOW_WIDTH - totalWidth) / 2.0f + ballRadius;
    float startY = 100.0f;

    // Initialize balls
    std::vector<Ball> balls;
    initializeBalls(balls, startX, startY, ballRadius, spacing);

    // Start physics thread
    std::thread physics(physicsThread, std::ref(balls));

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    std::lock_guard<std::mutex> lock(physicsMutex);
                    for (size_t i = 0; i < balls.size(); ++i) {
                        sf::Vector2f ballPos = balls[i].shape.getPosition();
                        float dx = mousePos.x - ballPos.x;
                        float dy = mousePos.y - ballPos.y;
                        float distance = std::sqrt(dx * dx + dy * dy);
                        if (distance <= balls[i].shape.getRadius()) {
                            draggedBallIndex = static_cast<int>(i);
                            break;
                        }
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    std::lock_guard<std::mutex> lock(physicsMutex);
                    draggedBallIndex = -1;
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                std::lock_guard<std::mutex> lock(physicsMutex);
                if (draggedBallIndex != -1) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    Ball& ball = balls[draggedBallIndex];
                    ball.x = mousePos.x;
                    ball.y = mousePos.y;
                    ball.shape.setPosition(ball.x, ball.y);
                    ball.line[1].position = ball.shape.getPosition();

                    // Update angle based on new position
                    ball.angle = std::atan2(ball.x - ball.line[0].position.x, ball.y - ball.line[0].position.y);
                    ball.angularVelocity = 0.0f; // Reset angular velocity while dragging
                }
            }
        }

        // Rendering
        window.clear(sf::Color::White);

        // Draw strings and balls
        {
            std::lock_guard<std::mutex> lock(physicsMutex);
            for (auto& ball : balls) {
                window.draw(ball.line, 2, sf::Lines);
                window.draw(ball.shape);
            }
        }

        window.display();
    }

    // Wait for physics thread to finish
    if (physics.joinable()) {
        physics.join();
    }

    return 0;
}
