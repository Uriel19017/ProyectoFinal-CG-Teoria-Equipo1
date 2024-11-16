/*
					Proyecto Lab CGyIHC
						Equipo 1
					   14/11/2024
*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>

#include <cmath>
#include <fstream> // Para manejo de archivos



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
//void animate(void);

void DoMovement();
void Animation();
void AnimationCar();
void AnimationBoy();
void AnimationDrone();
void saveAnimationToFile();
void loadAnimationFromFile();
void clearAnimationFile();
void resetAnimationAndFile();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(10.0f, 10.0f, 10.0f));
float MovementSpeed = 0.01f; //0.01
GLfloat lastX = SCR_WIDTH / 2.0f,
lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool keys[1024];

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS;
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool, bool);



//Variables Animaciones

// Drone animation
bool Animdrone = false;
float rotdrone = 0.0f;
float ydronemovement = 0.0f;

//Auto
float rotWheel = 0.0f;
float rotAuto = 0.0f;

bool AnimCar = false; // Variable para activar y desactivar mi animación
bool step = false;
float CarRotation = 0.0f;  // Ángulo de rotación del carro

glm::vec3 AutoPos(-1.8f, 0.25f, 0.9f);

//Boy

float rightLegRotation = 0.0f;
float leftLegRotation = 0.0f;
float rightArmRotation = 0.0f;
float leftArmRotation = 0.0f;
float headRotation = 0.0f;

bool AnimBoy = false; // Variable to activate and deactivate my animation
bool step1 = false;

float BoyRotation = 0.0f;  // Dog rotation angle
glm::vec3 BoyPos(0.0f, 0.0f, 0.0f);

//Bird 
float rotBird = 0.0f;
float rightWingR = 0.0f;
float leftWingR = 0.0f;
float tailBird = 0.0f;

// Variables para manejar los 4 estados del carro
bool estado1Activo = true;
bool estado2Activo = false;
bool estado3Activo = false;
bool estado4Activo = false;
bool estado5Activo = false;
bool estado6Activo = false;

// Variables para manejar los 5 estados del Boy
bool estado1ActivoB = true;
bool estado2ActivoB = false;
bool estado3ActivoB = false;
bool estado4ActivoB = false;


//KeyFrames
float BirdPosX, BirdPosY, BirdPosZ;

//Lighting
glm::vec3 lightPosition(0.0f, 15.0f, -10.0f);
float posi = 0.0f;
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
float vari = 0.0f;
const float rotationSpeed = 0.006f; // Velocidad de rotación


//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
//glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = glm::vec3(0.5f);



#define MAX_FRAMES 100
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	
	float rotBird; 
	float rotBirdInc;
	float BirdPosX;
	float BirdPosY;
	float BirdPosZ;
	float BirdPosXInc;
	float BirdPosYInc;
	float BirdPosZInc;

	float rightWingR;
	float leftWingR;
	float tailBird;
	float rightWingRInc;
	float leftWingRInc;
	float tailBirdInc;


}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].rotBird = rotBird;
	KeyFrame[FrameIndex].BirdPosX = BirdPosX;
	KeyFrame[FrameIndex].BirdPosY = BirdPosY;
	KeyFrame[FrameIndex].BirdPosZ = BirdPosZ;

	KeyFrame[FrameIndex].rightWingR = rightWingR;
	KeyFrame[FrameIndex].leftWingR = leftWingR;
	KeyFrame[FrameIndex].tailBird = tailBird;

	FrameIndex++;
}

void resetElements(void)
{
	rotBird = KeyFrame[0].rotBird;
	BirdPosX = KeyFrame[0].BirdPosX;
	BirdPosY = KeyFrame[0].BirdPosY;
	BirdPosZ = KeyFrame[0].BirdPosZ;

	rightWingR = KeyFrame[0].rightWingR;
	leftWingR = KeyFrame[0].leftWingR;
	tailBird = KeyFrame[0].tailBird;
}

void interpolation(void)
{

	
	KeyFrame[playIndex].rotBirdInc = (KeyFrame[playIndex + 1].rotBird - KeyFrame[playIndex].rotBird) / i_max_steps;
	KeyFrame[playIndex].BirdPosXInc = (KeyFrame[playIndex + 1].BirdPosX - KeyFrame[playIndex].BirdPosX) / i_max_steps;
	KeyFrame[playIndex].BirdPosYInc = (KeyFrame[playIndex + 1].BirdPosY - KeyFrame[playIndex].BirdPosY) / i_max_steps;
	KeyFrame[playIndex].BirdPosZInc = (KeyFrame[playIndex + 1].BirdPosZ - KeyFrame[playIndex].BirdPosZ) / i_max_steps;

	KeyFrame[playIndex].rightWingRInc = (KeyFrame[playIndex + 1].rightWingR - KeyFrame[playIndex].rightWingR) / i_max_steps;
	KeyFrame[playIndex].leftWingRInc = (KeyFrame[playIndex + 1].leftWingR - KeyFrame[playIndex].leftWingR) / i_max_steps;
	KeyFrame[playIndex].tailBirdInc = (KeyFrame[playIndex + 1].tailBirdInc - KeyFrame[playIndex].tailBirdInc) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	if (isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	
}


// Limpiar el archivo de animación
void clearAnimationFile() {
	std::ofstream outFile("animBird_data.bin", std::ios::binary | std::ios::trunc);
	if (outFile.is_open()) {
		outFile.close();
		std::cout << "Archivo de animacion limpiado exitosamente.\n";
	}
	else {
		std::cerr << "Error al abrir el archivo para limpiar.\n";
	}
}

// Reiniciar la animación y limpiar el archivo
void resetAnimationAndFile() {
	// Limpiar el archivo de animación
	clearAnimationFile();

	// Reiniciar las posiciones del modelo a las iniciales usando KeyFrame[0]
	resetElements();

	// Reiniciar el índice de los frames
	FrameIndex = 0;

	// Reiniciar el estado de la animación
	play = false;           // Detener la animación
	playIndex = 0;

	// Guardar el estado inicial en el archivo
	saveAnimationToFile(); // Llama a esta función para guardar las posiciones iniciales

	// Reiniciar el estado de la animación
	play = false; // Detener la animación
	playIndex = 0; // Reiniciar el índice
	i_curr_steps = 0; // Reiniciar el contador de pasos
	std::cout << "Animacion reiniciada.\n";
}

// Guardar los KeyFrames en mi archivo binario
void saveAnimationToFile() {
	std::ofstream outFile("animBird_data.bin", std::ios::binary);
	if (outFile.is_open()) {
		int framesToSave = std::min(FrameIndex, MAX_FRAMES);
		outFile.write(reinterpret_cast<char*>(&framesToSave), sizeof(framesToSave));
		outFile.write(reinterpret_cast<char*>(KeyFrame), sizeof(FRAME) * framesToSave); // Guarda solo los frames utilizados
		outFile.close();
		//std::cout << "Animacion guardada exitosamente.\n";
	}
	else {
		std::cerr << "Error al abrir el archivo para guardar.\n";
	}
}

// Cargar los frames desde mi archivo binario
void loadAnimationFromFile() {
	std::ifstream inFile("animBird_data.bin", std::ios::binary);
	if (inFile.is_open()) {
		inFile.read(reinterpret_cast<char*>(&FrameIndex), sizeof(FrameIndex));
		inFile.read(reinterpret_cast<char*>(KeyFrame), sizeof(FRAME) * FrameIndex); // Carga solo la cantidad de frames guardados
		inFile.close();
		std::cout << "Animacion cargada exitosamente.\n";
	}
	else {
		std::cerr << "No se encontro archivo de animacion guardada.\n";
	}
}

void Animation() {
	if (play) {
		// Verificar si hemos llegado al final de la interpolación entre frames
		if (i_curr_steps >= i_max_steps) {
			playIndex++;
			// Verificar si hemos llegado al final de la animación
			if (playIndex >= FrameIndex - 1) {
				//printf("Termina animacion\n");
				playIndex = 0; // Reiniciar al principio
				play = false;  // Detener la animación
			}
			else {
				// Preparar la interpolación para el siguiente frame
				i_curr_steps = 0; // Reiniciar el contador
				interpolation();   // Calcular la interpolación del siguiente frame
			}
		}
		else {
			// Actualizar las posiciones y rotaciones para la animación
			
			rotBird += KeyFrame[playIndex].rotBirdInc;

			BirdPosX += KeyFrame[playIndex].BirdPosXInc;
			BirdPosY += KeyFrame[playIndex].BirdPosYInc;
			BirdPosZ += KeyFrame[playIndex].BirdPosZInc;

			rightWingR += KeyFrame[playIndex].rightWingRInc;
			leftWingR += KeyFrame[playIndex].leftWingRInc;
			tailBird += KeyFrame[playIndex].tailBirdInc;

			// Incrementar el contador de pasos
			i_curr_steps++;
		}
	}
}

void AnimationCar() {

	if (AnimCar) { // Si la animación del carro está activa

		// Estado 1: caminar mientras AutoPos.x < 2 y girar 90°
		if (estado1Activo) {
			if (AutoPos.z < 24.3f) {
				// Movimiento hacia adelante
				AutoPos.z += 0.12f;  //modificar a 1.0
				//AutoPos.z += 0.00012f;
				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f; //mod a 1.o dependiendo 
				}
			}
			else {
				// Cambiar a girar 90 grados
				if (rotAuto < 90.0f) {
					rotAuto += 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {
					// Cambiar al estado 2
					estado1Activo = false;
					estado2Activo = true;
				}
			}
		}

		// Estado 2: caminar hacia adelante mientras AutoPos.z < 2
		if (estado2Activo) {
			if (AutoPos.x > -15.5f) {
				// Movimiento hacia adelante
				AutoPos.x -= 0.12f;

				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f;
				}
			}
			else {
				// Girar 90 grados
				if (rotAuto < 180.0f) {
					rotAuto += 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {
					// Cambiar al estado 3
					estado2Activo = false;
					estado3Activo = true;
				}
			}
		}
		// Estado 3: caminar hacia adelante mientras AutoPos.z < 2
		if (estado3Activo) {
			if (AutoPos.z > 19.5f) {
				// Movimiento hacia adelante
				AutoPos.z -= 0.12f;

				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f;
				}
			}
			else {
				// Girar 90 grados
				if (rotAuto < 270.0f) {
					rotAuto += 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {
					// Cambiar al estado 4
					estado3Activo = false;
					estado4Activo = true;
				}
			}
		}
		// Estado 4: caminar hacia adelante mientras AutoPos.z < 2
		if (estado4Activo) {
			if (AutoPos.x < -3.0f) {
				// Movimiento hacia adelante
				AutoPos.x += 0.12f;

				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f;
				}
			}
			else {
				// Girar 90 grados
				if (rotAuto > 180.0f) {
					rotAuto -= 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {

					// Cambiar al estado 4
					estado4Activo = false;
					estado5Activo = true;

				}
			}
		}
		// Estado 5: caminar hacia adelante mientras AutoPos.z < 2
		if (estado5Activo) {
			if (AutoPos.z > 1.3f) {
				// Movimiento hacia adelante
				AutoPos.z -= 0.12f;

				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f;
				}
			}
			else {
				// Girar 90 grados
				if (rotAuto < 270.0f) {
					rotAuto += 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {
					// Cambiar al estado 2 directamente
					estado5Activo = false;
					estado6Activo = true; // Pasar al estado 2
				}
			}
		}
		// Estado 6: caminar hacia adelante mientras AutoPos.z < 2
		if (estado6Activo) {
			if (AutoPos.x < -1.8f) {
				// Movimiento hacia adelante
				AutoPos.x += 0.12f;

				// Movimiento de las ruedas
				if (!step) {
					rotWheel -= 12.0f;
				}
			}
			else {
				// Girar 90 grados
				if (rotAuto < 360.0f) {
					rotAuto += 6.0f;

					// Movimiento de las ruedas mientras gira
					if (!step) {
						rotWheel -= 12.0f;
					}
				}
				else {
					// Cambiar al estado 3 directamente
					rotAuto = 0.0f;
					estado6Activo = false;
					estado1Activo = true; // Pasar al estado 2

				}
			}
		}
	}
}

void AnimationBoy() {


	if (AnimBoy) { // Walk animation

		// Estado 1: caminar hacia adelante
		if (estado1ActivoB) {
			if (BoyPos.z < 4.1f) {
				// Movimiento hacia adelante
				BoyPos.z += 0.04;

				// Movimiento de cabeza, brazos y pies.
				if (!step1) {
					rightLegRotation += 4.5f;
					leftLegRotation -= 4.5f;
					rightArmRotation += 4.5;
					leftArmRotation -= 4.5f;
					headRotation += 1.8f;


					if (rightLegRotation > 45.0f) {
						step1 = true;
					}
				}
				else {
					rightLegRotation -= 4.5f;
					leftLegRotation += 4.5f;
					rightArmRotation -= 4.5;
					leftArmRotation += 4.5f;
					headRotation -= 1.8f;

					if (rightLegRotation < -45.0f) {
						step1 = false;
					}
				}
			}
			else {
				// Girar 90 grados
				if (BoyRotation < 90.0f) {
					BoyRotation += 3.5;

					if (!step1) {
						rightLegRotation += 4.5f;
						leftLegRotation -= 4.5f;
						rightArmRotation += 4.5;
						leftArmRotation -= 4.5f;
						headRotation += 1.8f;

						if (rightLegRotation > 45.0f) {
							step1 = true;
						}
					}
					else {
						rightLegRotation -= 4.5f;
						leftLegRotation += 4.5f;
						rightArmRotation -= 4.5;
						leftArmRotation += 4.5f;
						headRotation -= 1.8f;

						if (rightLegRotation < -45.0f) {
							step1 = false;
						}
					}

				}
				else {
					// Cambiar al estado 2
					estado1ActivoB = false;
					estado2ActivoB = true;
				}
			}
		}

		// Estado 2: caminar hacia la derecha
		if (estado2ActivoB) {
			if (BoyPos.x < 6.3f) {

				BoyPos.x += 0.04;

				// Movimiento de cabeza, brazos y pies.
				if (!step1) {
					rightLegRotation += 4.5f;
					leftLegRotation -= 4.5f;
					rightArmRotation += 4.5;
					leftArmRotation -= 4.5f;
					headRotation += 1.8f;

					if (rightLegRotation > 45.0f) {
						step1 = true;
					}
				}
				else {
					rightLegRotation -= 4.5f;
					leftLegRotation += 4.5f;
					rightArmRotation -= 4.5;
					leftArmRotation += 4.5f;
					headRotation -= 1.8f;

					if (rightLegRotation < -45.0f) {
						step1 = false;
					}
				}
			}
			else {
				// Girar 180 grados
				if (BoyRotation < 180.0f) {
					BoyRotation += 3.5;
					// Movimiento de cabeza, brazos y pies.
					if (!step1) {
						rightLegRotation += 4.5f;
						leftLegRotation -= 4.5f;
						rightArmRotation += 4.5;
						leftArmRotation -= 4.5f;
						headRotation += 1.8f;;

						if (rightLegRotation > 45.0f) {
							step1 = true;
						}
					}
					else {
						rightLegRotation -= 4.5f;
						leftLegRotation += 4.5f;
						rightArmRotation -= 4.5;
						leftArmRotation += 4.5f;
						headRotation -= 1.8f;

						if (rightLegRotation < -45.0f) {
							step1 = false;
						}
					}

				}
				else {
					// Cambiar al estado 3
					estado2ActivoB = false;
					estado3ActivoB = true;
				}
			}
		}

		// Estado 3: 
		if (estado3ActivoB) {
			if (BoyPos.z > 0.0f) {
				
				BoyPos.z -= 0.04;

				// Movimiento de cabeza, brazos y pies.
				if (!step1) {
					rightLegRotation += 4.5f;
					leftLegRotation -= 4.5f;
					rightArmRotation += 4.5;
					leftArmRotation -= 4.5f;
					headRotation += 1.8f;

					if (rightLegRotation > 45.0f) {
						step1 = true;
					}
				}
				else {
					rightLegRotation -= 4.5f;
					leftLegRotation += 4.5f;
					rightArmRotation -= 4.5;
					leftArmRotation += 4.5f;
					headRotation -= 1.8f;

					if (rightLegRotation < -45.0f) {
						step1 = false;
					}
				}
			}
			else {
				// Girar 270 grados
				if (BoyRotation < 270.0f) {
					BoyRotation += 3.5;
					// Movimiento de cabeza, brazos y pies.
					if (!step1) {
						rightLegRotation += 4.5f;
						leftLegRotation -= 4.5f;
						rightArmRotation += 4.5;
						leftArmRotation -= 4.5f;
						headRotation += 1.8f;

						if (rightLegRotation > 45.0f) {
							step1 = true;
						}
					}
					else {
						rightLegRotation -= 4.5f;
						leftLegRotation += 4.5f;
						rightArmRotation -= 4.5;
						leftArmRotation += 4.5f;
						headRotation -= 1.8f;

						if (rightLegRotation < -45.0f) {
							step1 = false;
						}
					}
				}
				else {
					// Cambiar al estado 4
					estado3ActivoB = false;
					estado4ActivoB = true;
				}
			}
		}
		// Estado 4: 
		if (estado4ActivoB) {
			if (BoyPos.x > 0.5f) {
				// Movimiento hacia la izquierda
				BoyPos.x -= 0.04;

				// Movimiento de cabeza, brazos y pies.
				if (!step1) {
					rightLegRotation += 4.5f;
					leftLegRotation -= 4.5f;
					rightArmRotation += 4.5;
					leftArmRotation -= 4.5f;
					headRotation += 1.8f;

					if (rightLegRotation > 45.0f) {
						step1 = true;
					}
				}
				else {
					rightLegRotation -= 4.5f;
					leftLegRotation += 4.5f;
					rightArmRotation -= 4.5;
					leftArmRotation += 4.5f;
					headRotation -= 1.8f;

					if (rightLegRotation < -45.0f) {
						step1 = false;
					}
				}
			}
			else {
				// Girar 360
				if (BoyRotation < 360.0f) {
					BoyRotation += 3.5;
					// Movimiento de cabeza, brazos y pies.
					if (!step1) {
						rightLegRotation += 4.5f;
						leftLegRotation -= 4.5f;
						rightArmRotation += 4.5;
						leftArmRotation -= 4.5f;
						headRotation += 1.8f;

						if (rightLegRotation > 45.0f) {
							step1 = true;
						}
					}
					else {
						rightLegRotation -= 4.5f;
						leftLegRotation += 4.5f;
						rightArmRotation -= 4.5;
						leftArmRotation += 4.5f;
						headRotation -= 1.8f;

						if (rightLegRotation < -45.0f) {
							step1 = false;
						}
					}

				}
				else {
					// Cambiar al estado 1 para volver a empezar animación
					BoyRotation = 0.0f;
					estado4ActivoB = false;
					estado1ActivoB = true;
				}
			}
		}

	}

}

void AnimationDrone() {
	if (Animdrone) {
		static float direction = 1.5f;
		rotdrone += 10.1;
		ydronemovement += direction;

		if (ydronemovement >= 1.5f) {
			direction = -1.5f;
		}

		if (ydronemovement <= -1.5f) {
			direction = 1.5f;
		}
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   2.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   2.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final Equipo 01", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	loadAnimationFromFile();

	glEnable(GL_DEPTH_TEST);



	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		"resources/SkyBox2/right.jpg",
		"resources/SkyBox2/left.jpg",
		"resources/SkyBox2/top.jpg",
		"resources/SkyBox2/bottom.jpg",
		"resources/SkyBox2/front.jpg",
		"resources/SkyBox2/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//Models

	
	//Model estadio("Models/estadio/estadio/estadio2.obj");
	
	Model terreno("resources/ter/ter.obj");
	
	//Model car((char*)"Models/ter/ter.obj");

	//Model Piso((char*)"Models/piso.obj");
	
	
	Model arbol((char*)"Models/arbol/tree_in_OBJ.obj");
	Model poste1((char*)"Models/POSTE1/poste1.obj");
	// golf
	
	Model golf((char*)"Models/carro/car.obj");
	
	// Car
	Model Carroceria((char*)"Models/Auto_Lab/Carroceria.obj");
	Model Wheel((char*)"Models/Auto_Lab/Wheel.obj");

	// Boy
	Model botaDer((char*)"Models/Personaje/bota.obj");
	Model piernaDer((char*)"Models/Personaje/piernader.obj");
	Model piernaIzq((char*)"Models/Personaje/piernader.obj");
	Model torso((char*)"Models/Personaje/torso.obj");
	Model brazoDer((char*)"Models/Personaje/brazoder.obj");
	Model brazoIzq((char*)"Models/Personaje/brazoizq.obj");
	Model cabeza((char*)"Models/Personaje/cabeza.obj");

	// Bird
	Model birdBody((char*)"Models/bird/body.obj");
	Model leftWing((char*)"Models/bird/left_wing.obj");
	Model rightWing((char*)"Models/bird/left_wingtrue.obj");
	Model tail((char*)"Models/bird/tail.obj");

	// Drone
	Model drone((char*)"Models/drone/drone1.obj");

	// Condominio
	Model condo((char*)"Models/edificios/edificio4.obj");

	// Hotel reception
	Model reception((char*)"Models/reception_models/reception/reception.obj");
	Model chair((char*)"Models/reception_models/chair/chair.obj");
	Model coffee_table((char*)"Models/reception_models/coffee_table/coffeeTable.obj");
	Model couch((char*)"Models/reception_models/couch/couch.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].rotBird = 0;
		KeyFrame[i].BirdPosX = 0;
		KeyFrame[i].BirdPosXInc = 0;
		KeyFrame[i].BirdPosY = 0;
		KeyFrame[i].BirdPosYInc = 0;
		KeyFrame[i].BirdPosZ = 0;
		KeyFrame[i].BirdPosZInc = 0;

		KeyFrame[i].rightWingR = 0;
		KeyFrame[i].rightWingRInc = 0;
		KeyFrame[i].leftWingR = 0;
		KeyFrame[i].leftWingRInc = 0;
		KeyFrame[i].tailBird = 0;
		KeyFrame[i].tailBirdInc = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	//Cambios
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelTemp = glm::mat4(1.0f);
	glm::mat4 modelTempA = glm::mat4(1.0f);
	glm::mat4 modelTempB = glm::mat4(1.0f);
	glm::mat4 modelTempC = glm::mat4(1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		//animate();

		DoMovement();
		Animation();
		AnimationCar();
		AnimationBoy();
		AnimationDrone();

		


		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 1.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", glm::vec3(0.6f, 0.6f, 1.0f));
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);
		myShader.use();

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		

		/*model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		staticShader.setMat4("model", model);
		terreno.Draw(staticShader);*/


		//Se configura la escala del auto
		float scaleFactor = 0.002f;
		//model = glm::mat4(1);
		
		// Auto
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelTempA = model = glm::translate(model, AutoPos);
		modelTempA = model = glm::rotate(model, glm::radians(rotAuto), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		staticShader.setMat4("model", model);
		Carroceria.Draw(staticShader);

		// Llanta izquierda delantera
		model = modelTempA;
		model = glm::translate(model, glm::vec3(8.5f * 0.02f, 2.5f * 0.02f, 12.9f * 0.02f)); // Nueva posición
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		Wheel.Draw(staticShader);

		// Llanta izquierda delantera
		model = modelTempA;
		model = glm::translate(model, glm::vec3(-8.5f * 0.02f, 2.5f * 0.02f, 12.9f * 0.02f)); // Nueva posición
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		Wheel.Draw(staticShader);

		// Llanta izquierda trasera
		model = modelTempA;
		model = glm::translate(model, glm::vec3(-8.5f * 0.02f, 2.5f * 0.02f, -14.5f * 0.02f)); // Nueva posición
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		Wheel.Draw(staticShader);

		// Llanta izquierda trasera
		model = modelTempA;
		model = glm::translate(model, glm::vec3(8.5f * 0.02f, 2.5f * 0.02f, -14.5f * 0.02f)); // Nueva posición
		model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor)); // Aplica la escala
		model = glm::rotate(model, glm::radians(rotWheel), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		Wheel.Draw(staticShader);
//##################################################################################################################################
		
		
		//Muñequito
		model = glm::mat4(1);
		staticShader.setMat4("model", model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 0.3f, -11.5f));
		model = glm::translate(model, BoyPos);
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		modelTempB = model = glm::rotate(model, glm::radians(BoyRotation), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		torso.Draw(staticShader);

		//Pierna Der
		model = glm::translate(modelTempB, glm::vec3(-0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(rightLegRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaDer.Draw(staticShader);

		//Pie Der
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);

		//Pierna Izq
		model = glm::translate(modelTempB, glm::vec3(0.5f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(leftLegRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", model);
		botaDer.Draw(staticShader);

		//Brazo derecho
		model = glm::translate(modelTempB, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(rightArmRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoDer.Draw(staticShader);

		//Brazo izquierdo
		model = glm::translate(modelTempB, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
		model = glm::rotate(model, glm::radians(leftArmRotation), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoIzq.Draw(staticShader);

		//Cabeza
		model = glm::translate(modelTempB, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(headRotation), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", model);
		cabeza.Draw(staticShader);


// ####################################################################################################################
		//Bird
		model = glm::mat4(1);
		// modifica la escala
		//model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, 2.0f, -13.0f));// modifica la ubicacion general 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 1.8f, -8.1f));// modifica la ubicacion general
		model = glm::translate(model, glm::vec3(BirdPosX, BirdPosY, BirdPosZ));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));// moodifica la escala general
		model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelTempC = model = glm::rotate(model, glm::radians(rotBird), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		birdBody.Draw(staticShader);

		//Ala Derecha
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rightWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		leftWing.Draw(staticShader);

		//Ala Izquierda
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(leftWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rightWing.Draw(staticShader);

		//Cola Bird
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(tailBird), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		tail.Draw(staticShader);


		//Bird 2
		model = glm::mat4(1);
		// modifica la escala
		//model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, 2.0f, -13.0f));// modifica la ubicacion general 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(9.5f, 1.8f, -8.1f));// modifica la ubicacion general
		model = glm::translate(model, glm::vec3(BirdPosX, BirdPosY, BirdPosZ));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));// moodifica la escala general
		model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelTempC = model = glm::rotate(model, glm::radians(rotBird), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		birdBody.Draw(staticShader);

		//Ala Derecha
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rightWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		leftWing.Draw(staticShader);

		//Ala Izquierda
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(leftWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rightWing.Draw(staticShader);

		//Cola Bird
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(tailBird), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		tail.Draw(staticShader);


		//Bird 3
		model = glm::mat4(1);
		// modifica la escala
		//model = glm::translate(glm::mat4(1.0f), glm::vec3(27.0f, 2.0f, -13.0f));// modifica la ubicacion general 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(8.5f, 1.8f, -8.1f));// modifica la ubicacion general
		model = glm::translate(model, glm::vec3(BirdPosX, BirdPosY, BirdPosZ));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));// moodifica la escala general
		model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelTempC = model = glm::rotate(model, glm::radians(rotBird), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", model);
		birdBody.Draw(staticShader);

		//Ala Derecha
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rightWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		leftWing.Draw(staticShader);

		//Ala Izquierda
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(leftWingR), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		rightWing.Draw(staticShader);

		//Cola Bird
		model = glm::translate(modelTempC, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(tailBird), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		tail.Draw(staticShader);

		// Drone
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		model = glm::translate(model, glm::vec3(330.0f, 1.8f, -190.0f));
		model = glm::translate(model, glm::vec3(0.0f, ydronemovement, 0.0f));
		model = glm::rotate(model, glm::radians(rotdrone), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		drone.Draw(staticShader);


		// Condominio
		
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.09f));
		model = glm::translate(model, glm::vec3(130.0f, 0.8f, -155.0f));
		staticShader.setMat4("model", model);
		condo.Draw(staticShader);
		
		//Arbol
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		model = glm::translate(model, glm::vec3(8.0f, -3.4f, 0.0f));
		staticShader.setMat4("model", model);
		//arbol.Draw(staticShader);

		//Poste
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		model = glm::translate(model, glm::vec3(-200.0f, -35.0f, 3.0f));
		staticShader.setMat4("model", model);
		//poste1.Draw(staticShader);

		//golf
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.145f));
		model = glm::translate(model, glm::vec3(20.0f, 2.7f, -4.0f));
		staticShader.setMat4("model", model);
		golf.Draw(staticShader);

		// Reception models

		// Reception 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(11.0f, 0.27f, -17.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		staticShader.setMat4("model", model);
		reception.Draw(staticShader);

		// Sofa 1.1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10.8f, 0.287f, -16.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 1.2
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10.8f, 0.287f, -16.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 1.3
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(11.0f, 0.287f, -16.6f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 1.4
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(11.0f, 0.287f, -16.7f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Coffee table 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10.9f, 0.287f, -16.61f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		coffee_table.Draw(staticShader);

		// Couch 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10.9f, 0.287f, -16.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		couch.Draw(staticShader);

		// Reception 2
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.5f, 0.27f, -17.0f));
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		staticShader.setMat4("model", model);
		reception.Draw(staticShader);

		// Sofa 2.1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.3f, 0.287f, -16.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 2.2
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.3f, 0.287f, -16.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 2.3
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.6f, 0.287f, -16.6f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Sofa 2.4
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.6f, 0.287f, -16.7f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		chair.Draw(staticShader);

		// Coffee table 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.5f, 0.287f, -16.61f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		coffee_table.Draw(staticShader);

		// Couch 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.5f, 0.287f, -16.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.06f));
		staticShader.setMat4("model", model);
		couch.Draw(staticShader);

		glDisable(GL_BLEND);
		
		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	//Bird Controls

	if (keys[GLFW_KEY_1])
	{

		rightWingR += 40.0f;
		leftWingR -= 40.0f;

	}

	if (keys[GLFW_KEY_2])
	{

		rightWingR -= 40.0f;
		leftWingR += 40.0f;

	}


	///////////////////////////// 


	if (keys[GLFW_KEY_3])
	{

		BirdPosY += 0.13f;
		rightWingR -= 65.0f;
		leftWingR += 65.0f;

	}
	if (keys[GLFW_KEY_4])
	{

		BirdPosY -= 0.020f;
		rightWingR += 3.8f;
		leftWingR -= 3.8f;

	}



	//ROTACIÓN

	if (keys[GLFW_KEY_5])
	{
		//BirdPosX += 0.015f;
		rotBird -= 0.13f;
		rightWingR -= 65.0f;
		leftWingR += 65.0f;

	}

	if (keys[GLFW_KEY_6])
	{
		//BirdPosX += 0.015f;
		rotBird -= 0.13f;
		rightWingR += 65.0f;
		leftWingR -= 65.0f;

	}

	if (keys[GLFW_KEY_7])
	{
		BirdPosZ += 0.020f;
		rightWingR -= 3.8f;
		leftWingR += 3.8f;

	}

	if (keys[GLFW_KEY_8])
	{
		BirdPosZ += 0.020f;
		rightWingR += 3.8f;
		leftWingR -= 3.8f;

	}

	if (keys[GLFW_KEY_Z])
	{
		BirdPosZ -= 0.020f;
		rightWingR -= 3.8f;
		leftWingR += 3.8f;

	}

	if (keys[GLFW_KEY_X])
	{
		BirdPosZ -= 0.020f;
		rightWingR += 3.8f;
		leftWingR -= 3.8f;

	}



	//Movimiento X Y Z

	if (keys[GLFW_KEY_H])
	{

		BirdPosX += 0.13f;
		rightWingR += 65.0f;
		leftWingR -= 65.0f;

	}

	if (keys[GLFW_KEY_F])
	{

		BirdPosX += 0.13f;
		rightWingR -= 65.0f;
		leftWingR += 65.0f;

	}


	if (keys[GLFW_KEY_U])
	{

		BirdPosX -= 0.020f;
		rightWingR += 3.8f;
		leftWingR -= 3.8f;

	}

	if (keys[GLFW_KEY_I])
	{

		BirdPosX -= 0.020f;
		rightWingR -= 3.8f;
		leftWingR += 3.8f;

	}


	if (keys[GLFW_KEY_R])
	{

		BirdPosY += 0.13f;
		rightWingR += 65.0f;
		leftWingR -= 65.0f;

	}

	if (keys[GLFW_KEY_Y])
	{

		BirdPosY -= 0.015f;
		rightWingR -= 3.8f;
		leftWingR += 3.8f;

	}
	if (keys[GLFW_KEY_G])
	{

		BirdPosZ -= 0.015f;
		rightWingR -= 3.8f;
		leftWingR += 3.8f;

	}

	if (keys[GLFW_KEY_T])
	{

		BirdPosZ -= 0.015f;
		rightWingR += 3.8f;
		leftWingR -= 3.8f;

	}



	//// Camera controls
	//if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	//{
	//	camera.ProcessKeyboard(FORWARD, deltaTime);

	//}

	//if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	//{
	//	camera.ProcessKeyboard(BACKWARD, deltaTime);


	//}

	//if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	//{
	//	camera.ProcessKeyboard(LEFT, deltaTime);


	//}

	//if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	//{
	//	camera.ProcessKeyboard(RIGHT, deltaTime);

	//}


}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime * MovementSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime * MovementSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime * MovementSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime * MovementSpeed);



	//CAMBIOS
	// Reproducir animación
	if (keys[GLFW_KEY_L]) {
		if (!play && FrameIndex > 1) {
			resetElements();
			interpolation();
			loadAnimationFromFile();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else {
			play = false;
		}
	}

	// Guardar la animación
	if (keys[GLFW_KEY_K]) {
		if (FrameIndex < MAX_FRAMES) {
			saveFrame();
			saveAnimationToFile();
		}
	}

	//Limpiar el archivo y reiniciar animación al presionar 0
	if (keys[GLFW_KEY_0]) {
		resetAnimationAndFile();
	}

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_B])
	{
		AnimBoy = !AnimBoy; // Activa o desactiva la animaci?n
		AnimCar = !AnimCar;
		Animdrone = !Animdrone;

	}


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}