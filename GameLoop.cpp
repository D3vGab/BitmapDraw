/**********************************************************************************
// GameLoop
// Descrição: Esqueleto de um laço de tempo real para criar um jogo.
**********************************************************************************/

#include <windows.h>        // inclui funções do windows
#include <windowsx.h>       // inclui funções extras do windows
#include <random>
//#include "Resources.h"      // definições dos recursos utilizados

using namespace std;
// protótipo do procedimento da janela
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

// -----------------------------------------------------------------------
// constantes, variáveis e funções globais

int windowWidth = 960;     // largura da janela
int windowHeight = 540;     // altura da janela
bool fullScreen = FALSE;    // tela cheia ou modo janela

bool vkKeys[256] = { 0 };   // estado das teclas do teclado
HWND hwnd;                  // identificador da janela
HDC     hdc;                // dispositivo gráfico
int     x, y;               // posição
int     velX, velY;         // velocidade

// contantes e variáveis da imagem Bitmap

HBITMAP image;
BITMAP  bm;
HDC     hdcImg;
// -----------------------------------------------------------------------
// funções do jogo

// inicializa o jogo alocando recursos
void GameInit()
{
    hdc = GetDC(hwnd);
    hdcImg = CreateCompatibleDC(hdc);

    image = (HBITMAP) LoadImage (
        NULL,                               // nulo para Bitmap
        "Resources\\CarKara.bmp",        // destino do Bitmap
        IMAGE_BITMAP,                       // tipo de recurso
        0,                                  // largura do Bitmap
        0,                                  // altura do Bitmap
        LR_LOADFROMFILE                     // tipo de carregamento
        );

    // le as propiedades da imagem

    GetObject(image, sizeof(BITMAP), &bm);
    SelectObject(hdcImg, image);

    x = 0;
    y = 0;
    velX = 5;
    velY = 5;
}

// atualiza a lógica do jogo
void GameUpdate()
{
    // sai com o pressionamento da tecla ESC
    if (vkKeys[VK_ESCAPE])
        PostMessage(hwnd, WM_DESTROY, 0, 0);

    x += velX;
    y += velY;

    // testar colisão
    if (y + bm.bmHeight > windowHeight)
        velY = -velY;
    if (x + bm.bmWidth > windowWidth)
        velX = -velX;
    if (y < 0)
        velY = -velY;
    if (x < 0)
        velX = -velX;
}

// desenha o próximo quadro do jogo
void GameDraw()
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 0, 0)));

    BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcImg, 0, 0, SRCCOPY);

}

// finaliza o jogo desalocando recursos
void GameFinalize()
{
    DeleteDC(hdcImg);
    DeleteObject((HBITMAP)image);

    ReleaseDC(hwnd, hdc);
}

// -----------------------------------------------------------------------

// programa principal
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    MSG			msg;
    WNDCLASS	wndclass;

    // definindo uma classe de janela chamada "GameWindow" 
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WinProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "GameWindow";

    // registrando "GameWindow"
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, "Erro na criação da janela!", "Game", MB_ICONERROR);
        return 0;
    }

    int windowStyle = WS_OVERLAPPED | WS_SYSMENU;

    // tela cheia ou janela
    if (fullScreen)
    {
        windowWidth = GetSystemMetrics(SM_CXSCREEN);
        windowHeight = GetSystemMetrics(SM_CYSCREEN);
        windowStyle = WS_POPUP;
    }

    // criando uma janela baseada na classe "GameWindow"
    // é necessário usar CreateWindowEx para usar AdjustWindowRectEx
    hwnd = CreateWindowEx(
        NULL,                           // estilos extras
        "GameWindow",                   // nome da "window class"
        "Game",                         // título da janela
        windowStyle,                    // estilo da janela
        CW_USEDEFAULT,                  // posição x inicial
        CW_USEDEFAULT,                  // posição y inicial
        windowWidth,                    // largura da janela
        windowHeight,                   // altura da janela
        NULL,                           // identificador do objeto pai
        NULL,                           // identificador do menu
        hInstance,                      // identificador da aplicação
        NULL);                          // parâmetros de criação

    if (!fullScreen)
    {
        // retângulo com o tamanho da área cliente desejada
        RECT winRect = { 0, 0, windowWidth, windowHeight };

        // ajuste do tamanho da janela
        AdjustWindowRectEx(&winRect,
            GetWindowStyle(hwnd),
            GetMenu(hwnd) != NULL,
            GetWindowExStyle(hwnd));

        // atualiza posição da janela
        int windowPosX = (GetSystemMetrics(SM_CXSCREEN) / 2) - ((winRect.right - winRect.left) / 2);
        int windowPosY = (GetSystemMetrics(SM_CYSCREEN) / 2) - ((winRect.bottom - winRect.top) / 2);

        // redimensiona janela com uma chamada a MoveWindow
        MoveWindow(
            hwnd,                           // identificador da janela
            windowPosX,                     // posição x
            windowPosY,                     // posição y
            winRect.right - winRect.left,   // largura
            winRect.bottom - winRect.top,   // altura
            TRUE);                          // repintar
    }

    // mostra e atualiza a janela
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    GameInit();

    // loop principal do jogo
    do
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            GameUpdate();
            GameDraw();
            Sleep(10);
        }
    } while (msg.message != WM_QUIT);

    GameFinalize();

    // fim do programa
    return int(msg.wParam);
}

// -----------------------------------------------------------------------

// procedimento da janela
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // processa teclas pressionadas
    case WM_KEYDOWN:
        vkKeys[wParam] = true;
        return 0;

        // processa teclas liberadas
    case WM_KEYUP:
        vkKeys[wParam] = false;
        return 0;

        // janela foi fechada
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
// -----------------------------------------------------------------------
