#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void processKeyboardInput();
unsigned int initializeskyBox();
//void drawskybox(Shader skybox, unsigned int cubemapTexture, unsigned int skyboxVAO);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window window("Game Engine", 1200, 800);
Camera camera(glm::vec3(21.2f, -26.0f, 3.5f));
//Camera camera(glm::vec3(0.0f, -26.0f, 0.0f));

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(1080.0f, 1000.0f, -1000.0f);

int main()
{
	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader skyboxshader("Shaders/skyboxvert.glsl", "Shaders/skyboxfrag.glsl");

	//Textures
	GLuint tex = loadBMP("Resources/Textures/sand.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/darksteel.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/grey.bmp");
	GLuint tex5 = loadBMP("Resources/Textures/stone.bmp");
	GLuint tex6 = loadBMP("Resources/Textures/des.bmp");

	std::string facesCubemap[6] =
	{
		"Resources/Textures/skyboxtest2/ny.bmp",
		"Resources/Textures/skyboxtest2/py.bmp",
		"Resources/Textures/skyboxtest2/nx.bmp",
		"Resources/Textures/skyboxtest2/px.bmp",
		"Resources/Textures/skyboxtest2/pz.bmp",
		"Resources/Textures/skyboxtest2/nz.bmp"
	};

	GLuint sbtex = loadSB(facesCubemap);

	glEnable(GL_DEPTH_TEST);

	std::vector<Texture> sand;
	sand.push_back(Texture());
	sand[0].id = tex;
	sand[0].type = "texture_diffuse";

	std::vector<Texture> steel;
	steel.push_back(Texture());
	steel[0].id = tex2;
	steel[0].type = "texture_diffuse";

	std::vector<Texture> orange;
	orange.push_back(Texture());
	orange[0].id = tex3;
	orange[0].type = "texture_diffuse";

	std::vector<Texture> grey;
	grey.push_back(Texture());
	grey[0].id = tex4;
	grey[0].type = "texture_diffuse";

	std::vector<Texture> des;
	des.push_back(Texture());
	des[0].id = tex6;
	des[0].type = "texture_diffuse";

	std::vector<Texture> stone;
	stone.push_back(Texture());
	stone[0].id = tex5;
	stone[0].type = "texture_diffuse";

	std::vector<Texture> skyboxtexture;
	skyboxtexture.push_back(Texture());
	skyboxtexture[0].id = sbtex;
	skyboxtexture[0].type = "texture_diffuse";

	//skybox initialize
	unsigned int skyboxVAO = initializeskyBox();

	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh crater = loader.loadObj("Resources/Models/crater.obj", sand);
	Mesh ship = loader.loadObj("Resources/Models/ship.obj", stone);
	Mesh brokenShip = loader.loadObj("Resources/Models/brokenShip.obj", grey);
	Mesh engine = loader.loadObj("Resources/Models/engine.obj", grey);
	Mesh tailwing = loader.loadObj("Resources/Models/tailwing.obj", grey);
	Mesh gun = loader.loadObj("Resources/Models/gun.obj", steel);
	Mesh key = loader.loadObj("Resources/Models/superKey.obj", orange);
	//Mesh motor = loader.loadObj("Resources/Models/oilBottle.obj");
	Mesh stones[2];
	for (int i = 1; i <= 2; i++) {
		std::string stringulescu = "Resources/Models/stone" + std::to_string(i) + ".obj";
		stones[i - 1] = loader.loadObj(stringulescu, des);
	}
	//Mesh moon = loader.loadObj("Resources/Models/moon.obj");
	Mesh moons[10];
	for (int i = 0; i < 10; i++) {
		std::string moonString = "Resources/Models/sphere.obj";
		moons[i] = loader.loadObj(moonString, grey);
	}


	//imgui
#if defined(IMGUI_IMPL_OPENGL_ES2)
// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();
	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("Resources/Fonts/times.ttf", 25);

	//animation
	float x[10], y[10], z[10], j[10], position[10];
	std::fill_n(x, 10, 10000);
	std::fill_n(y, 10, 10000);
	std::fill_n(z, 10, 10000);
	std::fill_n(j, 10, 10000);
	std::fill_n(position, 10, 10000);
	srand(time(0));
	static int asteroidDensity = 0;
	int taskFlag = 0;
	int moveFlag = 0;

	bool gunFlag = false;
	bool stonesFlag[7] = { true, true, true, true, true, true, true };
	bool engineFlag[3] = { true, true, true };
	bool tailFlag = true;
	bool keyFlag = true;
	bool shipFlag = false;
	float maxDistance = 49.0f;

	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) && glfwWindowShouldClose(window.getWindow()) == 0)
	{
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput();
		camera.outOfBounds(22.0f);
		std::cout << "Camera view direction: " << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << "\n";

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//task1
		if (window.isPressed(GLFW_KEY_A)) moveFlag++;
		if (window.isPressed(GLFW_KEY_D)) moveFlag++;
		if (window.isPressed(GLFW_KEY_S)) moveFlag++;
		if (window.isPressed(GLFW_KEY_W)) moveFlag++;
		if (moveFlag == 400) taskFlag = 2;
		//task2
		if (taskFlag == 2 && gunFlag == true) taskFlag = 3;
		//task3
		if (taskFlag == 3 && stonesFlag[6] == false) taskFlag = 4;
		//task4
		if (taskFlag == 4 && camera.getCameraPosition().x >= 10.0f && camera.getCameraPosition().x <= 13.0f && camera.getCameraPosition().z >= -6.0f && camera.getCameraPosition().z <= 10.0f) taskFlag = 5;
		//task5
		if (taskFlag == 5 && stonesFlag[0] == false && stonesFlag[1] == false && stonesFlag[2] == false && stonesFlag[3] == false && stonesFlag[4] == false && stonesFlag[5] == false) taskFlag = 6;
		//task6
		if (taskFlag == 6 && tailFlag == false && engineFlag[0] == false && engineFlag[1] == false && engineFlag[2] == false) taskFlag = 7;
		//task7
		if (taskFlag == 7 && keyFlag == false) taskFlag = 8;
		//task8
		if (taskFlag == 8 && camera.getCameraPosition().x >= 18.0f && camera.getCameraPosition().x <= 21.0f && camera.getCameraPosition().z >= -6.0f && camera.getCameraPosition().z <= 4.0f) taskFlag = 9;
		
			
			//sun
		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		//Test for one Obj loading = light source

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sunShader);


		shader.use();


		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");


		//crater
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		crater.draw(shader);


		// ship
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(20.0f, -27.0f, -10.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 5.0f, glm::vec3(1.0, 0.0, 0.0));
		ModelMatrix = glm::rotate(ModelMatrix, 15.0f, glm::vec3(0.0, 0.0, 1.0));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		if (shipFlag == false) brokenShip.draw(shader);
		else ship.draw(shader);
		

		// engine
		glm::vec3 initEnginePosition[3] = {
			glm::vec3(5.0f, -29.0f, -15.0f),
			glm::vec3(-22.0f, -27.2f, -15.0f),
			glm::vec3(-12.0f, -31.0f, 3.0f)
		};
		for (int i = 0; i < 3; i++) {
			ModelMatrix = glm::mat4(1.0);
			float distancePE = pow(camera.getCameraPosition().x - initEnginePosition[i].x, 2) + pow(camera.getCameraPosition().z - initEnginePosition[i].z, 2);
			if (window.isPressed(GLFW_KEY_E) && distancePE < maxDistance) engineFlag[i] = false;
			switch (i)
			{
			case 0:
				if (engineFlag[i] == true) {
					ModelMatrix = glm::translate(ModelMatrix, initEnginePosition[i]);
					ModelMatrix = glm::rotate(ModelMatrix, 105.0f, glm::vec3(1.0, 0.0, 0.0));
				}
				else {
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(11.0f, -29.0f, -17.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 90.0f, glm::vec3(1.0, 0.0, 0.0));
				}
				break;
			case 1:
				if (engineFlag[i] == true) {
					ModelMatrix = glm::translate(ModelMatrix, initEnginePosition[i]);
					ModelMatrix = glm::rotate(ModelMatrix, 169.0f, glm::vec3(1.0, 0.0, 0.0));
					ModelMatrix = glm::rotate(ModelMatrix, -40.0f, glm::vec3(0.0, 1.0, 0.0));
				}
				else {
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(8.0f, -29.0f, -20.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 90.0f, glm::vec3(1.0, 0.0, 0.0));
				}
				break;
			case 2:
				if (engineFlag[i] == true) {
					ModelMatrix = glm::translate(ModelMatrix, initEnginePosition[i]);
					ModelMatrix = glm::rotate(ModelMatrix, -40.0f, glm::vec3(1.0, 0.0, 0.0));
				}
				else {
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(13.0f, -29.0f, -16.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 90.0f, glm::vec3(1.0, 0.0, 0.0));
				}
				break;
			default:
				break;
			}
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			if (shipFlag == false) engine.draw(shader);
		}


		// tailwing
		ModelMatrix = glm::mat4(1.0);
		float distancePT = pow(camera.getCameraPosition().x, 2) + pow(camera.getCameraPosition().z - 17.0f, 2);
		if (window.isPressed(GLFW_KEY_E) && distancePT < maxDistance) tailFlag = false;
		if (tailFlag == true) {
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -30.8f, 17.0f));
			ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(0.0, 0.0, 1.0));
			ModelMatrix = glm::rotate(ModelMatrix, 10.0f, glm::vec3(0.0, 1.0, 0.0));
			ModelMatrix = glm::rotate(ModelMatrix, 35.0f, glm::vec3(1.0, 0.0, 0.0));
		}
		else {
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(13.0f, -29.0f, -20.0f));
			ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(0.0, 0.0, 1.0));
			ModelMatrix = glm::rotate(ModelMatrix, 35.0f, glm::vec3(0.0, 1.0, 0.0));
			ModelMatrix = glm::rotate(ModelMatrix, 45.0f, glm::vec3(1.0, 0.0, 0.0));
		}
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		if (shipFlag == false) tailwing.draw(shader);

		if (window.isPressed(GLFW_KEY_R) && tailFlag == false && engineFlag[0] == false && engineFlag[1] == false && engineFlag[2] == false) { keyFlag = false; shipFlag = true; }


		// key
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -30.4f, 2.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		if (keyFlag == true) key.draw(shader);



		// gun
		ModelMatrix = glm::mat4(1.0);
		glm::vec3 gunPos = glm::vec3(20.0f, -28.7f, 8.0f);
		if (gunFlag == false) {
			ModelMatrix = glm::translate(ModelMatrix, gunPos);
			ModelMatrix = glm::rotate(ModelMatrix, 90.0f, glm::vec3(0.0, 0.0, 1.0));
			ModelMatrix = glm::rotate(ModelMatrix, 90.0f, glm::vec3(1.0, 0.0, 0.0));
		}
		else {
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(camera.getCameraPosition().x + 0.1f, camera.getCameraPosition().y - 0.5f, camera.getCameraPosition().z));
			ModelMatrix = glm::rotate(ModelMatrix, atan(camera.getCameraViewDirection().x / camera.getCameraViewDirection().z) * 180 / 3.14f, glm::vec3(0.0, 1.0, 0.0));
			if (camera.getCameraViewDirection().z < 0.0f) ModelMatrix = glm::rotate(ModelMatrix, 180.0f, glm::vec3(0.0, 1.0, 0.0));
		}
		if (window.isPressed(GLFW_KEY_X) && pow((camera.getCameraPosition().x - gunPos.x), 2) + pow((camera.getCameraPosition().z - gunPos.z), 2) <= 16) {
			gunFlag = true;
		}
		
		//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		gun.draw(shader);

		float m1 = camera.getCameraViewDirection().z / camera.getCameraViewDirection().x;


		//stones
		glm::vec3 stonePosition[7] = {
			glm::vec3(0.0f, -30.5f, 0.0f),//0
			glm::vec3(10.0f, -31.0f, 10.0f),//1 stanga iesire
			glm::vec3(-15.0f, -30.6f, 12.0f),//2
			glm::vec3(-21.0f, -31.0f, 16.0f),//3
			glm::vec3(-17.0f, -31.0f, -8.0f),//4
			glm::vec3(-26.0f, -30.2f, -12.0f),//5 nu dispare
			glm::vec3(11.0f, -30.0f, 6.0f)//6 fata usii
		};
		for (int i = 0; i < 7; i++) {
			float m2 = (camera.getCameraPosition().z - stonePosition[i].z) / (camera.getCameraPosition().x - stonePosition[i].x);
			float distancePS = pow(camera.getCameraPosition().x - stonePosition[i].x, 2) + pow(camera.getCameraPosition().z - stonePosition[i].z, 2);
			// std::cout << atan(m1) * 180 / 3.14 - atan(m2) * 180 / 3.14 << "\n";
			if (window.isPressed(GLFW_KEY_Q) && distancePS < maxDistance && atan(m2) - atan(m1) < 0.4f) stonesFlag[i] = false;

			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, stonePosition[i]);
			if (i == 6) ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			if (stonesFlag[i] == true) stones[i % 2].draw(shader);
		}


		//moon
		for (int i = 0; i < asteroidDensity; i++)
		{
			if (x[i] + position[i] > 1050 || x[i] + position[i] < -1050 || y[i] + position[i] >1050 || y[i] + position[i] < -1050 || z[i] + position[i] >1050 || z[i] + position[i] < -1050)
			{
				x[i] = rand() % 4000 - 2000;
				y[i] = rand() % 2000 + 0;
				z[i] = rand() % 4000 - 2000;
				position[i] = 0;
				j[i] = rand() % 2 - 0.5;
			}

			position[i] = position[i] + j[i];
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(x[i] + position[i], y[i] + position[i], z[i] + position[i]));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			moons[i].draw(shader);
		}


		//skybox
		glDepthFunc(GL_LEQUAL);

		skyboxshader.use();

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = glm::mat4(glm::mat3(glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp())));
		projection = glm::perspective(90.0f, (float)window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sbtex);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);


		//imgui
		{
			ImGui::Begin("Lost in Space");
			ImGui::PushFont(font1);
	
			if (taskFlag == 0)
			{
				ImGui::Text("Story:");
				ImGui::TextWrapped("While travelling alone back to Earth, your ship engine malfunctioned and crashed on a deserted planet. To leave the planet, you need to survive and to find necessary parts to repair the engine. Follow the steps in order to get home.");
				if (ImGui::Button("Start the game")) taskFlag = 1;
			}

			if (taskFlag == 1)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("1. Move character (W,A,S,D).");
			}

			if (taskFlag == 2)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("2. Take the gun(X).");
			}

			if (taskFlag == 3)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("3. Destroy stone in front of the door(Q).");
			}

			if (taskFlag == 4)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("4. Get out of spaceship.");
			}

			if (taskFlag == 5)
			{
				ImGui::Text("Tasks:");	//
				ImGui::TextWrapped("5. Destroy stones to have space to move around(Q).");
			}

			if (taskFlag == 6)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("6. Find the parts that felt of the ship(E).");
			}

			if (taskFlag == 7)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("7. Get the key to repair the ship(R).");
			}

			if (taskFlag == 8)
			{
				ImGui::Text("Tasks:");
				ImGui::TextWrapped("8. Get ready to leave the planet(get in the ship).");
			}

			if (taskFlag == 9)
			{
				ImGui::TextWrapped("Congratulation! You are on your way home.");
				ImGui::TextWrapped("Game Over.");
			}

			//ImGui::SliderInt("Flag Density", &taskFlag, 1, 9);
			if (taskFlag != 0)
			{
				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::SliderInt("Asteroid Density", &asteroidDensity, 0, 10);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::PopFont();
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.update();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void processKeyboardInput()
{
	float cameraSpeed = 30 * deltaTime;

	//translation
	//pow(camera.getCameraPosition().x, 2) + pow(camera.getCameraPosition().z, 2) < pow(22, 2)
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboardMoveFront(cameraSpeed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboardMoveBack(cameraSpeed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboardMoveLeft(cameraSpeed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboardMoveRight(cameraSpeed);
	/*
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboardMoveUp(cameraSpeed);
	if (window.isPressed(GLFW_KEY_F))
		camera.keyboardMoveDown(cameraSpeed);
	*/

	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(3 * cameraSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-3 * cameraSpeed);
	/*
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotateOx(cameraSpeed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotateOx(-cameraSpeed);
	*/
}

unsigned int initializeskyBox()
{
	float skyboxVertices[] =
	{
		//   Coordinates
		-10000.0f, -10000.0f,  10000.0f,//        7--------6
		 10000.0f, -10000.0f,  10000.0f,//       /|       /|
		 10000.0f, -10000.0f, -10000.0f,//      4--------5 |
		-10000.0f, -10000.0f, -10000.0f,//      | |      | |
		-10000.0f,  10000.0f,  10000.0f,//      | 3------|-2
		 10000.0f,  10000.0f,  10000.0f,//      |/       |/
		 10000.0f,  10000.0f, -10000.0f,//      0--------1
		-10000.0f,  10000.0f, -10000.0f
	};

	unsigned int skyboxIndices[] =
	{
		1, 2, 6,	// Right
		6, 5, 1,

		0, 4, 7,	// Left
		7, 3, 0,

		4, 5, 6,	// Top
		6, 7, 4,

		0, 3, 2,	// Bottom
		2, 1, 0,

		0, 1, 5,	// Back
		5, 4, 0,

		3, 7, 6,	// Front
		6, 2, 3
	};


	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return skyboxVAO;
}
