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
	}

	vs = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	ps =  std::make_shared <SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	DirectX::XMFLOAT4 colorTint(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 colorTint2(.5f, .5f, .5f, 1.0f);
	DirectX::XMFLOAT4 colorTint3(.2f, .2f, .2f, 1.0f);


	materials.push_back(std::make_shared<Material>(vs, ps, colorTint));
	materials.push_back(std::make_shared<Material>(vs, ps, colorTint2));
	materials.push_back(std::make_shared<Material>(vs, ps, colorTint3));



	gameEntities.push_back(std::make_shared<GameEntity>(Mesh(triangleVerticies, 3, triangleIndicies, 3), *materials[0]));
	gameEntities.push_back(std::make_shared<GameEntity>(Mesh(squareVerticies, 4, squareIndicies, 6), *materials[0]));
	gameEntities.push_back(std::make_shared<GameEntity>(Mesh(objectVerticies, 6, objectIndicies, 9), *materials[1]));
	gameEntities.push_back(std::make_shared<GameEntity>(Mesh(hexagonVerticies, 6, hexagonIndicies, 12), *materials[1]));
	gameEntities.push_back(std::make_shared<GameEntity>(Mesh(pentagonVerticies, 5, pentagonIndicies, 10), *materials[2]));



	cameraList.push_back(
		std::make_shared<Camera>
		(
			XMFLOAT3(0, 0, -5),
			5.0f,
			2.0f,
			XM_PIDIV4,
			Window::AspectRatio()
			)
	);
	
	cameraList.push_back
	(
		std::make_shared<Camera>
		(
			XMFLOAT3(0, 0, -1),
			10.0f,
			1.0f,
			XM_PIDIV4/2,
			Window::AspectRatio())
	);

	activeCamera = cameraList[0];

	//Initialize Window Color and color tint
	ChangeColor(color, 0.4f, 0.6f, 0.75f, 0.0f);
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
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	if (activeCamera == nullptr)
	{
		return;
	}
	for (const auto& obj : cameraList)
	{
		obj->UpdateProjectionMatrix(Window::AspectRatio());
	}
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
	/*for (int i = 0; i < 5; i++)
	{
		gameEntities[i]->GetTransform()->SetPosition(sinf(totalTime), 0, 0);
		CopyInfoToStruct(i);
	}*/

	activeCamera->Update(deltaTime);
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
		obj->Draw(activeCamera);
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

	if (ImGui::CollapsingHeader("Entities")) 
	{

		for (int i = 0; i < 5; i++) 
		{
			if (ImGui::CollapsingHeader(("Entity " + std::to_string(i + 1)).c_str())) 
			{
				//Local Variables
				XMFLOAT3 pos = gameEntities[i]->GetTransform()->GetPosition();
				XMFLOAT3 rotation = gameEntities[i]->GetTransform()->GetRotation();
				XMFLOAT3 scale = gameEntities[i]->GetTransform()->GetScale();


				ImGui::SliderFloat3("Position", &pos.x, -1.0f, 1.0f);
				ImGui::SliderFloat3("Rotation", &rotation.x, -5.0f, 5.0f);
				ImGui::SliderFloat3("Scale", &scale.x, 0.0f, 5.0f);

				gameEntities[i]->GetTransform()->SetPosition(pos);
				gameEntities[i]->GetTransform()->SetRotation(rotation);
				gameEntities[i]->GetTransform()->SetScale(scale);
			}
		}

	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		for (int i = 0; i < 2; i++)
		{
			if (ImGui::Button(("Camera " + std::to_string(i + 1)).c_str()))
			{
				activeCamera = cameraList[i];
			}
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




