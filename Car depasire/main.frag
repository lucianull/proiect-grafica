//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_02_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core

//	Variabile de intrare (dinspre Shader.vert);
in vec4 ex_Color;
in vec2 tex_Coord;		//	Coordonata de texturare;
//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;		//	Culoarea actualizata;
//	Variabilele uniforme;
uniform int codTexture;
uniform sampler2D carTexture;
uniform sampler2D policeTexture;

//	Variabile pentru culori;
vec4 red = vec4(1.0,0.0,0.0,1.0);
vec4 green= vec4(0.0,1.0,0.0,1.0);

void main(void)
  {
    if(codTexture == 0)
    {
         out_Color = texture(policeTexture, tex_Coord);
    }
    else
    {
        out_Color = texture(carTexture, tex_Coord);	//	Amestecarea texturii si a culorii;
    }
  }