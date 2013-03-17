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
struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

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
void Init_Light(void); // sets up the light and the material

//  the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

	//UINT_PTR timer = SetTimer(hWnd, 0, 2000, NULL);

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
				
			} break;
		// post a message to shutdown the program if ESC is pressed
		case WM_KEYUP:
			{
				if(wParam == VK_SPACE)
				{
					static BOOL light1 = true; light1 = !light1;
					d3ddev->LightEnable(0, light1);
				}				

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

	Init_Graphics(); // call the function to initialize the models
	Init_Light(); // call the function to initialize the light and material

	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE); // turn off the 3D lighting
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE); // turn on the z-buffer
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50)); // ambient light
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

		// set the view transform
		D3DXMATRIX matView; // the view transform matrix
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f,8.0f,25.0f), // the camera position
			&D3DXVECTOR3(0.0f,0.0f,0.0f), // the look-at position
			&D3DXVECTOR3(0.0f,1.0f,0.0f) ); // the up direction
		d3ddev->SetTransform(D3DTS_VIEW, &matView);
		
		// set the projection transform
		D3DXMATRIX matProjection;		
		D3DXMatrixPerspectiveFovLH(&matProjection,
									D3DXToRadian(45), // the horizontal field of view
									(FLOAT)SCREEN_WIDTH/(FLOAT)SCREEN_HEIGHT, // aspect ratio
									1.0f, // the near view-plane
									100.0f); // the far view-plane
		d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);
		
		// select the vertex and index buffers to use
		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
		d3ddev->SetIndices(i_buffer);

		// set the world transform
		static float index = 0; index += 0.05f;
		D3DXMATRIX matRotateY;		
		D3DXMatrixRotationY(&matRotateY, index);				
		d3ddev->SetTransform(D3DTS_WORLD, &(matRotateY));

		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 8);
	
	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL); // displays the created frame
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
	v_buffer->Release(); // close and release the vertex buffer
	i_buffer->Release(); // close and release the vertex buffer
	d3ddev->Release(); // close and release the 3D device
	d3d-> Release(); // close and release Direct3D
}

// this is the function tha puts the 3D models into video RAM
void Init_Graphics(void)
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX t_vert[] =
	{
		// fuselage
		{ 0.0f, 3.0f, -3.0f, -0.7992f, 1.0f, 0.2307f },  // 0
		{ -3.0f, 0.0f, 0.0f, -0.7992f, 1.0f, 0.2307f },
		{ 0.0f, 0.0f, 10.0f, -0.7992f, 1.0f, 0.2307f },

		{ 3.0f, 0.0f, 0.0f, 0.7992f, 1.0f, 0.2307f },	// 3
		{ 0.0f, 3.0f, -3.0f, 0.7992f, 1.0f, 0.2307f },
		{ 0.0f, 0.0f, 10.0f, 0.7992f, 1.0f, 0.2307f },

		{ 3.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f },	// 6
		{ -3.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f },
		{ 0.0f, 3.0f, -3.0f, 0.0f, -1.0f, -1.0f },

		{ 3.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },		// 9
		{ 0.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f },
		{ -3.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },

		// left gun
		{ 3.2f, -1.0f, -3.0f, 0.1666f, 1.0f, 0.0f },	// 12		
		{ 2.0f, 1.0f, 2.0f, 0.1666f, 1.0f, 0.0f },
		{ 3.2f, -1.0f, 11.0f, 0.1666f, 1.0f, 0.0f },
		
		{ 3.2f, -1.0f, -3.0f, -0.1666f, -1.0f, 0.0f },	// 15		
		{ 3.2f, -1.0f, 11.0f, -0.1666f, -1.0f, 0.0f },
		{ 2.0f, 1.0f, 2.0f, -0.1666f, -1.0f, 0.0f },

		// right gun
		{ -3.2f, -1.0f, -3.0f, 0.0384f, -1.0f, 0.0f },	// 18		
		{ -2.0f, 1.0f, 2.0f, 0.0384f, -1.0f, 0.0f },
		{ -3.2f, -1.0f, 11.0f, 0.0384f, -1.0f, 0.0f },

		{ -3.2f, -1.0f, -3.0f, -0.0384f, 1.0f, 0.0f },	// 21
		{ -3.2f, -1.0f, 11.0f, -0.0384f, 1.0f, 0.0f },
		{ -2.0f, 1.0f, 2.0f, -0.0384f, 1.0f, 0.0f }
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer( 24 * sizeof(CUSTOMVERTEX),
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
		0, 1, 2,    // fuselage
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23				
	};
	
	// create an index buffer interface called i_buffer
	d3ddev->CreateIndexBuffer( (8 * 3) * sizeof(short), // 3 per triangle, 12;
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

// this is the function that sets up the lights and materials
void Init_Light(void)
{
	D3DLIGHT9 light; // create a light struct	
	D3DMATERIAL9 material;	// create the material struct

	ZeroMemory(&light, sizeof(light)); // clear out the light struct for use
	light.Type = D3DLIGHT_DIRECTIONAL; // make the light type 'directional light'
	light.Diffuse = D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f); // set the light's color
	light.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);

	d3ddev->SetLight(0, &light); // send the light struct properties to light #0
	d3ddev->LightEnable(0, TRUE); // turn on light #0
	
	ZeroMemory(&material, sizeof(material)); // clear out the struct for use
	material.Diffuse = D3DXCOLOR(0.8f, 0.0f, 0.0f, 1.0f); // set diffuse color to white	
	material.Ambient = D3DXCOLOR(0.4f, 0.0f, 0.0f, 1.0f); // set ambient color to white	

	d3ddev->SetMaterial(&material); // set the globally-used material to &material
}