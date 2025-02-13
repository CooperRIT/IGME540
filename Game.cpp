#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	transform = Transform();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	//CreateGeometry();

	//Mesh Class Code
	
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 purple = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	XMFLOAT4 orange = XMFLOAT4(1.0f, 0.647f, 0.0f, 1.0f);
	XMFLOAT4 cyan = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);



	Vertex triangleVerticies[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	Vertex squareVerticies[] =
	{
		{XMFLOAT3(-1.0f, 1.0f, 0.0f), red},
		{XMFLOAT3(-.75f, 1.0f, 0.0f), blue},
		{XMFLOAT3(-1.0f, .75f, 0.0f), green},
		{XMFLOAT3(-.75, .75f, 0.0f), red},
	};

	Vertex objectVerticies[] =
	{
		{XMFLOAT3(.6f, 1.0f, 0.0f), green},
		{XMFLOAT3(.8f, 0.7f, 0.0f), green},
		{XMFLOAT3(.4f, 0.7f, 0.0f), green},
		{XMFLOAT3(0.6f, .4f, 0.0f), green},
		{XMFLOAT3(.2f, .4f, 0.0f), green},
		{XMFLOAT3(1.0f, .4f, 0.0f), green}
	};

	Vertex hexagonVerticies[] =
	{
		{XMFLOAT3(0.0f, 0.5f, 0.0f), yellow},
		{XMFLOAT3(0.5f, 0.25f, 0.0f), purple},
		{XMFLOAT3(0.5f, -0.25f, 0.0f), yellow},
		{XMFLOAT3(0.0f, -0.5f, 0.0f), purple},
		{XMFLOAT3(-0.5f, -0.25f, 0.0f), yellow},
		{XMFLOAT3(-0.5f, 0.25f, 0.0f), purple}
	};

	Vertex pentagonVerticies[] =
	{
		{XMFLOAT3(0.0f, 0.5f, 0.0f), orange},
		{XMFLOAT3(0.5f, 0.2f, 0.0f), cyan},
		{XMFLOAT3(0.3f, -0.3f, 0.0f), orange},
		{XMFLOAT3(-0.3f, -0.3f, 0.0f), cyan},
		{XMFLOAT3(-0.5f, 0.2f, 0.0f), orange}
	};


	unsigned int triangleIndicies[] = { 0, 1, 2 };

	unsigned int squareIndicies[] = 
	{ 
		0, 1, 2,
		1, 3, 2
	};

	unsigned int objectIndicies[] =
	{
		0, 1, 2,
		2, 3, 4,
		1, 5, 3
	};

	unsigned int hexagonIndicies[] =
	{
		0, 1, 5,
		1, 2, 5,
		2, 3, 5,
		3, 4, 5
	};

	unsigned int pentagonIndicies[] =
	{
		0, 1, 2,
		2, 3, 4,
		4, 0, 2
	};
	


	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);


		//Create a constant buffer
		unsigned int bufferSize = sizeof(VertexShaderToCopyToGpuToGPU);
		bufferSize = (bufferSize + 15) /16 * 16;

		D3D11_BUFFER_DESC cbDesc{};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = bufferSize;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());

		//Actually Bind The Buffer
		Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	}

	gameEntities.push_back(std::make_shared<GameEntity>(constantBuffer, Mesh(triangleVerticies, 3, triangleIndicies, 3)));
	gameEntities.push_back(std::make_shared<GameEntity>(constantBuffer, Mesh(squareVerticies, 4, squareIndicies, 6)));
	gameEntities.push_back(std::make_shared<GameEntity>(constantBuffer, Mesh(objectVerticies, 6, objectIndicies, 9)));
	gameEntities.push_back(std::make_shared<GameEntity>(constantBuffer, Mesh(hexagonVerticies, 6, hexagonIndicies, 9)));
	gameEntities.push_back(std::make_shared<GameEntity>(constantBuffer, Mesh(pentagonVerticies, 5, pentagonIndicies, 10)));

	for (int i = 0; i < 5; i++)
	{
		entityInfo.push_back
		(
			EntityInformation
			{
				{gameEntities[i]->GetTransform()->GetPosition().x, gameEntities[i]->GetTransform()->GetPosition().y, gameEntities[i]->GetTransform()->GetPosition().z},
				{gameEntities[i]->GetTransform()->GetRotation().x, gameEntities[i]->GetTransform()->GetRotation().y, gameEntities[i]->GetTransform()->GetRotation().z},
				{gameEntities[i]->GetTransform()->GetScale().x, gameEntities[i]->GetTransform()->GetScale().y, gameEntities[i]->GetTransform()->GetScale().z},
			}
		);
	}


	//Initialize Window Color and color tint
	ChangeColor(color, 0.4f, 0.6f, 0.75f, 0.0f);
	ChangeColor(&objectColorTint.x, 1.0f, 1.0f, 1.0f, 0.0f);
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//Deallocate Meshes Array
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime);

	BuildUI();

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	 //Move them on the X
	for (int i = 0; i < 5; i++)
	{
		gameEntities[i]->GetTransform()->SetPosition(sinf(totalTime), 0, 0);
		CopyInfoToStruct(i);
	}
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//triangle->Draw();
	for (const auto& obj : gameEntities) 
	{
		obj->Draw(objectColorTint);
	}

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void::Game::UpdateImGui(float deltaTime) 
{
	// Put this all in a helper method that is called from Game::Update()
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
}

void::Game::BuildUI()
{
	//Begins the creation of a new window
	ImGui::Begin("GUI Inspector");

	if (ImGui::CollapsingHeader("Application Details"))
	{
		ImGui::Text("FrameRate: %f fps", ImGui::GetIO().Framerate);

		ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

		ImGui::ColorEdit4("Background Color", color);

		ImGui::ColorEdit4("ColorTint", &objectColorTint.x);

		if (ImGui::Button("Show Demo Window"))
		{
			demoWindowState = !demoWindowState;
		}
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{

		ImGui::Text("Triangle:");
		ImGui::Text("Triangles: %i", 1);
		ImGui::Text("Vertices: %i", gameEntities[0]->GetMesh()->GetVertexCount());
		ImGui::Text("Indices: %i", gameEntities[0]->GetMesh()->GetIndexCount());
		ImGui::Text("\n");

		ImGui::Text("Quad:");
		ImGui::Text("Triangles: %i", 2);
		ImGui::Text("Vertices: %i", gameEntities[1]->GetMesh()->GetVertexCount());
		ImGui::Text("Indices: %i", gameEntities[1]->GetMesh()->GetIndexCount());
		ImGui::Text("\n");

		ImGui::Text("Object:");
		ImGui::Text("Triangles: %i", 3);
		ImGui::Text("Vertices: %i", gameEntities[2]->GetMesh()->GetVertexCount());
		ImGui::Text("Indices: %i", gameEntities[2]->GetMesh()->GetIndexCount());
		ImGui::Text("\n");

		ImGui::Text("Hexagon:");
		ImGui::Text("Triangles: %i", 6);
		ImGui::Text("Vertices: %i", gameEntities[3]->GetMesh()->GetVertexCount());
		ImGui::Text("Indices: %i", gameEntities[3]->GetMesh()->GetIndexCount());
		ImGui::Text("\n");

		ImGui::Text("Pentagon:");
		ImGui::Text("Triangles: %i", 3);
		ImGui::Text("Vertices: %i", gameEntities[4]->GetMesh()->GetVertexCount());
		ImGui::Text("Indices: %i", gameEntities[4]->GetMesh()->GetIndexCount());
		ImGui::Text("\n");


	}

	if (ImGui::CollapsingHeader("Entities")) {

		if (ImGui::CollapsingHeader("Entity 1")) 
		{
			ImGui::SliderFloat3("Position", entityInfo[0].objectPosition, -1.0f, 1.0f);
			ImGui::SliderFloat3("Rotation", entityInfo[0].objectRotation, -5.0f, 5.0f);
			ImGui::SliderFloat3("Scale", entityInfo[0].objectScale, 0.0f, 5.0f);

			gameEntities[0]->GetTransform()->SetPosition(entityInfo[0].objectPosition);
			gameEntities[0]->GetTransform()->SetRotation(entityInfo[0].objectRotation);
			gameEntities[0]->GetTransform()->SetScale(entityInfo[0].objectScale);
		}

		if (ImGui::CollapsingHeader("Entity 2")) 
		{
			ImGui::SliderFloat3("Position", entityInfo[1].objectPosition, -1.0f, 1.0f);
			ImGui::SliderFloat3("Rotation", entityInfo[1].objectRotation, -5.0f, 5.0f);
			ImGui::SliderFloat3("Scale", entityInfo[1].objectScale, 0.0f, 5.0f);

			gameEntities[1]->GetTransform()->SetPosition(entityInfo[1].objectPosition);
			gameEntities[1]->GetTransform()->SetRotation(entityInfo[1].objectRotation);
			gameEntities[1]->GetTransform()->SetScale(entityInfo[1].objectScale);
		}

		if (ImGui::CollapsingHeader("Entity 3")) 
		{
			ImGui::SliderFloat3("Position", entityInfo[2].objectPosition, -1.0f, 1.0f);
			ImGui::SliderFloat3("Rotation", entityInfo[2].objectRotation, -5.0f, 5.0f);
			ImGui::SliderFloat3("Scale", entityInfo[2].objectScale, 0.0f, 5.0f);

			gameEntities[2]->GetTransform()->SetPosition(entityInfo[2].objectPosition);
			gameEntities[2]->GetTransform()->SetRotation(entityInfo[2].objectRotation);
			gameEntities[2]->GetTransform()->SetScale(entityInfo[2].objectScale);
		}

		if (ImGui::CollapsingHeader("Entity 4"))
		{
			ImGui::SliderFloat3("Position", entityInfo[3].objectPosition, -1.0f, 1.0f);
			ImGui::SliderFloat3("Rotation", entityInfo[3].objectRotation, -5.0f, 5.0f);
			ImGui::SliderFloat3("Scale", entityInfo[3].objectScale, 0.0f, 5.0f);

			gameEntities[3]->GetTransform()->SetPosition(entityInfo[3].objectPosition);
			gameEntities[3]->GetTransform()->SetRotation(entityInfo[3].objectRotation);
			gameEntities[3]->GetTransform()->SetScale(entityInfo[3].objectScale);
		}

		
		if (ImGui::CollapsingHeader("Entity 5"))
		{
			ImGui::SliderFloat3("Position", entityInfo[4].objectPosition, -1.0f, 1.0f);
			ImGui::SliderFloat3("Rotation", entityInfo[4].objectRotation, -5.0f, 5.0f);
			ImGui::SliderFloat3("Scale", entityInfo[4].objectScale, 0.0f, 5.0f);

			gameEntities[4]->GetTransform()->SetPosition(entityInfo[4].objectPosition);
			gameEntities[4]->GetTransform()->SetRotation(entityInfo[4].objectRotation);
			gameEntities[4]->GetTransform()->SetScale(entityInfo[4].objectScale);
		}
	}


	if (demoWindowState)
	{
		ImGui::ShowDemoWindow(&demoWindowState);
	}

	//Ends the creation of a new window
	ImGui::End();

}

void::Game::ChangeColor(float* _color, float r, float g, float b, float a)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
	_color[3] = a;
}

void::Game::CopyXMFloatToArray(XMFLOAT3 xmFloat, float* floatArray)
{
	floatArray[0] = xmFloat.x;
	floatArray[1] = xmFloat.y;
	floatArray[2] = xmFloat.z;
}

void Game::CopyInfoToStruct(int index)
{
	entityInfo[index].objectPosition[0] = gameEntities[index]->GetTransform()->GetPosition().x;
	entityInfo[index].objectPosition[1] = gameEntities[index]->GetTransform()->GetPosition().y;
	entityInfo[index].objectPosition[2] = gameEntities[index]->GetTransform()->GetPosition().z;


	entityInfo[index].objectRotation[0] = gameEntities[index]->GetTransform()->GetRotation().x;
	entityInfo[index].objectPosition[1] = gameEntities[index]->GetTransform()->GetRotation().y;
	entityInfo[index].objectPosition[2] = gameEntities[index]->GetTransform()->GetRotation().z;

	entityInfo[index].objectScale[0] = gameEntities[index]->GetTransform()->GetScale().x;
	entityInfo[index].objectScale[1] = gameEntities[index]->GetTransform()->GetScale().y;
	entityInfo[index].objectScale[2] = gameEntities[index]->GetTransform()->GetScale().z;
}





