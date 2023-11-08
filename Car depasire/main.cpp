#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL.h"	
//////////////////////////////////////

GLuint
VaoId,
VboId,
EboId,
ProgramId,
myMatrixLocation,
viewLocation,
projLocation,
matrRotlLocation,
textureCar,
texturePolis,
textureRoad,
texturePlains;

GLfloat
	winWidth = 1200, winHeight = 900;
glm::mat4
	myMatrix, matrRot, resizeMatrix;
int codTexture;
GLfloat xMin = -400.f, xMax = 400.f, yMin = -300.f, yMax = 300.f;

void LoadTexture(const char* photoPath, GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//	Desfasurarea imaginii pe orizonatala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("main.vert", "main.frag");
	glUseProgram(ProgramId);
}

void CreateVBO()
{
	GLfloat Car[] = {
		//first rectangle
		-400.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		-400.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		-320.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		-320.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		//second rectangle
		-300.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		-300.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		-220.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		-220.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		//road
	    -400.f,  100.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	    -400.f, -100.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		 400.f, -100.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		 400.f,  100.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f,

		 xMin,  yMax, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		 xMin,  yMin, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		 xMax,  yMin, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		 xMax,  yMax, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	};

	static const GLuint Indices[] = {
		//first rectangle
		0, 1, 2,
		0, 2, 3,
		//second rectangle
		4, 5, 6,
		4, 6, 7,
		//road
		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15
	};
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Car), Car, GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	//  Se asociaza atributul (2 =  texturare) pentru shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));


}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Textura), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}


void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	CreateVBO();	//  Culoarea de fond a ecranului;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniform1i(glGetUniformLocation(ProgramId, "carTexture"), 0);
	LoadTexture("car.png", textureCar);
	LoadTexture("polis.png", texturePolis);
	LoadTexture("road.png", textureRoad);
	//LoadTexture("plains.png", texturePlains);
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//myMatrix = resizeMatrix; // Update the matrix for the second rectangle
	//glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	//glBindTexture(GL_TEXTURE_2D, texturePlains);
	//glUniform1i(glGetUniformLocation(ProgramId, "carTexture"), 0);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18* sizeof(GLuint)));

	myMatrix = resizeMatrix; // Update the matrix for the second rectangle
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, textureRoad);
	glUniform1i(glGetUniformLocation(ProgramId, "carTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));

	// Draw the first rectangle (car)
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, textureCar);
	glUniform1i(glGetUniformLocation(ProgramId, "carTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

	// Load the texture for the second rectangle (polis)
	 //Draw the second rectangle (polis)
	myMatrix = resizeMatrix; // Update the matrix for the second rectangle
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glBindTexture(GL_TEXTURE_2D, texturePolis);
	glUniform1i(glGetUniformLocation(ProgramId, "carTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

	glFlush();
}
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Depasire nebuna");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}