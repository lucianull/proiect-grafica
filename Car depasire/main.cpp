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
	codColLocation,
	codTextureLocation,
	texture;

GLfloat
	winWidth = 1200, winHeight = 900;
glm::mat4
	myMatrix, matrRot;
int codTexture;
float xMin = -400.f, xMax = 400, yMin = -300, yMax = 300;

struct Point {
	GLfloat x, y, z, t, colR, colG, colB, shadX, shadY;
	Point() {}
	Point(GLfloat x, GLfloat y, GLfloat z, GLfloat t, GLfloat colR, GLfloat colG, GLfloat colB, GLfloat shadX, GLfloat shadY)
		: x(x), y(y), z(z), t(t), colR(colR), colG(colG), colB(colB), shadX(shadX), shadY(shadY) {
	}
	Point& operator=(Point& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
			z = other.z;
			t = other.t;
			colR = other.colR;
			colG = other.colG;
			colB = other.colB;
			shadX = other.shadX;
			shadY = other.shadY;
		}
		return *this;
	}
};

class RectangleCar
{
public:
	Point* Vertices; //left-down : right-down : right-up: left - up
	int Indices[6];
	RectangleCar() {}
	RectangleCar(Point LD, Point RD, Point RU, Point LU) {
		Vertices = new Point[4];
		Vertices[0] = LD;
		Vertices[1] = RD;
		Vertices[2] = RU;
		Vertices[3] = LU;
		Indices[0] = 0;
		Indices[1] = 1;
		Indices[2] = 2;
		Indices[3] = 0;
		Indices[4] = 2;
		Indices[5] = 3;
	}
	~RectangleCar() {
		delete[] Vertices;
	}
};

RectangleCar Cars[2];


void LoadTexture(const char* photoPath)
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
	Cars[0] = RectangleCar(
		Point(-400.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f),
		Point(-400.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f),
		Point(-320.f, -80.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f),
		Point(-320.f, -40.f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f)
	);

	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, Cars[0].Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6, Cars[0].Indices, GL_STATIC_DRAW);

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	codTextureLocation = glGetUniformLocation(ProgramId, "codTexture");
	LoadTexture("car.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	codTexture = 1;
	glUniform1i(codTextureLocation, codTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
	glFlush();								//  here i get exception thrown
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