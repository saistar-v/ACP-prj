# ACP-prj
# 2D Graphics Editor (Console)

A menu-driven console graphics editor written in **C** that draws shapes using `*` and `_` characters on a 2D character canvas.

## Features

- **Draw Shapes**: Circle, Rectangle, Line, and Triangle
- **Object Management**: Add and delete individual shapes by ID
- **Canvas Display**: Render all shapes on an 80×40 character canvas with a border
- **Object Listing**: View all shapes with their parameters
- **Clipping**: Shapes that extend beyond the canvas are safely clipped

## How It Works

The editor maintains:
1. A **2D character array** (`40 rows × 80 columns`) as the drawing canvas
2. An **object list** that stores shape parameters so shapes can be individually deleted

When a shape is added or removed, the entire canvas is redrawn from the object list, ensuring consistency.

### Drawing Characters
| Character | Usage |
|-----------|-------|
| `*` | Vertical edges, circle sides, diagonal lines, corners |
| `_` | Horizontal edges (top/bottom of rectangles, horizontal lines) |

### Drawing Algorithms
- **Circle**: Midpoint Circle Algorithm (Bresenham-style)
- **Line**: Bresenham's Line Algorithm
- **Rectangle**: Edge tracing with `_` for horizontal and `*` for vertical edges
- **Triangle**: Three lines connecting three vertices

## Build & Run

### Prerequisites
- A C compiler (`gcc`, `clang`, or MSVC)

### Compile

```bash
# GCC / MinGW
gcc -o graphics_editor graphics_editor.c -lm

# Clang
clang -o graphics_editor graphics_editor.c -lm

# MSVC (Developer Command Prompt)
cl graphics_editor.c
```

### Run

```bash
./graphics_editor
```

## Usage Example

```
  ============================================
       2D GRAPHICS EDITOR  (Console)
  ============================================
    1. Add Circle
    2. Add Rectangle
    3. Add Line
    4. Add Triangle
    5. Delete Object
    6. List Objects
    7. Display Canvas
    8. Clear Canvas
    0. Exit
  --------------------------------------------
  Choice: 1

  --- Add Circle ---
  Canvas size: 80 cols x 40 rows (col: 0-79, row: 0-39)
  Center column (x): 40
  Center row    (y): 20
  Radius           : 10
  [OK] Object added with ID 1.
```

## Sample Output

```
+--------------------------------------------------------------------------------+
|                              ___________                                       |
|                          ****           ****                                   |
|                        **                   **                                 |
|                      **                       **                               |
|                     *                           *                              |
|                    *                             *                             |
|                    *                             *                             |
|                    *                             *                             |
|                     *                           *                              |
|                      **                       **                               |
|                        **                   **                                 |
|                          ****           ****                                   |
|                              ___________                                       |
+--------------------------------------------------------------------------------+
```

## Project Structure

```
2d-graphics-editor/
├── graphics_editor.c   # Main source file (single-file program)
├── README.md           # This file
├── .gitignore          # Git ignore rules
└── LICENSE             # MIT License
```

## Configuration

You can modify these constants at the top of `graphics_editor.c`:

| Constant | Default | Description |
|----------|---------|-------------|
| `CANVAS_ROWS` | 40 | Canvas height in characters |
| `CANVAS_COLS` | 80 | Canvas width in characters |
| `MAX_OBJECTS` | 50 | Maximum number of shapes |
| `FILL_CHAR` | `*` | Character for vertical/diagonal edges |
| `HLINE_CHAR` | `_` | Character for horizontal edges |

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
