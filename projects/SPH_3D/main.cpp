#include <src/libraries/Utility/definitions.h>
#include <src/libraries/Shader/shaderProgram.h>
#include <src/libraries/Rendering/Camera.h>
#include <src/libraries/Geometry/RenderCone.h>
#include <src/libraries/Geometry/RenderCube.h>
#include <src/libraries/Geometry/RenderPlane.h>
#include <src/libraries/Geometry/RenderSphere.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <iostream>
#include <array>
#include <src/imGUI/imgui.h>
#include <src/imGUI/imgui_glfw.h>
#include <src/iconfont/IconsMaterialDesignIcons.h>
#include <src/libraries/Utility/tinyfiledialogs.h>
#include<filesystem>

#define WIDTH 1280
#define HEIGHT 720

std::unique_ptr<ImGui::ImGui> gui;


inline float ranf(float min = 0.0f, float max = 1.0f)
{
	return ((max - min) * rand() / RAND_MAX + min);
}

/*****************************************Key Callbacks*****************************************/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		gui->keyCallback(window, key, scancode, action, mods);
		return;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
}

int main() {
	/*****************************************Init GLFW Stuff*****************************************/
	if (!glfwInit())
		exit(EXIT_FAILURE);
	GLFWwindow* window;
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);*/
	/*glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(WIDTH, HEIGHT, "SPH 3D", 0, 0);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glewInit();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	double dt = 0.00001;
	glfwSetTime(0.0);
	double currentTime = glfwGetTime();
	double lastTime = glfwGetTime();
	double frameTime = lastTime;
	double accumulator = 0.0;
	double renderAccumulator = 0.0;
	double iterationCount = 0.0;

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, 0.0001f, 50.0f);

	/*****************************************Imgui Init***************************************************/
	gui = std::make_unique<ImGui::ImGui>(window, false);		//create gui object
	glfwSetKeyCallback(window, key_callback);
	ImGui::StyleColorsClassic();								//pick color style Classic/Dark/Light  or edit a completely own style have to look where this is done

	glfwSetMouseButtonCallback(
		window, [](GLFWwindow* w, const int button, const int action, const int m) {
			gui->mouseButtonCallback(w, button, action, m);
		});
	glfwSetScrollCallback(window,
		[](GLFWwindow* w, const double xoffset, const double yoffset) {
			gui->scrollCallback(w, xoffset, yoffset);
		});
	glfwSetCharCallback(window, [](GLFWwindow* w, const unsigned int c) {
		gui->charCallback(w, c);
		});

	/*****************************************Cube*****************************************/
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 colliModelMatrix = glm::mat4(1.0f);

	float cube_width = 1.25f;
	float cube_height = 1.5f;
	float cube_depth = 0.25f;

	std::unique_ptr<RenderCube> cube;

	glm::vec3 colliSpherePos = glm::vec3(0.0f);
	float colliSPhereRadius = 0.5f;
	std::unique_ptr<RenderSphere> colliSphere;
	colliSphere = std::make_unique<RenderSphere>(glm::vec4(colliSpherePos, 1.0), colliSPhereRadius, 20);

	glm::vec3 colliCubePos = glm::vec3(0.0f);
	glm::vec3 colliCubeDim = glm::vec3(0.5f);
	std::unique_ptr<RenderCube> colliCube;
	colliCube = std::make_unique<RenderCube>(colliCubeDim.x, colliCubeDim.y, colliCubeDim.z);

	glm::vec3 cube_dimensions = glm::vec3(cube_width, cube_height, cube_depth);

	GLuint m_wave_ssbo;
	GLuint m_wave_vao;


	/**************SSBOs for SPH Particles***************/
	GLuint m_position_ssbo;
	GLuint m_velocity_ssbo;
	GLuint m_forces_ssbo;
	GLuint m_densities_ssbo;
	GLuint m_pressures_ssbo;
	GLuint m_vao_sph;


	/*****************************************Load Shaders*****************************************/
	ShaderProgram cubeShader = ShaderProgram(SHADERS_PATH "/sph_3d/cube.vert", SHADERS_PATH "/sph_3d/cube.frag");
	cubeShader.updateUniform("projectionMatrix", camera.projection());
	cubeShader.updateUniform("modelMatrix", modelMatrix);

	ShaderProgram colliShader = ShaderProgram(SHADERS_PATH "/sph_3d/cube.vert", SHADERS_PATH "/sph_3d/cube.frag");
	colliShader.updateUniform("projectionMatrix", camera.projection());
	//colliShader.updateUniform("modelMatrix", colliModelMatrix);

	ShaderProgram particleShader = ShaderProgram(SHADERS_PATH "/sph_3d/particle.vert", SHADERS_PATH "/sph_3d/particle.frag");
	particleShader.updateUniform("projectionMatrix", camera.projection());
	particleShader.updateUniform("modelMatrix", modelMatrix);

	//ShaderProgram computeTestShader = ShaderProgram(SHADERS_PATH "/sph/computeTest.comp");
	ShaderProgram computeDensitiesShader = ShaderProgram(SHADERS_PATH "/sph_3d/densities_pressures.comp");
	ShaderProgram computeForcesShader = ShaderProgram(SHADERS_PATH "/sph_3d/forces.comp");
	ShaderProgram computeIntegrateShader = ShaderProgram(SHADERS_PATH "/sph_3d/integrate.comp");

	/**************************************** Uniform variables **********************************/
	int NUM_PARTICLE = 30000;
	float SPH_PARTICLE_RADIUS = 0.005f;

	float sph_restingDensity = 1000.f;
	float sph_stiffness = 2000.0f;
	float sph_viscosity = 3000.0f;
	float sph_surfaceTension = 1.10f;

	float sph_mass = 0.02f;				//volume * restingDensity
	float smoothingLength = (4 * SPH_PARTICLE_RADIUS);
	//so geht es eigentlich mit masse etc

	float sph_wallDamping = 0.3f;

	/******************************************* ImGUI Parameters ********************************/
	bool simulate = false;
	bool is_generated = false;
	bool wave_left = false;
	bool wave_right = false;
	float push_left = 0.0f;
	float push_right = 0.0f;
	bool hold = false;
	int countdown = 0;
	float wave_speed = 0.01f;
	float wave_range = cube_dimensions.x / 2;

	bool is_collision = false;
	int collisionObject = -1;
	bool colliObjectDone = false;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	float aTest = 0.0f;
	bool is_push = true;
	int count = 0;

	/*****************************************Render Loop***************************************************/
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//currentTime = glfwGetTime();
		//dt = currentTime - lastTime;
		//accumulator += dt;
		//if (currentTime - frameTime > 1.0) {
		//	updateFramerate(currentTime, frameTime, iterationCount, window);
		//	frameTime = currentTime;
		//	std::cout << iterationCount << std::endl;
		//}

		camera.update(window);


		gui->newFrame();
		ImGui::Begin("SPH");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("");

		if (ImGui::CollapsingHeader("Setup"))
		{
			ImGui::Text("SPH Variables:");
			ImGui::DragInt("Particle Count", &NUM_PARTICLE, 1, 1, 1000000);
			ImGui::DragFloat("Particle Radius", &SPH_PARTICLE_RADIUS, 0.001f, 0.001f, 1.0f);
			ImGui::Text("");
			ImGui::DragFloat("Resting Density", &sph_restingDensity, 0.1f, 0.0f, 5000.0f);
			ImGui::DragFloat("Stiffness", &sph_stiffness, 0.1f, 0.0f, 5000.0f);
			ImGui::DragFloat("Viscosity", &sph_viscosity, 0.1f, 0.0f, 5000.0f);


			ImGui::Text("");
			ImGui::Text("Boundary Cube:");
			ImGui::DragFloat3("Cube Dimensions", glm::value_ptr(cube_dimensions), 0.05f, 0.05f, 5.0f);
			cube = std::make_unique<RenderCube>(cube_dimensions.x, cube_dimensions.y, cube_dimensions.z);

			ImGui::DragFloat("Wall damping", &sph_wallDamping, 0.01f, 0.01f, 5.0f);
			ImGui::Text("");
			if (ImGui::Button("Create"))
			{

				is_generated = true;

				/*******************SPH *******************/
				std::vector<glm::vec4> position(NUM_PARTICLE);
				std::vector<glm::vec4> velocity(NUM_PARTICLE);
				std::vector<glm::vec4> color(NUM_PARTICLE);
				std::vector<glm::vec4> forces(NUM_PARTICLE);
				std::vector<float> densities(NUM_PARTICLE);
				std::vector<float> pressures(NUM_PARTICLE);

				for (int i = 0; i < NUM_PARTICLE; i++)
				{
					//position[i] = glm::vec4(ranf(-cube_width / 2, cube_width / 2), ranf(0, cube_height), ranf(-cube_depth / 2, cube_depth / 2), 1.0f);
					position[i] = glm::vec4(glm::sphericalRand(0.25f), 1.0f);

					velocity[i] = glm::vec4(0.0f);
					forces[i] = glm::vec4(0.0f);
					densities[i] = 0.0f;
					pressures[i] = 0.0f;
				}

				glCreateBuffers(1, &m_position_ssbo);
				glNamedBufferStorage(m_position_ssbo, sizeof(glm::vec4) * NUM_PARTICLE, position.data(), GL_DYNAMIC_STORAGE_BIT);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_position_ssbo);

				glCreateBuffers(1, &m_velocity_ssbo);
				glNamedBufferStorage(m_velocity_ssbo, sizeof(glm::vec4) * NUM_PARTICLE, velocity.data(), GL_DYNAMIC_STORAGE_BIT);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velocity_ssbo);

				glCreateBuffers(1, &m_forces_ssbo);
				glNamedBufferStorage(m_forces_ssbo, sizeof(glm::vec4) * NUM_PARTICLE, forces.data(), GL_DYNAMIC_STORAGE_BIT);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_forces_ssbo);

				glCreateBuffers(1, &m_densities_ssbo);
				glNamedBufferStorage(m_densities_ssbo, sizeof(float) * NUM_PARTICLE, densities.data(), GL_DYNAMIC_STORAGE_BIT);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_densities_ssbo);

				glCreateBuffers(1, &m_pressures_ssbo);
				glNamedBufferStorage(m_pressures_ssbo, sizeof(float) * NUM_PARTICLE, pressures.data(), GL_DYNAMIC_STORAGE_BIT);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_pressures_ssbo);
				/**************VAO for SPH Particles***************/
				GLuint m_vao_sph;

				glGenVertexArrays(1, &m_vao_sph);
				glBindVertexArray(m_vao_sph);

				glBindBuffer(GL_ARRAY_BUFFER, m_position_ssbo);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

				/*glBindBuffer(GL_ARRAY_BUFFER, m_densities_ssbo);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);*/

				glBindVertexArray(0);
			}
		}

		if (ImGui::CollapsingHeader("Simulation"))
		{
			if (ImGui::Button("Start"))
			{
				simulate = true;
			}

			ImGui::SameLine();					//this makes sure two Buttons are positioned in the same Line
			if (ImGui::Button("Pause"))
			{
				simulate = false;
			}

			ImGui::SameLine();					//this makes sure two Buttons are positioned in the same Line
			if (ImGui::Button("Reset"))
			{
				is_generated = false;
				simulate = false;
				push_left = 0.0f;
				wave_left = false;
			}

			ImGui::Text("");
			ImGui::Checkbox("Wave left", &wave_left);
			if (wave_left)
			{
				if (!hold)
				{
					push_left += wave_speed;


					if (push_left >= wave_range)
					{
						push_left = 0.0f;
						hold = true;
					}
				}
				else
				{
					if (!wave_right)
					{
						countdown++;

						if (countdown == 500)
						{
							hold = false;
							countdown = 0;
						}
					}
				}

			}
			ImGui::DragFloat("Wave Speed", &wave_speed, 0.001f, 0.0001f, 0.1f);
			ImGui::DragFloat("Wave Range", &wave_range, 0.01f, 0.01f, (cube_dimensions.x * 2.0f) - 0.05f);

			ImGui::Text("");
			ImGui::Checkbox("Collision", &is_collision);
			if (is_collision)
			{
				ImGui::RadioButton("Sphere ", &collisionObject, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Cube ", &collisionObject, 1);
				if (collisionObject == 0)
				{
					ImGui::DragFloat3("Collision Object Pos", glm::value_ptr(colliSpherePos), 0.01f, -3.f, 3.0f);
					ImGui::DragFloat("Object Radius", &colliSPhereRadius, 0.01f, 0.01f, 1.0f);

					glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), colliSpherePos);
					colliModelMatrix = glm::scale(transMatrix, glm::vec3(colliSPhereRadius));
				}

				if (collisionObject == 1)
				{
					ImGui::DragFloat3("Collision Object Pos", glm::value_ptr(colliCubePos), 0.01f, -3.f, 3.0f);
					ImGui::DragFloat3("Collision Object Dim", glm::value_ptr(colliCubeDim), 0.01f, 0.01f, 3.0f);

					glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), colliCubePos);
					colliModelMatrix = glm::scale(transMatrix, glm::vec3(colliCubeDim));
				}
			}

		}

		ImGui::End();

		//render GUI
		gui->render();

		/************** Compute Shader Stuff*****************/
		if (simulate && is_generated)
		{
			computeDensitiesShader.use();
			computeDensitiesShader.updateUniform("NUM_PARTICLES", NUM_PARTICLE);
			computeDensitiesShader.updateUniform("RESTING_DENSITY", sph_restingDensity);
			computeDensitiesShader.updateUniform("PARTICLE_MASS", sph_mass);
			computeDensitiesShader.updateUniform("PARTICLE_STIFFNESS", sph_stiffness);
			computeDensitiesShader.updateUniform("SMOOTHING_LENGTH", smoothingLength);
			glDispatchCompute((NUM_PARTICLE + 128 - 1) / 128, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			computeForcesShader.use();
			computeForcesShader.updateUniform("NUM_PARTICLES", NUM_PARTICLE);
			computeForcesShader.updateUniform("PARTICLE_MASS", sph_mass);
			computeForcesShader.updateUniform("PARTICLE_VISCOSITY", sph_viscosity);
			//computeForcesShader.updateUniform("PARTICLE_SURFACE_TENSION", sph_surfaceTension);
			computeForcesShader.updateUniform("SMOOTHING_LENGTH", smoothingLength);
			glDispatchCompute((NUM_PARTICLE + 128 - 1) / 128, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			computeIntegrateShader.use();
			//computeIntegrateShader.updateUniform("NUM_PARTICLES", SPH_NUM_PARTICLES);
			computeIntegrateShader.updateUniform("boundWidth", cube_dimensions.x);
			computeIntegrateShader.updateUniform("boundHeight", cube_dimensions.y);
			computeIntegrateShader.updateUniform("boundDepth", cube_dimensions.z);
			computeIntegrateShader.updateUniform("WALL_DAMPING", sph_wallDamping);
			computeIntegrateShader.updateUniform("push_left", push_left);
			computeIntegrateShader.updateUniform("modelMatrix", colliModelMatrix);
			computeIntegrateShader.updateUniform("collisionSphereRadius", colliSPhereRadius);
			computeIntegrateShader.updateUniform("collisionCubeDim", colliCubeDim);
			computeIntegrateShader.updateUniform("collisionObject", collisionObject);
			glDispatchCompute((NUM_PARTICLE + 128 - 1) / 128, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}


		/************** Visual and Render stuff ************/
		if (is_generated)
		{
			cubeShader.use();
			cubeShader.updateUniform("viewMatrix", camera.view());
			cube->renderShell();
			//wave_maker_one.render();
			if (is_collision)
			{
				colliShader.use();
				colliShader.updateUniform("viewMatrix", camera.view());
				colliShader.updateUniform("modelMatrix", colliModelMatrix);

				if (collisionObject == 0)
				{
					colliSphere->render();
				}
				if (collisionObject == 1)
				{
					colliCube->render();
				}

			}



			particleShader.use();
			particleShader.updateUniform("viewMatrix", camera.view());
			/***render Particles ***/
			glBindVertexArray(m_vao_sph);
			glDrawArrays(GL_POINTS, 0, NUM_PARTICLE);
			glBindVertexArray(0);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}