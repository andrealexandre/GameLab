#include <windows.h> // include the basic windows header file
#include <windowsx.h>
#include <d3d9.h> // include Direct3D header file
#include <d3dx9.h>
#include <tchar.h> // include tchar header file

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// define the screen resolution
#define SCREEN_WIDTH  1366
#define SCREEN_HEIGHT 768

// custom flexible vertex format
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// global declarations
LPDIRECT3D9 d3d;			// the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;	// the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL; // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL; // the pointer to the index buffer

// function prototypes
void InitD3D(HWND hWnd); // sets up to our Direct3D interface
void Render_Frame(void); // the pointer to the device class
void CleanD3D(void);	 // closes Direct3D and releases memory
void Init_Graphics(void); // 3D declarations

//  the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// define vertex struct
typedef struct Custom_Vertex
{
	FLOAT X, Y, Z; // from the D3DFVF_XYZ flag
	DWORD COLOR; // from the D3DFVF_DIFFUSE flag
} CUSTOMVERTEX;

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	//clear out the window class for use;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//fill in the street with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wc.lpszClassName = _T("WindowClass1");

	// register the window class
	RegisterClassEx(&wc);
			
	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		_T("WindowClass1"), //name of the window class
		_T("Our Direct3D Program"), // title of the window
		WS_EX_TOPMOST | WS_POPUP, // fullscreen values
		0, 0,    // the starting x and y positions should be 0
		SCREEN_WIDTH, SCREEN_HEIGHT,    // set the window to 800 x 600
		NULL, // we have no parent window, NULL
		NULL, // we aren't using menus, NULL
		hInstance, // application handle
		NULL); //used with multiple windows, NULL

	// display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	InitD3D(hWnd);
	// enter the main loop

	// this struct holds Windows event messages
	MSG msg;

	//UINT_PTR timer = SetTimer(hWnd, 0, 25, NULL);

	// Enter the infinite message loop
	while(TRUE)
	{
		// Check to see if any messages are waiting in the queue
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;
				
		Render_Frame();
	}

	//KillTimer(hWnd, timer);

	// clean up DirectX and COM
	CleanD3D();

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch(message)
	{
		case WM_TIMER:
			{				
				//index -= 0.05f;
			} break;
		// post a message to shutdown the program if ESC is pressed
		case WM_KEYUP:
			{
				if( wParam != VK_ESCAPE) break;
			}
		// this messsage is read when the window is closed
		case WM_DESTROY:
			{
				// close the application entirely				
				PostQuitMessage(0);
				return 0;
			} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp; // create a struct to hold a various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp)); // clear out the struct for use
	d3dpp.Windowed = FALSE; // program fullscreen, not windowed
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard old frames
	d3dpp.hDeviceWindow = hWnd; // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = SCREEN_WIDTH; // set the width of the buffer
	d3dpp.BackBufferHeight = SCREEN_HEIGHT; // set the height of the buffer
	d3dpp.EnableAutoDepthStencil = TRUE; // automatically run the z-buffer for us
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // 16-bit pixel format for the z-buffer

	// create a device class using this information and information from the d3dpp struct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						hWnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&d3dpp,
						&d3ddev);

	Init_Graphics(); // call the function to initialize the triangle

	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE); // turn off the 3D lighting
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // both sides of the triangles are shown
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE); // turn on the z-buffer
}

// this is a the function used to render a single frame
void Render_Frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	d3ddev->BeginScene(); //begins the 3D scene

		// select which vextex format we are using
		d3ddev->SetFVF(CUSTOMFVF);

		// SET UP THE PIPELINE
		D3DXMATRIX matView; // the view transform matrix

		D3DXMatrixLookAtLH(&matView,
							&D3DXVECTOR3(0.0f,0.0f,10.0f), // the camera position
							&D3DXVECTOR3(0.0f,0.0f,0.0f), // the look-at position
							&D3DXVECTOR3(0.0f,1.0f,0.0f) ); // the up direction

		d3ddev->SetTransform(D3DTS_VIEW, &matView); // set the view transform to matView

		D3DXMATRIX matProjection; // the projection transform matrix
		
		D3DXMatrixPerspectiveFovLH(&matProjection,
									D3DXToRadian(45), // the horizontal field of view
									(FLOAT)SCREEN_WIDTH/(FLOAT)SCREEN_HEIGHT, // aspect ratio
									1.0f, // the near view-plane
									100.0f); // the far view-plane

		d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection); //set projection

		//select the vertex and index buffers to use
		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		d3ddev->SetIndices(i_buffer);

		D3DXMATRIX matTranslateA; // a matrix to store the translation for triangle A
		D3DXMATRIX matTranslateB; // a matrix to store the translation for triangle B		
		D3DXMATRIX matRotateY; // a matrix to store the rotation for each triangle		
		static float index = 0.0f; index += 0.05f; // an ever-increasing float value
		
		// build MULTIPLE matrices to translate the model and one to rotate		
		D3DXMatrixRotationY(&matRotateY, -index); // the front side
		
		// tell Direct3D about each world transform, and then draw another triangle
		d3ddev->SetTransform(D3DTS_WORLD, &matRotateY);

		//d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		//d3ddev->SetTransform(D3DTS_WORLD, &(matTranslateB * matRotateY));
		//d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
	
	d3ddev->EndScene(); //ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL); // displays the created frame
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
	v_buffer->Release(); // close and release the vertex buffer
	d3ddev->Release(); // close and release the 3D device
	d3d-> Release(); // close and release Direct3D
}

// this is the function tha puts the 3D models into video RAM
void Init_Graphics(void)
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX t_vert[] =
	{
		{ -3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255) },	// vertex 0
		{ 3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 0) },	// vertex 1
		{ -3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0) },	// 2
		{ 3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255) },	// 3
		{ -3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255) },	// ...
		{ 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(255, 0, 0) },
		{ -3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0) },
		{ 3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 255) }
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer( 4 * sizeof(CUSTOMVERTEX),
								0,
								CUSTOMFVF,
								D3DPOOL_MANAGED,
								&v_buffer,
								NULL);

	VOID* pVoid; // the void pointer

	// lock v_buffer and load the vertices into it
	v_buffer->Lock(0,0, (void**) &pVoid, 0);
	memcpy(pVoid, t_vert, sizeof(t_vert));
	v_buffer->Unlock();

	// create the indices using an int array
	short indices[] =
	{
		0, 1, 2,	// side 1
		2, 1, 3,		
		4, 0, 6,	// side 2		
		6, 0, 2,
		7, 5, 6,	// side 3
		6, 5, 4,
		3, 1, 7,	// side 4
		7, 1, 5,
		4, 5, 0,	// side 5
		0, 5, 1,
		3, 7, 2,	// side 6
		2, 7, 6
	};
	
	// create an index buffer interface called i_buffer
	d3ddev->CreateIndexBuffer( 36 * sizeof(short), // 3 per triangle, 12;
								0,
								D3DFMT_INDEX16,
								D3DPOOL_MANAGED,
								&i_buffer,
								NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();
}