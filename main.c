#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <windows.h>
#include <pthread.h>
#include <stdio.h>
#include "raytracing.h"

#define MAX_255(x) x > 255 ? 255 : x
#define RELU(x) x < 0 ? 0 : x
#define RANGE_0_255(x) RELU(MAX_255(x))

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void draw(HWND hwnd);


int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "mainclass";
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(100, 0, 0));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           "mainclass",         /* Classname */
           "Raytracing",       /* Title Text */
           WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           N,                 /* The programs width */
           N,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    pthread_t thread_display;
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            pthread_create(&thread_display, NULL, (void*)draw, hwnd);
            break;
        case WM_KEYUP:
            printf("%d\n", wParam);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void draw(HWND hwnd)
{
    HDC hDC;

    Vector camera;
    Vector light_pos;
    Vector light_pos2;
    Vector A, B;
    Sphere S1, S2;
    Light L1;
    Light L2;
    FloatColor red;
    FloatColor green;
    FloatColor white;
    FloatColor black;
    FloatColor background;

    Scene scene;

    vector_init(&camera, 0, 0, 100);
    vector_init(&light_pos, -1000, -500, -500);
    vector_init(&light_pos2, -900, -500, -500);
    vector_init(&A, -150, 0, -500);
    vector_init(&B, 150, 0, -500);

    floatcolor_init(&red, 0.9, 0, 0);
    floatcolor_init(&green, 0.5, 0.9, 0.2);
    floatcolor_init(&white, 1.0, 1.0, 1.0);
    floatcolor_init(&black, 0.0, 0.0, 0.0);
    floatcolor_init(&background, 0.7, 0.7, 0.7);

    S1.center = &A;
    S1.radius = 100;
    S1.color = &red;
    S1.reflexion_rate = 0.4;

    S2.center = &B;
    S2.radius = 150;
    S2.color = &white;
    S2.reflexion_rate = 0;

    L1.position = &light_pos;
    L1.color = &white;

    L2.position = &light_pos2;
    L2.color = &background;

    scene.lights[0] = &L1;
    scene.lights[1] = &L2;
    scene.lights_number = 2;
    scene.objects[0] = &S1;
    scene.objects[1] = &S2;
    scene.objects_number = 2;

    hDC = GetDC(hwnd);

    Sleep(500);

    for(int row = 0; row < N; row++)
    {
        for(int col = 0; col < N; col++)
        {
            FloatColor fc;

            get_pixel(&fc, row, col, &scene, &camera, &background, 10);
            SetPixel(hDC, col, row, RGB(RANGE_0_255(255.0*fc.red), RANGE_0_255(255.0*fc.green), RANGE_0_255(255.0*fc.blue)));
            //printf("%f %x\n", fc.red, RGB((255.0*fc.red), (255.0*fc.green), (255.0*fc.blue)));
        }
    }
}