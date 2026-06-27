#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 30
#define COLS 60
#define MAX_OBJECTS 20

char canvas[ROWS][COLS];

//Day 1

typedef enum
{
    SHAPE_CIRCLE,
    SHAPE_RECT,
    SHAPE_LINE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct
{
    ShapeType type;
    int active;

    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;

} Shape;

Shape objects[MAX_OBJECTS];

/* -------------------------------------------------- */
/* Canvas Functions */
/* -------------------------------------------------- */

void clearCanvas()
{
    for(int r=0;r<ROWS;r++)
    {
        for(int c=0;c<COLS;c++)
        {
            canvas[r][c]=' ';
        }
    }
}

void displayCanvas()
{
    printf("+");
    for(int c=0;c<COLS;c++)
        printf("-");
    printf("+\n");

    for(int r=0;r<ROWS;r++)
    {
        printf("|");

        for(int c=0;c<COLS;c++)
            printf("%c",canvas[r][c]);

        printf("|\n");
    }

    printf("+");
    for(int c=0;c<COLS;c++)
        printf("-");
    printf("+\n");
}

void setPixel(int r,int c,char ch)
{
    if(r>=0 && r<ROWS && c>=0 && c<COLS)
    {
        canvas[r][c]=ch;
    }
}

/* -------------------------------------------------- */
/* Drawing Algorithms */
/* -------------------------------------------------- */

void drawLine(int r1,int c1,int r2,int c2,char ch)
{
    int dr=abs(r2-r1);
    int dc=abs(c2-c1);

    int sr=(r1<r2)?1:-1;
    int sc=(c1<c2)?1:-1;

    int err=dr-dc;

    while(1)
    {
        setPixel(r1,c1,ch);

        if(r1==r2 && c1==c2)
            break;

        int e2=2*err;

        if(e2>-dc)
        {
            err-=dc;
            r1+=sr;
        }

        if(e2<dr)
        {
            err+=dr;
            c1+=sc;
        }
    }
}

void drawCircle(int cr,int cc,int radius,char ch)
{
    int x=0;
    int y=radius;
    int d=1-radius;

    while(x<=y)
    {
        setPixel(cr+y,cc+x,ch);
        setPixel(cr-y,cc+x,ch);
        setPixel(cr+y,cc-x,ch);
        setPixel(cr-y,cc-x,ch);

        setPixel(cr+x,cc+y,ch);
        setPixel(cr-x,cc+y,ch);
        setPixel(cr+x,cc-y,ch);
        setPixel(cr-x,cc-y,ch);

        if(d<0)
        {
            d+=2*x+3;
        }
        else
        {
            d+=2*(x-y)+5;
            y--;
        }

        x++;
    }
}

void drawRect(int r1,int c1,int r2,int c2,char ch)
{
    if(r1>r2)
    {
        int t=r1;
        r1=r2;
        r2=t;
    }

    if(c1>c2)
    {
        int t=c1;
        c1=c2;
        c2=t;
    }

    for(int c=c1;c<=c2;c++)
    {
        setPixel(r1,c,ch);
        setPixel(r2,c,ch);
    }

    for(int r=r1;r<=r2;r++)
    {
        setPixel(r,c1,ch);
        setPixel(r,c2,ch);
    }
}

void drawTriangle(int r1,int c1,
                  int r2,int c2,
                  int r3,int c3,
                  char ch)
{
    drawLine(r1,c1,r2,c2,ch);
    drawLine(r2,c2,r3,c3,ch);
    drawLine(r3,c3,r1,c1,ch);
}

/* -------------------------------------------------- */
/* Object Handling */
/* -------------------------------------------------- */

int newSlot()
{
    for(int i=0;i<MAX_OBJECTS;i++)
    {
        if(objects[i].active==0)
            return i;
    }

    printf("Maximum objects reached (%d)\n",MAX_OBJECTS);
    return -1;
}

void redrawAll()
{
    clearCanvas();

    for(int i=0;i<MAX_OBJECTS;i++)
    {
        if(!objects[i].active)
            continue;

        Shape *s=&objects[i];

        switch(s->type)
        {
            case SHAPE_CIRCLE:
                drawCircle(
                    s->y1,
                    s->x1,
                    s->radius,
                    '*'
                );
                break;

            case SHAPE_RECT:
                drawRect(
                    s->y1,
                    s->x1,
                    s->y2,
                    s->x2,
                    '*'
                );
                break;

            case SHAPE_LINE:
                drawLine(
                    s->y1,
                    s->x1,
                    s->y2,
                    s->x2,
                    '_'
                );
                break;

            case SHAPE_TRIANGLE:
                drawTriangle(
                    s->y1,s->x1,
                    s->y2,s->x2,
                    s->y3,s->x3,
                    '*'
                );
                break;
        }
    }
}

/* -------------------------------------------------- */
/* Add Shapes */
/* -------------------------------------------------- */

void addCircle()
{
    int cx,cy,r;

    printf("Center column: ");
    scanf("%d",&cx);

    printf("Center row: ");
    scanf("%d",&cy);

    printf("Radius: ");
    scanf("%d",&r);

    int i=newSlot();

    if(i==-1)
        return;

    objects[i]=(Shape)
    {
        SHAPE_CIRCLE,
        1,
        cx,cy,
        0,0,
        0,0,
        r
    };

    printf("Circle added.\n");
}

void addRect()
{
    int c1,r1,c2,r2;

    printf("Top-left (col row): ");
    scanf("%d %d",&c1,&r1);

    printf("Bottom-right (col row): ");
    scanf("%d %d",&c2,&r2);

    int i=newSlot();

    if(i==-1)
        return;

    objects[i]=(Shape)
    {
        SHAPE_RECT,
        1,
        c1,r1,
        c2,r2,
        0,0,
        0
    };

    printf("Rectangle added.\n");
}

void addLineShape()
{
    int c1,r1,c2,r2;

    printf("Start (col row): ");
    scanf("%d %d",&c1,&r1);

    printf("End (col row): ");
    scanf("%d %d",&c2,&r2);

    int i=newSlot();

    if(i==-1)
        return;

    objects[i]=(Shape)
    {
        SHAPE_LINE,
        1,
        c1,r1,
        c2,r2,
        0,0,
        0
    };

    printf("Line added.\n");
}

void addTriangle()
{
    int c1,r1,c2,r2,c3,r3;

    printf("Vertex 1 (col row): ");
    scanf("%d %d",&c1,&r1);

    printf("Vertex 2 (col row): ");
    scanf("%d %d",&c2,&r2);

    printf("Vertex 3 (col row): ");
    scanf("%d %d",&c3,&r3);

    int i=newSlot();

    if(i==-1)
        return;

    objects[i]=(Shape)
    {
        SHAPE_TRIANGLE,
        1,
        c1,r1,
        c2,r2,
        c3,r3,
        0
    };

    printf("Triangle added.\n");
}

/* -------------------------------------------------- */
/* List Objects */
/* -------------------------------------------------- */

void listObjects()
{
    int found=0;

    printf("\nACTIVE OBJECTS\n");
    printf("------------------------------------------------\n");

    for(int i=0;i<MAX_OBJECTS;i++)
    {
        if(!objects[i].active)
            continue;

        found=1;

        printf("ID %d : ",i+1);

        switch(objects[i].type)
        {
            case SHAPE_CIRCLE:
                printf("Circle\n");
                break;

            case SHAPE_RECT:
                printf("Rectangle\n");
                break;

            case SHAPE_LINE:
                printf("Line\n");
                break;

            case SHAPE_TRIANGLE:
                printf("Triangle\n");
                break;
        }
    }

    if(!found)
        printf("No active objects.\n");

    printf("\n");
}

void deleteObject()
{
    listObjects();

    int id;

    printf("Enter ID to delete: ");
    scanf("%d",&id);

    if(id<1 || id>MAX_OBJECTS)
    {
        printf("Invalid ID\n");
        return;
    }

    if(!objects[id-1].active)
    {
        printf("Object already deleted.\n");
        return;
    }

    objects[id-1].active=0;

    printf("Object deleted.\n");
}

/* -------------------------------------------------- */
/* Menus */
/* -------------------------------------------------- */

void addMenu()
{
    int ch;

    printf("\n");
    printf("1. Circle\n");
    printf("2. Rectangle\n");
    printf("3. Line\n");
    printf("4. Triangle\n");
    printf("Choice: ");

    scanf("%d",&ch);

    switch(ch)
    {
        case 1: addCircle(); break;
        case 2: addRect(); break;
        case 3: addLineShape(); break;
        case 4: addTriangle(); break;
        default: printf("Invalid choice\n");
    }

    redrawAll();
}

void mainMenu()
{
    int choice;

    while(1)
    {
        printf("\n");
        printf("====================================\n");
        printf("      2D ASCII GRAPHICS EDITOR\n");
        printf("====================================\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. List Objects\n");
        printf("4. Show Canvas\n");
        printf("5. Clear All\n");
        printf("6. Exit\n");
        printf("Choice: ");

        scanf("%d",&choice);

        switch(choice)
        {
            case 1:
                addMenu();
                break;

            case 2:
                deleteObject();
                redrawAll();
                break;

            case 3:
                listObjects();
                break;

            case 4:
                redrawAll();
                displayCanvas();
                break;

            case 5:
                memset(objects,0,sizeof(objects));
                clearCanvas();
                printf("Canvas cleared.\n");
                break;

            case 6:
                printf("Goodbye!\n");
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}

/* -------------------------------------------------- */
/* Main */
/* -------------------------------------------------- */

int main()
{
    memset(objects,0,sizeof(objects));
    clearCanvas();

    mainMenu();

    return 0;
}
// Day 2: Added shape drawing functions
// Day 3: Added menu and object management
