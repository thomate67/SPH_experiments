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

#define WIDTH 1000
#define HEIGHT 1000

std::unique_ptr<ImGui::ImGui> gui;

// constants
#define SPH_NUM_PARTICLES 20000

#define SPH_PARTICLE_RADIUS 0.005f

#define SPH_WORK_GROUP_SIZE 128
// work group count is the ceiling of particle count divided by work group size
#define SPH_NUM_WORK_GROUPS ((SPH_NUM_PARTICLES + SPH_WORK_GROUP_SIZE - 1) / SPH_WORK_GROUP_SIZE)


inline float ranf(float min = 0.0f, float max = 1.0f)
{
	return ((max - min) * rand() / RAND_MAX + min);
}

/*****************************************Key Callbacks*****************************************/
static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
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
	window = glfwCreateWindow(WIDTH, HEIGHT, "SPH 2D", 0, 0);

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

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 60.0f, 0.0001f, 50.0f);

	/*****************************************Imgui Init***************************************************/
	gui = std::make_unique<ImGui::ImGui>(window, false);		//create gui object
	glfwSetKeyCallback(window, key_callback);
	ImGui::StyleColorsClassic();								//pick color style Classic/Dark/Light  or edit a completely own style have to look where this is done

	glfwSetMouseButtonCallback(
		window, [](GLFWwindow * w, const int button, const int action, const int m) {
		gui->mouseButtonCallback(w, button, action, m);
	});
	glfwSetScrollCallback(window,
		[](GLFWwindow * w, const double xoffset, const double yoffset) {
		gui->scrollCallback(w, xoffset, yoffset);
	});
	glfwSetCharCallback(window, [](GLFWwindow * w, const unsigned int c) {
		gui->charCallback(w, c);
	});

	/*****************************************Cube*****************************************/
	//RenderCube cube = RenderCube(cube_width, cube_height, cube_depth);

	//glm::mat4 modelMatrix = glm::mat4(1.0f);

	/*****************************************SPH Particles*****************************************/

	std::vector<glm::vec2> position(SPH_NUM_PARTICLES);
	std::vector<glm::vec2> velocity(SPH_NUM_PARTICLES);
	std::vector<glm::vec2> color(SPH_NUM_PARTICLES);
	std::vector<glm::vec2> forces(SPH_NUM_PARTICLES);
	std::vector<float> densities(SPH_NUM_PARTICLES);
	std::vector<float> pressures(SPH_NUM_PARTICLES);

	/*for (int i = 0, x = 0, y = 0; i < SPH_NUM_PARTICLES; i++)
	{
		position[i].x = -0.625f + SPH_PARTICLE_RADIUS * 2 * x;
		position[i].y = 1 - SPH_PARTICLE_RADIUS * 2 * y;

		x++;
		if (x >= 125)
		{
			x = 0;
			y++;
		}

		velocity[i] = glm::vec2(0.0f);
		forces[i] = glm::vec2(0.0f);
		densities[i] = 0.0f;
		pressures[i] = 0.0f;
	}*/
	
	for (int i = 0, x = 0, y = 0; i < SPH_NUM_PARTICLES; i++)
	{
		position[i].x = -1 + SPH_PARTICLE_RADIUS * 2 * x;
		position[i].y = 1 - SPH_PARTICLE_RADIUS * 2 * y;

		x++;
		if (x >= 125)
		{
			x = 0;
			y++;
		}

		velocity[i] = glm::vec2(0.0f);
		forces[i] = glm::vec2(0.0f);
		densities[i] = 0.0f;
		pressures[i] = 0.0f;
	}

	/**************SSBOs for SPH Particles***************/
	GLuint m_position_ssbo;
	GLuint m_velocity_ssbo;
	GLuint m_forces_ssbo;
	GLuint m_densities_ssbo;
	GLuint m_pressures_ssbo;

	glCreateBuffers(1, &m_position_ssbo);
	glNamedBufferStorage(m_position_ssbo, sizeof(glm::vec2) * SPH_NUM_PARTICLES, position.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_position_ssbo);

	glCreateBuffers(1, &m_velocity_ssbo);
	glNamedBufferStorage(m_velocity_ssbo, sizeof(glm::vec2) * SPH_NUM_PARTICLES, velocity.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velocity_ssbo);

	glCreateBuffers(1, &m_forces_ssbo);
	glNamedBufferStorage(m_forces_ssbo, sizeof(glm::vec2) * SPH_NUM_PARTICLES, forces.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_forces_ssbo);

	glCreateBuffers(1, &m_densities_ssbo);
	glNamedBufferStorage(m_densities_ssbo, sizeof(float) * SPH_NUM_PARTICLES, densities.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_densities_ssbo);

	glCreateBuffers(1, &m_pressures_ssbo);
	glNamedBufferStorage(m_pressures_ssbo, sizeof(float) * SPH_NUM_PARTICLES, pressures.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_pressures_ssbo);
	/**************VAO for SPH Particles***************/
	GLuint m_vao_sph;

	glGenVertexArrays(1, &m_vao_sph);
	glBindVertexArray(m_vao_sph);

	glBindBuffer(GL_ARRAY_BUFFER, m_position_ssbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/*glBindBuffer(GL_ARRAY_BUFFER, m_densities_ssbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);*/

	glBindVertexArray(0);

	/*****************************************Load Shaders*****************************************/
	//ShaderProgram cubeShader = ShaderProgram(SHADERS_PATH "/sph/cube.vert", SHADERS_PATH "/sph/cube.frag");
	//cubeShader.updateUniform("projectionMatrix", camera.projection());
	//cubeShader.updateUniform("modelMatrix", modelMatrix);

	ShaderProgram particleShader = ShaderProgram(SHADERS_PATH "/sph_2d/particle.vert", SHADERS_PATH "/sph_2d/particle.frag");
	//particleShader.updateUniform("projectionMatrix", camera.projection());

	//ShaderProgram computeTestShader = ShaderProgram(SHADERS_PATH "/sph/computeTest.comp");
	ShaderProgram computeDensitiesShader = ShaderProgram(SHADERS_PATH "/sph_2d/densities_pressures.comp");
	ShaderProgram computeForcesShader = ShaderProgram(SHADERS_PATH "/sph_2d/forces.comp");
	ShaderProgram computeIntegrateShader = ShaderProgram(SHADERS_PATH "/sph_2d/integrate.comp");

	/**************************************** Uniform variables **********************************/
	float cube_width = 1.f;
	float cube_height = 1.f;
	float cube_depth = 1.f;

	glm::vec3 cube_dimensions = glm::vec3(cube_width, cube_height, cube_depth);

	float sph_restingDensity = 1000.f;
	float sph_stiffness = 2000.0f;
	float sph_viscosity = 3000.0f;
	float sph_surfaceTension = 1.10f;

	float sph_mass = 0.02f;				//volume * restingDensity
	float smoothingLength = (4 * SPH_PARTICLE_RADIUS);
	//so geht es eigentlich mit masse etc

	float sph_wallDamping = 0.3f;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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

		/************** Compute Shader Stuff*****************/

		computeDensitiesShader.use();
		computeDensitiesShader.updateUniform("NUM_PARTICLES", SPH_NUM_PARTICLES);
		computeDensitiesShader.updateUniform("RESTING_DENSITY", sph_restingDensity);
		computeDensitiesShader.updateUniform("PARTICLE_MASS", sph_mass);
		computeDensitiesShader.updateUniform("PARTICLE_STIFFNESS", sph_stiffness);
		computeDensitiesShader.updateUniform("SMOOTHING_LENGTH", smoothingLength);
		glDispatchCompute(SPH_NUM_WORK_GROUPS, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		computeForcesShader.use();
		computeForcesShader.updateUniform("NUM_PARTICLES", SPH_NUM_PARTICLES);
		computeForcesShader.updateUniform("PARTICLE_MASS", sph_mass);
		computeForcesShader.updateUniform("PARTICLE_VISCOSITY", sph_viscosity);
		//computeForcesShader.updateUniform("PARTICLE_SURFACE_TENSION", sph_surfaceTension);
		computeForcesShader.updateUniform("SMOOTHING_LENGTH", smoothingLength);
		glDispatchCompute(SPH_NUM_WORK_GROUPS, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		computeIntegrateShader.use();
		//computeIntegrateShader.updateUniform("NUM_PARTICLES", SPH_NUM_PARTICLES);
		computeIntegrateShader.updateUniform("boundWidth", cube_width);
		computeIntegrateShader.updateUniform("boundHeight", cube_height);
		//computeIntegrateShader.updateUniform("boundDepth", cube_width);
		computeIntegrateShader.updateUniform("WALL_DAMPING", sph_wallDamping);
		glDispatchCompute(SPH_NUM_WORK_GROUPS, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		/************** Visual and Render stuff ************/
		//cubeShader.use();
		//cubeShader.updateUniform("viewMatrix", camera.view());
		//cube.renderShell();

		particleShader.use();
		//particleShader.updateUniform("viewMatrix", camera.view());
		/***render Particles ***/
		glBindVertexArray(m_vao_sph);
		glDrawArrays(GL_POINTS, 0, SPH_NUM_PARTICLES);
		glBindVertexArray(0);

		gui->newFrame();					
		ImGui::Begin("Demo window");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("");
		if (ImGui::Button("Change!"))			//Buttons are clickable objects that run the code between {} on click
		{
			//cubeColor = glm::vec3(0.93f, 0.0f, 0.93f);
			//cubeShader.updateUniform("cubeColor", cubeColor);
		}
		ImGui::SameLine();					//this makes sure two Buttons are positioned in the same Line
		if (ImGui::Button("Back!"))
		{
			//cubeColor = glm::vec3(0.695f, 0.133f, 0.133f);
			//cubeShader.updateUniform("cubeColor", cubeColor);
		}
		
		if (ImGui::DragFloat("Viscosity", &sph_viscosity, 0.1f, 0.0f, 5000.0f));
		if (ImGui::DragFloat("Stiffness", &sph_stiffness, 0.1f, 0.0f, 5000.0f));
		ImGui::End();

		//render GUI
		gui->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}