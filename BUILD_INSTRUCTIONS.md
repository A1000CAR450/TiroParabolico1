# Instrucciones de Compilación - Retro Parabolic Shooter

## Requisitos

- **Compilador C++17 o superior**
  - GCC 7.0+
  - Clang 5.0+
  - MSVC 2017+
- **CMake 3.10+** (opcional pero recomendado)
- **Consola/Terminal** con soporte ANSI

## Compilación con CMake (Recomendado)

### En Linux/macOS:

```bash
# Crear directorio de compilación
mkdir build
cd build

# Configurar CMake
cmake ..

# Compilar
make

# Ejecutar
./tiro_parabolico
```

### En Windows (Visual Studio):

```bash
# Crear directorio de compilación
mkdir build
cd build

# Configurar CMake
cmake .. -G "Visual Studio 16 2019"

# Compilar
cmake --build . --config Release

# Ejecutar
Release\tiro_parabolico.exe
```

## Compilación Manual

### En Linux/macOS:

```bash
g++ -std=c++17 -O2 -o tiro_parabolico src/main.cpp -lm
./tiro_parabolico
```

### En Windows (MinGW):

```bash
g++ -std=c++17 -O2 -o tiro_parabolico.exe src/main.cpp
tiro_parabolico.exe
```

## Notas de Compatibilidad

### Windows
- Utiliza `system("cls")` para limpiar pantalla
- Compatible con Command Prompt y PowerShell
- Se recomienda ejecutar en terminal con soporte UTF-8

### Linux/macOS
- Utiliza `system("clear")` para limpiar pantalla
- Completamente compatible con bash, zsh, etc.
- Mejor experiencia en terminales modernas (xterm, gnome-terminal, iterm2)

## Troubleshooting

### La pantalla se ve desordenada
- Asegúrate que tu terminal tenga al menos 120 caracteres de ancho x 30 líneas
- Aumenta el tamaño de la fuente si es necesario
- Verifica que la codificación sea UTF-8

### El juego corre lento
- Cierra otras aplicaciones
- En WSL (Windows Subsystem for Linux), ejecuta directamente en Windows si es posible
- Verifica que la compilación sea con optimizaciones (-O2 o -O3)

### Problemas de compilación
- Asegúrate de usar C++17 o superior: `g++ --version`
- En Windows, utiliza Visual Studio 2017 o superior
- En macOS, actualiza Xcode: `xcode-select --install`

## Parámetros de Compilación Avanzados

### Compilación con optimizaciones agresivas:
```bash
g++ -std=c++17 -O3 -march=native -flto -o tiro_parabolico src/main.cpp -lm
```

### Compilación con símbolos de depuración:
```bash
g++ -std=c++17 -g -O0 -o tiro_parabolico src/main.cpp -lm
```

## Requisitos de Pantalla

- **Ancho mínimo**: 120 caracteres
- **Alto mínimo**: 30 líneas
- **Fuente monoespaciada** (recomendada para mejor visualización)
