#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "../Dependencies/stb_image.h"

const unsigned int screen_width = 1920;
const unsigned int screen_height = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_width / 2;
float lastY = screen_height / 2;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void FrameBufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void MouseCallback(GLFWwindow* window, const double xPos, const double yPos)
{
	if (firstMouse)
	{
		lastX = static_cast<float>(xPos);
		lastY = static_cast<float>(yPos);
		firstMouse = false;
	}
	const auto xOffset = xPos - lastX;
	const auto yOffset = lastY - yPos;
	lastX = static_cast<float>(xPos);
	lastY = static_cast<float>(yPos);

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int LoadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = {};
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR::TEXTURES::FAILED TO LOAD AT PATH: " << path << std::endl;
	}

	stbi_image_free(data);
	return textureID;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] =
	{
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	unsigned int vbo, cubeVAO; //Vertex Buffer Object, Vertex Array Object, Element Buffer Object
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &cubeVAO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// Normal vector attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// generate lampVAO
	unsigned int lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	//---*Load and Create a texture*---
	const unsigned int diffuseMap = LoadTexture("Resources/container2.png");
	const unsigned int specularMap = LoadTexture("Resources/container2_specular.png");


	// draw in wireFrame mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	const Shader lightingShader("Source/Shaders/vertexShader.vert", "Source/Shaders/fragmentShader.frag");
	Shader lampShader("Source/Shaders/CubeLamp.vert", "Source/Shaders/CubeLamp.frag");

	lightingShader.Use();
	lightingShader.SetInt("material.diffuse", 0);
	lightingShader.SetInt("material.specular", 1);

	// set light properties
// directional light
	lightingShader.SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	lightingShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	lightingShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	lightingShader.SetVec3("pointLights[0].position", pointLightPositions[0]);
	lightingShader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.SetVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.SetFloat("pointLights[0].constant", 1.0f);
	lightingShader.SetFloat("pointLights[0].linear", 0.09);
	lightingShader.SetFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader.SetVec3("pointLights[1].position", pointLightPositions[1]);
	lightingShader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.SetVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.SetFloat("pointLights[1].constant", 1.0f);
	lightingShader.SetFloat("pointLights[1].linear", 0.09);
	lightingShader.SetFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader.SetVec3("pointLights[2].position", pointLightPositions[2]);
	lightingShader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.SetVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.SetFloat("pointLights[2].constant", 1.0f);
	lightingShader.SetFloat("pointLights[2].linear", 0.09);
	lightingShader.SetFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader.SetVec3("pointLights[3].position", pointLightPositions[3]);
	lightingShader.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.SetVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.SetFloat("pointLights[3].constant", 1.0f);
	lightingShader.SetFloat("pointLights[3].linear", 0.09);
	lightingShader.SetFloat("pointLights[3].quadratic", 0.032);
	
	

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// timings
		const auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		ProcessInput(window);

		// render commands
		glClearColor(0.43f, 0.44f, 0.47f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.Use();
		lightingShader.SetVec3("viewPos", camera.Position);

		lightingShader.SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.SetFloat("material.shininess", 32.0f);

		const glm::mat4 view = camera.GetViewMatrix();
		lightingShader.SetMat4("view", view);

		const glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen_width) / static_cast<float>(screen_height), 0.1f, 100.0f);
		lightingShader.SetMat4("projection", projection);

		auto model = glm::mat4(1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(cubeVAO);

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			const float angle = 20.0f * static_cast<float>(i);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.SetMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lampShader.Use();
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", projection);
		glBindVertexArray(lampVAO);
		for (auto pointLightPosition : pointLightPositions)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPosition);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		// check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
	return 0;
}