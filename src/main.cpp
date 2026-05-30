#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <sstream>

// Windows-specific includes for console control
#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN system("cls")
#else
    #include <unistd.h>
    #define CLEAR_SCREEN system("clear")
#endif

// ============================================================================
// CONSTANTS
// ============================================================================

const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
const float PHYSICS_SCALE = 0.5f;  // 1 unit = 2 pixels horizontally
const int LAUNCHER_X = 5;
const int LAUNCHER_Y = SCREEN_HEIGHT - 4;

// ============================================================================
// PHYSICS ENGINE
// ============================================================================

class PhysicsEngine {
public:
    // Calcula la posición X del proyectil
    static float calcX(float v0, float angle, float t) {
        float rad = angle * M_PI / 180.0f;
        return v0 * cos(rad) * t;
    }

    // Calcula la posición Y del proyectil
    static float calcY(float v0, float angle, float h0, float gravity, float t) {
        float rad = angle * M_PI / 180.0f;
        float vy = v0 * sin(rad);
        return h0 + vy * t - 0.5f * gravity * t * t;
    }

    // Calcula el tiempo de vuelo total
    static float calcFlightTime(float v0, float angle, float h0, float gravity) {
        float rad = angle * M_PI / 180.0f;
        float vy = v0 * sin(rad);
        float a = -0.5f * gravity;
        float b = vy;
        float c = h0;

        // Resuelve: 0 = a*t^2 + b*t + c
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) return 0;

        float t1 = (-b + sqrt(discriminant)) / (2 * a);
        float t2 = (-b - sqrt(discriminant)) / (2 * a);

        return (t1 > 0) ? t1 : t2;
    }

    // Calcula la altura máxima
    static float calcMaxHeight(float v0, float angle, float h0, float gravity) {
        float rad = angle * M_PI / 180.0f;
        float vy = v0 * sin(rad);
        return h0 + (vy * vy) / (2 * gravity);
    }

    // Calcula el alcance horizontal
    static float calcRange(float v0, float angle, float h0, float gravity) {
        float flightTime = calcFlightTime(v0, angle, h0, gravity);
        return calcX(v0, angle, flightTime);
    }
};

// ============================================================================
// GAME STRUCTURES
// ============================================================================

struct Projectile {
    float x, y;
    float vx, vy;
    float h0;
    float gravity;
    float t;
    bool active;

    Projectile(float v0, float angle, float h0, float gravity)
        : x(LAUNCHER_X), y(LAUNCHER_Y), h0(h0), gravity(gravity), t(0), active(true) {
        float rad = angle * M_PI / 180.0f;
        vx = v0 * cos(rad);
        vy = v0 * sin(rad);
    }

    void update(float dt) {
        t += dt;
        x = LAUNCHER_X + vx * t * PHYSICS_SCALE;
        y = LAUNCHER_Y - (h0 + vy * t - 0.5f * gravity * t * t) * PHYSICS_SCALE;
    }

    bool isValid() const {
        return y <= LAUNCHER_Y && x >= 0 && x < SCREEN_WIDTH;
    }
};

struct Target {
    int x, y;
    int radius;
    bool exists;

    Target() : radius(3), exists(false) {
        regenerate();
    }

    void regenerate() {
        x = 40 + rand() % 60;
        y = 8 + rand() % 10;
        exists = true;
    }

    bool checkCollision(float px, float py, int radius = 2) {
        int dx = static_cast<int>(px) - x;
        int dy = static_cast<int>(py) - y;
        int dist = static_cast<int>(sqrt(dx * dx + dy * dy));
        return dist < (this->radius + radius);
    }
};

struct GameStats {
    int score;
    int level;
    int hits;
    int totalShots;
    int maxDistance;

    GameStats() : score(0), level(1), hits(0), totalShots(0), maxDistance(0) {}

    void recordHit(float distance) {
        hits++;
        totalShots++;
        score += (100 + static_cast<int>(distance / 2));
        maxDistance = std::max(maxDistance, static_cast<int>(distance));
    }

    void recordMiss(float distance) {
        totalShots++;
        score = std::max(0, score - 10);
    }

    int getAccuracy() const {
        if (totalShots == 0) return 0;
        return (hits * 100) / totalShots;
    }
};

// ============================================================================
// RENDERING ENGINE
// ============================================================================

class RenderEngine {
private:
    char buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

public:
    RenderEngine() {
        clearBuffer();
    }

    void clearBuffer() {
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                buffer[y][x] = ' ';
            }
        }
    }

    void setPixel(int x, int y, char c) {
        if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
            buffer[y][x] = c;
        }
    }

    void drawBox(int x, int y, int w, int h, bool filled = false) {
        // Top and bottom
        for (int i = 0; i < w; i++) {
            setPixel(x + i, y, '=');
            setPixel(x + i, y + h - 1, '=');
        }
        // Left and right
        for (int i = 0; i < h; i++) {
            setPixel(x, y + i, '#');
            setPixel(x + w - 1, y + i, '#');
        }
        if (filled) {
            for (int i = 1; i < h - 1; i++) {
                for (int j = 1; j < w - 1; j++) {
                    setPixel(x + j, y + i, ' ');
                }
            }
        }
    }

    void drawString(int x, int y, const std::string& str) {
        for (size_t i = 0; i < str.length() && x + i < SCREEN_WIDTH; i++) {
            setPixel(x + i, y, str[i]);
        }
    }

    void drawCircle(int cx, int cy, int radius, char c = '*') {
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    setPixel(cx + x, cy + y, c);
                }
            }
        }
    }

    void drawFilledCircle(int cx, int cy, int radius, char c = '*') {
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y < radius * radius) {
                    setPixel(cx + x, cy + y, c);
                }
            }
        }
    }

    void drawLine(int x1, int y1, int x2, int y2, char c = '.') {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        int x = x1, y = y1;
        while (true) {
            setPixel(x, y, c);
            if (x == x2 && y == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    void render() {
        CLEAR_SCREEN;
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                std::cout << buffer[y][x];
            }
            std::cout << '\n';
        }
        std::cout.flush();
    }
};

// ============================================================================
// GAME MANAGER
// ============================================================================

class Game {
private:
    RenderEngine renderer;
    GameStats stats;
    Target target;
    Projectile* projectile;
    float v0, angle, gravity, h0;
    bool gameRunning;
    bool showTrajectory;

public:
    Game() : projectile(nullptr), v0(50), angle(45), gravity(9.8f), h0(0),
             gameRunning(true), showTrajectory(false) {
        srand(static_cast<unsigned>(time(0)));
    }

    ~Game() {
        if (projectile) delete projectile;
    }

    void drawTitle() {
        renderer.drawString(20, 1, "##############################################");
        renderer.drawString(20, 2, "#                                            #");
        renderer.drawString(20, 3, "#        >>> RETRO PARABOLIC SHOOTER <<<     #");
        renderer.drawString(20, 4, "#                   v1.985                    #");
        renderer.drawString(20, 5, "#                                            #");
        renderer.drawString(20, 6, "##############################################");
    }

    void drawLauncher() {
        renderer.setPixel(LAUNCHER_X, LAUNCHER_Y, 'A');
    }

    void drawProjectile() {
        if (projectile && projectile->active) {
            renderer.setPixel(static_cast<int>(projectile->x), 
                            static_cast<int>(projectile->y), '*');
        }
    }

    void drawTarget() {
        if (target.exists) {
            renderer.drawCircle(target.x, target.y, target.radius, 'X');
        }
    }

    void drawGround() {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            renderer.setPixel(x, SCREEN_HEIGHT - 1, '=');
            renderer.setPixel(x, LAUNCHER_Y + 1, '-');
        }
    }

    void drawStats() {
        int statsX = 5;
        int statsY = 8;

        renderer.drawBox(statsX, statsY, 30, 8, true);
        renderer.drawString(statsX + 2, statsY + 1, "PARAMETROS");
        
        std::stringstream ss;
        ss << "V: " << std::fixed << std::setprecision(1) << v0 << " m/s";
        renderer.drawString(statsX + 2, statsY + 2, ss.str());
        
        ss.str("");
        ss << "A: " << static_cast<int>(angle) << "  G: " << gravity;
        renderer.drawString(statsX + 2, statsY + 3, ss.str());
        
        ss.str("");
        ss << "H: " << std::fixed << std::setprecision(1) << h0 << " m";
        renderer.drawString(statsX + 2, statsY + 4, ss.str());

        // Calcular predicciones
        float range = PhysicsEngine::calcRange(v0, angle, h0, gravity);
        float maxH = PhysicsEngine::calcMaxHeight(v0, angle, h0, gravity);
        float flightTime = PhysicsEngine::calcFlightTime(v0, angle, h0, gravity);

        ss.str("");
        ss << "R: " << std::fixed << std::setprecision(1) << range << " m";
        renderer.drawString(statsX + 2, statsY + 5, ss.str());

        ss.str("");
        ss << "MaxH: " << std::fixed << std::setprecision(1) << maxH << " m";
        renderer.drawString(statsX + 2, statsY + 6, ss.str());
    }

    void drawGameStats() {
        int statsX = 40;
        int statsY = 8;

        renderer.drawBox(statsX, statsY, 35, 8, true);
        renderer.drawString(statsX + 2, statsY + 1, "ESTADISTICAS");
        
        std::stringstream ss;
        ss << "Puntuacion: " << stats.score;
        renderer.drawString(statsX + 2, statsY + 2, ss.str());
        
        ss.str("");
        ss << "Aciertos: " << stats.hits << "/" << stats.totalShots;
        renderer.drawString(statsX + 2, statsY + 3, ss.str());
        
        ss.str("");
        ss << "Precisión: " << stats.getAccuracy() << "%";
        renderer.drawString(statsX + 2, statsY + 4, ss.str());

        ss.str("");
        ss << "Nivel: " << stats.level;
        renderer.drawString(statsX + 2, statsY + 5, ss.str());

        ss.str("");
        ss << "Max Dist: " << stats.maxDistance << " m";
        renderer.drawString(statsX + 2, statsY + 6, ss.str());
    }

    void drawControls() {
        int ctrlY = 17;
        renderer.drawString(5, ctrlY, "CONTROLES: [W]Velocidad+  [S]Velocidad-  [A]Angulo-  [D]Angulo+");
        renderer.drawString(5, ctrlY + 1, "            [Q]Gravedad-  [E]Gravedad+  [Z]Altura-  [X]Altura+");
        renderer.drawString(5, ctrlY + 2, "            [SPACE]Disparar  [T]Trayectoria  [R]Reset  [ESC]Salir");
    }

    void drawTrajectory() {
        if (!showTrajectory) return;

        float range = PhysicsEngine::calcRange(v0, angle, h0, gravity);
        float flightTime = PhysicsEngine::calcFlightTime(v0, angle, h0, gravity);
        float maxH = PhysicsEngine::calcMaxHeight(v0, angle, h0, gravity);

        // Dibuja la trayectoria predicha
        int steps = std::min(30, static_cast<int>(range * PHYSICS_SCALE));
        for (int i = 0; i < steps; i++) {
            float t = (i / static_cast<float>(steps)) * flightTime;
            float x = LAUNCHER_X + PhysicsEngine::calcX(v0, angle, t) * PHYSICS_SCALE;
            float y = LAUNCHER_Y - (h0 + PhysicsEngine::calcY(v0, angle, 0, gravity, t) - h0) * PHYSICS_SCALE;
            renderer.setPixel(static_cast<int>(x), static_cast<int>(y), '.');
        }
    }

    void displayMenu() {
        CLEAR_SCREEN;
        std::cout << "\n\n";
        std::cout << "  ########################################## \n";
        std::cout << "  #   RETRO PARABOLIC SHOOTER v1.985   # \n";
        std::cout << "  ########################################## \n\n";
        std::cout << "  [1] JUGAR \n";
        std::cout << "  [2] INSTRUCCIONES \n";
        std::cout << "  [3] CREDITOS \n";
        std::cout << "  [0] SALIR \n\n";
        std::cout << "  Selecciona una opcion: ";
    }

    void displayInstructions() {
        CLEAR_SCREEN;
        std::cout << "\n ===== INSTRUCCIONES ===== \n\n";
        std::cout << "OBJETIVO: Acerta al objetivo 'X' modificando los parametros de disparo.\n\n";
        std::cout << "PARAMETROS:\n";
        std::cout << "  - Velocidad Inicial (V): Rapidez del proyectil\n";
        std::cout << "  - Angulo (A): Angulo de lanzamiento (0-90 grados)\n";
        std::cout << "  - Gravedad (G): Aceleracion hacia abajo\n";
        std::cout << "  - Altura Inicial (H): Altura de lanzamiento\n\n";
        std::cout << "CONTROLES:\n";
        std::cout << "  W/S - Aumentar/Disminuir velocidad\n";
        std::cout << "  A/D - Disminuir/Aumentar angulo\n";
        std::cout << "  Q/E - Disminuir/Aumentar gravedad\n";
        std::cout << "  Z/X - Disminuir/Aumentar altura\n";
        std::cout << "  SPACE - Disparar\n";
        std::cout << "  T - Ver trayectoria predicha\n";
        std::cout << "  R - Reiniciar juego\n";
        std::cout << "  ESC - Salir\n\n";
        std::cout << "FISICA:\n";
        std::cout << "  El juego utiliza ecuaciones reales de movimiento parabolico.\n";
        std::cout << "  Cada parametro afecta la trayectoria del proyectil.\n\n";
        std::cout << "Presiona ENTER para volver...";
        std::cin.get();
    }

    void displayCredits() {
        CLEAR_SCREEN;
        std::cout << "\n\n";
        std::cout << "  =======================================\n";
        std::cout << "  RETRO PARABOLIC SHOOTER v1.985\n";
        std::cout << "  =======================================\n\n";
        std::cout << "  Desarrollo: Senior C++ Developer\n";
        std::cout << "  Especializacion: Fisica de Videojuegos\n";
        std::cout << "  Motor Grafico: ASCII Art Terminal\n";
        std::cout << "  Año: 1985-2026\n\n";
        std::cout << "  Tecnologias:\n";
        std::cout << "  - C++17 Standard\n";
        std::cout << "  - Simulacion Fisica Realista\n";
        std::cout << "  - Rendering ASCII Console\n\n";
        std::cout << "  Gracias por jugar!\n\n";
        std::cout << "Presiona ENTER para volver...";
        std::cin.get();
    }

    void processInput(char key) {
        switch (key) {
            case 'w':
            case 'W':
                v0 = std::min(100.0f, v0 + 1);
                break;
            case 's':
            case 'S':
                v0 = std::max(5.0f, v0 - 1);
                break;
            case 'a':
            case 'A':
                angle = std::max(0.0f, angle - 1);
                break;
            case 'd':
            case 'D':
                angle = std::min(90.0f, angle + 1);
                break;
            case 'q':
            case 'Q':
                gravity = std::max(1.0f, gravity - 0.1f);
                break;
            case 'e':
            case 'E':
                gravity = std::min(20.0f, gravity + 0.1f);
                break;
            case 'z':
            case 'Z':
                h0 = std::max(0.0f, h0 - 1);
                break;
            case 'x':
            case 'X':
                h0 = std::min(50.0f, h0 + 1);
                break;
            case ' ':
                if (!projectile) {
                    projectile = new Projectile(v0, angle, h0, gravity);
                }
                break;
            case 't':
            case 'T':
                showTrajectory = !showTrajectory;
                break;
            case 'r':
            case 'R':
                reset();
                break;
        }
    }

    void update() {
        if (projectile && projectile->active) {
            projectile->update(0.05f);

            // Verificar colisión
            if (target.checkCollision(projectile->x, projectile->y)) {
                float distance = projectile->x / PHYSICS_SCALE;
                stats.recordHit(distance);
                stats.level = (stats.hits / 3) + 1;
                delete projectile;
                projectile = nullptr;
                target.regenerate();
            }
            // Verificar si sale del mapa
            else if (!projectile->isValid()) {
                float distance = projectile->x / PHYSICS_SCALE;
                stats.recordMiss(distance);
                delete projectile;
                projectile = nullptr;
            }
        }
    }

    void render() {
        renderer.clearBuffer();
        drawGround();
        drawTitle();
        drawLauncher();
        drawTarget();
        drawProjectile();
        drawTrajectory();
        drawStats();
        drawGameStats();
        drawControls();
        renderer.render();
    }

    void reset() {
        if (projectile) {
            delete projectile;
            projectile = nullptr;
        }
        v0 = 50;
        angle = 45;
        gravity = 9.8f;
        h0 = 0;
        showTrajectory = false;
        stats = GameStats();
        target.regenerate();
    }

    void run() {
        char choice;
        do {
            displayMenu();
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case '1':
                    gameLoop();
                    break;
                case '2':
                    displayInstructions();
                    break;
                case '3':
                    displayCredits();
                    break;
                case '0':
                    gameRunning = false;
                    break;
            }
        } while (gameRunning && choice != '0');
    }

    void gameLoop() {
        reset();
        gameRunning = true;
        auto lastTime = std::chrono::high_resolution_clock::now();

        while (gameRunning) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);

            if (elapsed.count() >= 50) {
                update();
                render();
                lastTime = currentTime;
            }

            // Simulación de lectura de entrada sin bloquear
            // En producción usar bibliotecas como ncurses para mejor control
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

// ============================================================================
// MAIN
// ============================================================================

int main() {
    Game game;
    game.run();
    return 0;
}
