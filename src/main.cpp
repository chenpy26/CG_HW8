#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>
using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float x, y;
int checkPoints = 0;
vector<glm::vec2> points;
float vertices[100];
float curve[10000];
int ccount = 0;
float dynamicVertices[10000];
float check = 0;

//处理输入
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
//响应屏幕尺寸
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//响应鼠标
void position_callback(GLFWwindow* window, double xpos, double ypos)
{
	int half_height = SCR_HEIGHT / 2;
	int half_width = SCR_WIDTH / 2;
	x = xpos / half_width - 1;
	y = 1 - ypos / half_height;
}

void click_callback(GLFWwindow* window, int button, int action, int mods)
{
	//鼠标左键点击
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
	{
		glm::vec2 p = glm::vec2(x, y);
		points.push_back(p);
	}
	//鼠标右键点击
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		vector<glm::vec2>::iterator iter;
		int i = 0;
		for (iter = points.begin(), i = 0; iter != points.end(); ++iter, ++i) {
			if (i == points.size() - 1)
				points.erase(iter);
		}
	}
}

void dynamic(vector<glm::vec2> v) {
	if (v.size() == 1) return;
	vector<glm::vec2> next = vector<glm::vec2>();
	for (int i = 0; i < v.size() - 1; i++) {
		dynamicVertices[i * 2] = v[i].x + check * (v[i + 1].x - v[i].x);
		dynamicVertices[i * 2 + 1] = v[i].y + check * (v[i + 1].y - v[i].y);
		next.push_back(glm::vec2(dynamicVertices[i * 2], dynamicVertices[i * 2 + 1]));
	}
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, next.size() * 2 * sizeof(float), dynamicVertices, GL_STATIC_DRAW);

	//绑定位置坐标
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	int s = next.size();
	glDrawArrays(GL_POINTS, 0, s);
	glDrawArrays(GL_LINE_STRIP, 0, s);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	dynamic(next);
}

//计算阶乘
int Factorial(int n) {
	int f = 1;
	for (int i = 1; i <= n; ++i) {
		f = f * i;
	}
	return f;
}

int main() {
	//glfw初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Transformation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, position_callback);
	glfwSetMouseButtonCallback(window, click_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//存储点
	points = vector<glm::vec2>();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		//输入
		processInput(window);

		//渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ccount = 0;

		//生成点
		for (int i = 0; i < points.size(); ++i) {
			vertices[i * 2] = points[i].x;
			vertices[i * 2 + 1] = points[i].y;
		}
		//生成曲线
		float t, curveX = 0, curveY = 0;
		int n = points.size() - 1;
		for (t = 0; t < 1; t += 0.001) {
			for (int i = 0; i <= n; i++) {
				float b = Factorial(n) / (Factorial(i) * Factorial(n - i))
					* pow(t, i) * pow(1 - t, n - i);
				curveX = curveX + b * points[i].x;
				curveY = curveY + b * points[i].y;
			}
			curve[ccount * 2] = curveX;
			curve[ccount * 2 + 1] = curveY;
			curveX = 0;
			curveY = 0;
			ccount++;
		}

		//直线VBO和VAO
		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

		//绑定位置坐标
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		int s = points.size();
		glDrawArrays(GL_POINTS, 0, s);
		glDrawArrays(GL_LINE_STRIP, 0, s);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		//曲线VAO，VBO
		unsigned int curveVBO, curveVAO;
		glGenVertexArrays(1, &curveVAO);
		glGenBuffers(1, &curveVBO);

		glBindVertexArray(curveVAO);

		glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
		glBufferData(GL_ARRAY_BUFFER, ccount * 2 * sizeof(float), curve, GL_STATIC_DRAW);

		//绑定位置坐标
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_POINTS, 0, ccount);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &curveVAO);
		glDeleteBuffers(1, &curveVBO);

		//动态
		if (points.size() > 3) {
			dynamic(points);
			check += 0.0002;
			if (check > 1) check = 1;
		}

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}