/*
 * 2D Graphics Editor in C
 * =======================
 * A menu-driven console graphics editor that uses '*' and '_' characters
 * to draw shapes (circle, rectangle, line, triangle) on a 2D character canvas.
 *
 * Features:
 *   - Add objects: circle, rectangle, line, triangle
 *   - Delete objects by ID
 *   - Display the canvas
 *   - Clear the entire canvas
 *
 * Author: Graphics Editor Project
 * License: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ----- Configuration ----- */
#define CANVAS_ROWS    40
#define CANVAS_COLS    80
#define MAX_OBJECTS    50
#define EMPTY_CHAR     ' '
#define FILL_CHAR      '*'
#define HLINE_CHAR     '_'

/* ----- Shape Types ----- */
typedef enum {
    SHAPE_NONE,
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE,
    SHAPE_LINE,
    SHAPE_TRIANGLE
} ShapeType;

/* ----- Shape Data ----- */
typedef struct {
    int id;
    ShapeType type;

    /* Circle: (cx, cy) center, r radius */
    /* Rectangle: (x1, y1) top-left corner, width w, height h */
    /* Line: (x1, y1) start, (x2, y2) end */
    /* Triangle: three vertices (x1,y1), (x2,y2), (x3,y3) */
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int r;       /* radius for circle */
    int w, h;    /* width, height for rectangle */
} Shape;

/* ----- Global State ----- */
char canvas[CANVAS_ROWS][CANVAS_COLS];
Shape objects[MAX_OBJECTS];
int objectCount = 0;
int nextId = 1;

/* ========================================================================= */
/*                           Canvas Utilities                                */
/* ========================================================================= */

/* Clear the canvas to all spaces */
void clearCanvas(void) {
    for (int r = 0; r < CANVAS_ROWS; r++)
        for (int c = 0; c < CANVAS_COLS; c++)
            canvas[r][c] = EMPTY_CHAR;
}

/* Safely set a pixel on the canvas */
void setPixel(int row, int col, char ch) {
    if (row >= 0 && row < CANVAS_ROWS && col >= 0 && col < CANVAS_COLS)
        canvas[row][col] = ch;
}

/* Display the canvas with a border */
void displayCanvas(void) {
    printf("\n");

    /* Top border */
    printf("+");
    for (int c = 0; c < CANVAS_COLS; c++) printf("-");
    printf("+\n");

    /* Canvas rows */
    for (int r = 0; r < CANVAS_ROWS; r++) {
        printf("|");
        for (int c = 0; c < CANVAS_COLS; c++)
            putchar(canvas[r][c]);
        printf("|\n");
    }

    /* Bottom border */
    printf("+");
    for (int c = 0; c < CANVAS_COLS; c++) printf("-");
    printf("+\n");
}

/* ========================================================================= */
/*                           Drawing Functions                               */
/* ========================================================================= */

/*
 * Draw a circle using the midpoint circle algorithm.
 * Uses '*' for the outline. Horizontal segments use '_'.
 */
void drawCircle(int cy, int cx, int radius) {
    if (radius <= 0) return;

    int x = 0, y = radius;
    int d = 1 - radius;

    while (x <= y) {
        /* Eight symmetric points of the circle */
        /* Top and bottom arcs use '_' for horizontal feel */
        setPixel(cy - y, cx + x, (y == radius) ? HLINE_CHAR : FILL_CHAR);
        setPixel(cy - y, cx - x, (y == radius) ? HLINE_CHAR : FILL_CHAR);
        setPixel(cy + y, cx + x, (y == radius) ? HLINE_CHAR : FILL_CHAR);
        setPixel(cy + y, cx - x, (y == radius) ? HLINE_CHAR : FILL_CHAR);

        setPixel(cy - x, cx + y, FILL_CHAR);
        setPixel(cy - x, cx - y, FILL_CHAR);
        setPixel(cy + x, cx + y, FILL_CHAR);
        setPixel(cy + x, cx - y, FILL_CHAR);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

/*
 * Draw a rectangle outline.
 * Top and bottom edges use '_', left and right edges use '*'.
 */
void drawRectangle(int row, int col, int width, int height) {
    if (width <= 0 || height <= 0) return;

    /* Top edge */
    for (int c = col; c < col + width; c++)
        setPixel(row, c, HLINE_CHAR);

    /* Bottom edge */
    for (int c = col; c < col + width; c++)
        setPixel(row + height - 1, c, HLINE_CHAR);

    /* Left edge */
    for (int r = row; r < row + height; r++)
        setPixel(r, col, FILL_CHAR);

    /* Right edge */
    for (int r = row; r < row + height; r++)
        setPixel(r, col + width - 1, FILL_CHAR);

    /* Corners are '*' */
    setPixel(row, col, FILL_CHAR);
    setPixel(row, col + width - 1, FILL_CHAR);
    setPixel(row + height - 1, col, FILL_CHAR);
    setPixel(row + height - 1, col + width - 1, FILL_CHAR);
}

/*
 * Draw a line from (r1,c1) to (r2,c2) using Bresenham's algorithm.
 * Mostly-horizontal segments use '_', others use '*'.
 */
void drawLine(int r1, int c1, int r2, int c2) {
    int dr = abs(r2 - r1);
    int dc = abs(c2 - c1);
    int sr = (r1 < r2) ? 1 : -1;
    int sc = (c1 < c2) ? 1 : -1;
    int err = dc - dr;

    while (1) {
        /* Use '_' for horizontal or near-horizontal segments */
        char ch = (dr == 0) ? HLINE_CHAR : FILL_CHAR;
        setPixel(r1, c1, ch);

        if (r1 == r2 && c1 == c2) break;

        int e2 = 2 * err;
        if (e2 > -dr) { err -= dr; c1 += sc; }
        if (e2 <  dc) { err += dc; r1 += sr; }
    }
}

/*
 * Draw a triangle outline by connecting three vertices with lines.
 */
void drawTriangle(int r1, int c1, int r2, int c2, int r3, int c3) {
    drawLine(r1, c1, r2, c2);
    drawLine(r2, c2, r3, c3);
    drawLine(r3, c3, r1, c1);
}

/* ========================================================================= */
/*                         Object Management                                 */
/* ========================================================================= */

/*
 * Redraw the entire canvas from the object list.
 * Called after any add/delete so the canvas stays consistent.
 */
void redrawAll(void) {
    clearCanvas();
    for (int i = 0; i < objectCount; i++) {
        Shape *s = &objects[i];
        switch (s->type) {
            case SHAPE_CIRCLE:
                drawCircle(s->y1, s->x1, s->r);
                break;
            case SHAPE_RECTANGLE:
                drawRectangle(s->y1, s->x1, s->w, s->h);
                break;
            case SHAPE_LINE:
                drawLine(s->y1, s->x1, s->y2, s->x2);
                break;
            case SHAPE_TRIANGLE:
                drawTriangle(s->y1, s->x1, s->y2, s->x2, s->y3, s->x3);
                break;
            default:
                break;
        }
    }
}

/* Add a shape to the object list and redraw */
int addObject(Shape shape) {
    if (objectCount >= MAX_OBJECTS) {
        printf("  [Error] Maximum number of objects (%d) reached.\n", MAX_OBJECTS);
        return -1;
    }
    shape.id = nextId++;
    objects[objectCount++] = shape;
    redrawAll();
    printf("  [OK] Object added with ID %d.\n", shape.id);
    return shape.id;
}

/* Delete a shape by ID and redraw */
int deleteObject(int id) {
    int found = -1;
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].id == id) {
            found = i;
            break;
        }
    }
    if (found == -1) {
        printf("  [Error] Object with ID %d not found.\n", id);
        return 0;
    }
    /* Shift remaining objects left */
    for (int i = found; i < objectCount - 1; i++)
        objects[i] = objects[i + 1];
    objectCount--;
    redrawAll();
    printf("  [OK] Object %d deleted.\n", id);
    return 1;
}

/* List all current objects */
void listObjects(void) {
    if (objectCount == 0) {
        printf("  (no objects on canvas)\n");
        return;
    }
    printf("\n  %-4s  %-12s  %s\n", "ID", "Type", "Parameters");
    printf("  %-4s  %-12s  %s\n", "----", "------------", "------------------------------");
    for (int i = 0; i < objectCount; i++) {
        Shape *s = &objects[i];
        printf("  %-4d  ", s->id);
        switch (s->type) {
            case SHAPE_CIRCLE:
                printf("%-12s  center=(%d,%d) radius=%d\n", "Circle", s->x1, s->y1, s->r);
                break;
            case SHAPE_RECTANGLE:
                printf("%-12s  top-left=(%d,%d) w=%d h=%d\n", "Rectangle", s->x1, s->y1, s->w, s->h);
                break;
            case SHAPE_LINE:
                printf("%-12s  (%d,%d) -> (%d,%d)\n", "Line", s->x1, s->y1, s->x2, s->y2);
                break;
            case SHAPE_TRIANGLE:
                printf("%-12s  (%d,%d) (%d,%d) (%d,%d)\n", "Triangle",
                       s->x1, s->y1, s->x2, s->y2, s->x3, s->y3);
                break;
            default:
                printf("%-12s\n", "Unknown");
        }
    }
    printf("\n");
}

/* ========================================================================= */
/*                         Input Helpers                                     */
/* ========================================================================= */

/* Read an integer with a prompt; returns 1 on success */
int readInt(const char *prompt, int *value) {
    printf("  %s", prompt);
    if (scanf("%d", value) != 1) {
        /* Flush bad input */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("  [Error] Invalid number.\n");
        return 0;
    }
    return 1;
}

/* Validate that coordinates are within the canvas */
int validateCoord(int row, int col) {
    if (row < 0 || row >= CANVAS_ROWS || col < 0 || col >= CANVAS_COLS) {
        printf("  [Warning] Coordinates (%d, %d) are outside the canvas (%dx%d).\n",
               col, row, CANVAS_COLS, CANVAS_ROWS);
        printf("  Parts of the shape outside the canvas will be clipped.\n");
    }
    return 1;  /* Allow it — clipping is handled by setPixel */
}

/* ========================================================================= */
/*                           Menu Handlers                                   */
/* ========================================================================= */

void menuAddCircle(void) {
    Shape s = {0};
    s.type = SHAPE_CIRCLE;
    printf("\n  --- Add Circle ---\n");
    printf("  Canvas size: %d cols x %d rows (col: 0-%d, row: 0-%d)\n",
           CANVAS_COLS, CANVAS_ROWS, CANVAS_COLS - 1, CANVAS_ROWS - 1);
    if (!readInt("Center column (x): ", &s.x1)) return;
    if (!readInt("Center row    (y): ", &s.y1)) return;
    if (!readInt("Radius           : ", &s.r))  return;
    if (s.r <= 0) { printf("  [Error] Radius must be positive.\n"); return; }
    validateCoord(s.y1, s.x1);
    addObject(s);
}

void menuAddRectangle(void) {
    Shape s = {0};
    s.type = SHAPE_RECTANGLE;
    printf("\n  --- Add Rectangle ---\n");
    printf("  Canvas size: %d cols x %d rows (col: 0-%d, row: 0-%d)\n",
           CANVAS_COLS, CANVAS_ROWS, CANVAS_COLS - 1, CANVAS_ROWS - 1);
    if (!readInt("Top-left column (x): ", &s.x1)) return;
    if (!readInt("Top-left row    (y): ", &s.y1)) return;
    if (!readInt("Width              : ", &s.w))   return;
    if (!readInt("Height             : ", &s.h))   return;
    if (s.w <= 0 || s.h <= 0) { printf("  [Error] Width and height must be positive.\n"); return; }
    validateCoord(s.y1, s.x1);
    addObject(s);
}

void menuAddLine(void) {
    Shape s = {0};
    s.type = SHAPE_LINE;
    printf("\n  --- Add Line ---\n");
    printf("  Canvas size: %d cols x %d rows (col: 0-%d, row: 0-%d)\n",
           CANVAS_COLS, CANVAS_ROWS, CANVAS_COLS - 1, CANVAS_ROWS - 1);
    if (!readInt("Start column (x1): ", &s.x1)) return;
    if (!readInt("Start row    (y1): ", &s.y1)) return;
    if (!readInt("End column   (x2): ", &s.x2)) return;
    if (!readInt("End row      (y2): ", &s.y2)) return;
    addObject(s);
}

void menuAddTriangle(void) {
    Shape s = {0};
    s.type = SHAPE_TRIANGLE;
    printf("\n  --- Add Triangle ---\n");
    printf("  Canvas size: %d cols x %d rows (col: 0-%d, row: 0-%d)\n",
           CANVAS_COLS, CANVAS_ROWS, CANVAS_COLS - 1, CANVAS_ROWS - 1);
    if (!readInt("Vertex 1 column (x1): ", &s.x1)) return;
    if (!readInt("Vertex 1 row    (y1): ", &s.y1)) return;
    if (!readInt("Vertex 2 column (x2): ", &s.x2)) return;
    if (!readInt("Vertex 2 row    (y2): ", &s.y2)) return;
    if (!readInt("Vertex 3 column (x3): ", &s.x3)) return;
    if (!readInt("Vertex 3 row    (y3): ", &s.y3)) return;
    addObject(s);
}

void menuDeleteObject(void) {
    int id;
    printf("\n  --- Delete Object ---\n");
    listObjects();
    if (objectCount == 0) return;
    if (!readInt("Enter object ID to delete: ", &id)) return;
    deleteObject(id);
}

void menuClearAll(void) {
    objectCount = 0;
    nextId = 1;
    clearCanvas();
    printf("  [OK] Canvas cleared. All objects removed.\n");
}

/* ========================================================================= */
/*                              Main Menu                                    */
/* ========================================================================= */

void printMenu(void) {
    printf("\n");
    printf("  ============================================\n");
    printf("       2D GRAPHICS EDITOR  (Console)\n");
    printf("  ============================================\n");
    printf("    1. Add Circle\n");
    printf("    2. Add Rectangle\n");
    printf("    3. Add Line\n");
    printf("    4. Add Triangle\n");
    printf("    5. Delete Object\n");
    printf("    6. List Objects\n");
    printf("    7. Display Canvas\n");
    printf("    8. Clear Canvas\n");
    printf("    0. Exit\n");
    printf("  --------------------------------------------\n");
    printf("  Choice: ");
}

int main(void) {
    clearCanvas();

    int choice;
    int running = 1;

    printf("\n  Welcome to the 2D Graphics Editor!\n");
    printf("  Canvas size: %d columns x %d rows\n", CANVAS_COLS, CANVAS_ROWS);
    printf("  Shapes are drawn with '*' and '_' characters.\n");

    while (running) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            /* Flush bad input */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("  [Error] Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: menuAddCircle();     break;
            case 2: menuAddRectangle();  break;
            case 3: menuAddLine();       break;
            case 4: menuAddTriangle();   break;
            case 5: menuDeleteObject();  break;
            case 6: listObjects();       break;
            case 7: displayCanvas();     break;
            case 8: menuClearAll();      break;
            case 0:
                printf("\n  Goodbye!\n\n");
                running = 0;
                break;
            default:
                printf("  [Error] Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
