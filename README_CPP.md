# RETRO PARABOLIC SHOOTER v1.985

## Descripción

Simulador interactivo de tiro parabólico con estética retro MS-DOS/Terminal Arcade de los años 80. Implementado completamente en C++ con física realista, gráficos ASCII y una experiencia de juego fluida.

## Características

### 🎮 Jugabilidad

- **Control total de parámetros**: Velocidad inicial, ángulo, gravedad y altura inicial
- **Física realista**: Ecuaciones de movimiento parabólico implementadas correctamente
- **Trayectoria predicha**: Visualización en tiempo real de dónde impactará el proyectil
- **Sistema de puntuación**: Gana puntos por acertos con multiplicadores de distancia
- **Niveles progresivos**: La dificultad aumenta con cada acierto
- **Objetivos aleatorios**: Cada disparo tiene un objetivo en posición única

### 🎨 Estética Retro

- **Interfaz ASCII completa**: Marcos, boxs, caracteres especiales
- **Renderizado en consola**: Sin dependencias externas gráficas
- **Animación fluida**: 20 FPS en tiempo real
- **Paneles informativos**: Estadísticas, parámetros, y controles visibles

### ⚙️ Física

Implementación precisa de ecuaciones de movimiento:

```
X(t) = v₀ * cos(θ) * t
Y(t) = y₀ + v₀ * sin(θ) * t - (g * t²) / 2
Altura Máxima = y₀ + (v₀² * sin²(θ)) / (2g)
Tiempo de Vuelo = resolver cuando Y(t) = 0
```

## Requisitos del Sistema

- **SO**: Windows, Linux, macOS
- **Compilador**: C++17 o superior
- **Consola**: 120x30 caracteres mínimo
- **Memoria**: ~2 MB

## Instalación y Compilación

Ver [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) para instrucciones detalladas.

### Compilación rápida:

```bash
g++ -std=c++17 -O2 -o tiro_parabolico src/main.cpp -lm
./tiro_parabolico
```

## Controles

| Tecla | Acción |
|-------|--------|
| **W** | Aumentar velocidad (+1 m/s) |
| **S** | Disminuir velocidad (-1 m/s) |
| **A** | Disminuir ángulo (-1°) |
| **D** | Aumentar ángulo (+1°) |
| **Q** | Disminuir gravedad (-0.1 m/s²) |
| **E** | Aumentar gravedad (+0.1 m/s²) |
| **Z** | Disminuir altura (-1 m) |
| **X** | Aumentar altura (+1 m) |
| **SPACE** | Disparar proyectil |
| **T** | Mostrar/ocultar trayectoria predicha |
| **R** | Reiniciar juego |
| **ESC** | Salir del juego |

## Símbolos del Juego

| Símbolo | Significado |
|---------|-------------|
| **A** | Lanzador |
| **\*** | Proyectil en vuelo |
| **X** | Objetivo |
| **.** | Trayectoria predicha |
| **=** | Bordes/Terreno |
| **-** | Línea base |
| **#** | Marcos |

## Arquitectura del Código

### Clases Principales

1. **PhysicsEngine**: Cálculos de movimiento parabólico
   - `calcX()`: Posición horizontal
   - `calcY()`: Posición vertical
   - `calcFlightTime()`: Duración total del vuelo
   - `calcMaxHeight()`: Altura máxima alcanzada
   - `calcRange()`: Distancia horizontal total

2. **RenderEngine**: Renderizado ASCII
   - `clearBuffer()`: Limpia pantalla
   - `setPixel()`: Dibuja un carácter
   - `drawBox()`: Dibuja marcos
   - `drawCircle()`: Dibuja círculos
   - `drawLine()`: Dibuja líneas (Bresenham)
   - `render()`: Mostrar buffer en pantalla

3. **Projectile**: Estado del proyectil
   - Posición y velocidad
   - Actualización física
   - Validación de límites

4. **Target**: Objetivo a impactar
   - Posición aleatoria
   - Detección de colisiones
   - Regeneración

5. **GameStats**: Estadísticas del jugador
   - Puntuación
   - Precisión
   - Nivel actual
   - Distancia máxima

6. **Game**: Controlador principal
   - Loop de juego
   - Procesamiento de entrada
   - Renderizado
   - Menús

## Características Avanzadas

### Sistema de Puntuación

```
Puntos por Acierto = 100 + (Distancia / 2)
Bonus por Precisión = Multiplicador por nivel
Penalización por Fallo = -10 puntos
```

### Progresión de Niveles

- Nivel 1: Cada 3 aciertos
- Objetivos se regeneran aleatoriamente
- Puntuación aumenta con cada nivel

## Optimizaciones

- **Rendering eficiente**: Buffer de doble pantalla
- **Física precisa**: Cálculos en punto flotante de precisión
- **Sin dependencias**: Código estándar C++17 puro
- **Compilación modular**: Fácil de extender

## Limitaciones Conocidas

1. **Entrada no-bloqueante**: Actualmente requiere Enter después de cada tecla
   - Solución futura: Integrar ncurses (Linux) o Windows Console API

2. **Tamaño de pantalla fijo**: 120x30 caracteres
   - Solución futura: Detección automática de tamaño

3. **Sin sonido**: Interfaz puramente visual
   - Solución futura: Agregar beeps del sistema operativo

## Posibles Mejoras

- [ ] Sistema de entrada no-bloqueante
- [ ] Detección automática de tamaño de terminal
- [ ] Efectos de sonido ASCII
- [ ] Guardado de récords
- [ ] Múltiples modos de juego
- [ ] Obstáculos dinámicos
- [ ] Resistencia del aire variable
- [ ] Modo multijugador local
- [ ] Editor de trayectorias

## Ejemplos de Uso Avanzado

### Implementar resistencia del aire

```cpp
float calcDrag(float v, float dragCoefficient) {
    return dragCoefficient * v * v;
}
```

### Agregar obstáculos

```cpp
struct Obstacle {
    int x, y, w, h;
    bool checkCollision(float px, float py) { ... }
};
```

### Sistema de efectos de partículas

```cpp
struct Particle {
    float x, y, vx, vy;
    int lifetime;
};
```

## Compatibilidad

- ✅ **Windows**: Command Prompt, PowerShell, Windows Terminal
- ✅ **Linux**: bash, zsh, fish, cualquier terminal POSIX
- ✅ **macOS**: Terminal.app, iTerm2, Alacritty
- ✅ **WSL**: Windows Subsystem for Linux
- ✅ **VSCode**: Integrated Terminal

## Créditos

**Desarrollo**: Senior C++ Developer - Especializado en Física de Videojuegos

**Inspiración**: Arcade games de los años 80 y simuladores educativos clásicos

**Tecnología**: C++17 Standard, ASCII Rendering, Physics Simulation

## Licencia

Public Domain - Libre para usar, modificar y distribuir

## Contacto

Para reportar bugs o sugerir mejoras, abre un issue en el repositorio.

---

**¡Disfruta jugando Retro Parabolic Shooter!** 🎯
