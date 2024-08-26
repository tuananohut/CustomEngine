#include <iostream>
#include <fstream>

using namespace std;


typedef struct
{
  float x;
  float y;
  float z;
}VertexType;

typedef struct
{
  int v_index1, v_index2, v_index3;
  int t_index1, t_index2, t_index3;
  int n_index1, n_index2, n_index3;
}FaceType;


void GetModelFilename(char*);
bool RenderFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);


int main()
{
  bool result;
  char filename[256];
  int vertexCount, textureCount, normalCount, faceCount;
  char garbage;

  GetModelFilename(filename);

  result = RenderFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
  if (!result)
    {
      return -1;
    }

  cout << endl;
  cout << "Vertices: " << vertexCount << endl;
  cout << "UVs: " << textureCount << endl;
  cout << "Normals: " << normalCount << endl;
  cout << "Faces: " << faceCount << endl;

  result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
  if (!result)
    {
      return -1;
    }

  cout << "\nFile has been converted." << endl;
  cout << "\nDo you wish to exit (y/n)?";
  cin >> garbage;

  return 0;
}

void GetModelFilename(char* filename)
{
  bool done;
  ifstream fin;

  done = false;
  while(!done)
    {
      cout << "Enter model filename: ";
      cin >> filename;

      fin.open(filename);

      if(fin.good())
	{
	  done = true;
	}

      else
	{
	  fin.clear();
	  cout << endl;
	  cout << "File " << filename << " could not be opened." << endl << endl;
	}
    }
}

bool RenderFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
  ifstream fin;
  char input;

  vertexCount = 0;
  textureCount = 0;
  normalCount = 0;
  faceCount = 0;

  fin.open(filename);
  if (fin.fail() == true)
    {
      return false;
    }

  fin.get(input);
  while (!fin.eof())
    {
      if (input == 'v')
	{
	  fin.get(input);

	  if (input == ' ')
	    {
	      vertexCount++;
	    }

	  if (input == 't')
	    {
	      textureCount++;
	    }

	  if (input == 'n')
	    {
	      normalCount++;
	    }
	}

      if (input == 'f')
	{
	  fin.get(input);
	  if (input == ' ')
	    {
	      faceCount++;
	    }
	}

      while (input != '\n')
	{
	  fin.get(input);
	}

      fin.get(input);
    }  

  fin.close();

  return true;
}

bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
  VertexType *vertices, *texcoords, *normals;
  FaceType *faces;
  ifstream fin;
  int vertexIndex, texcoordIndex, normalIndex, faceIndex, vindex, tindex, nindex;
  char input, input2;
  ofstream fout;

  vertices = new VertexType[vertexCount];
  texcoords = new VertexType[textureCount];
  normals = new VertexType[normalCount];
  faces = new FaceType[faceCount];

  vertexIndex = 0;
  texcoordIndex = 0;
  normalIndex = 0;
  faceIndex = 0;

  fin.open(filename);
  if (fin.fail() == true)
    {
      return false;
    }

  fin.get(input);
    while (!fin.eof())
    {
      if (input == 'v')
	{
	  fin.get(input);

	  if (input == ' ')
	    {
	      fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

	      vertices[vertexIndex].z = vertices[vertexIndex].z * -1.f;
	      vertexIndex++;
	    }

	  if (input == 't')
	    {
	      fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

	      texcoords[texcoordIndex].y = 1.f - texcoords[texcoordIndex].y;
	      texcoordIndex++;
	    }

	  if (input == 'n')
	    {
	      fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

	      normals[normalIndex].z = normals[normalIndex].z * -1.f;
	      normalIndex++;
	    }
	}

      if (input == 'f')
	{
	  fin.get(input);
	  if (input == ' ')
	    {
	      fin >> faces[faceIndex].v_index3 >> input2 >> faces[faceIndex].t_index3 >> input2 >> faces[faceIndex].n_index3
		  >> faces[faceIndex].v_index2 >> input2 >> faces[faceIndex].t_index2 >> input2 >> faces[faceIndex].n_index2
		  >> faces[faceIndex].v_index1 >> input2 >> faces[faceIndex].t_index1 >> input2 >> faces[faceIndex].n_index1;
	      faceIndex++;
	    }
	}

      while (input != '\n')
	{
	  fin.get(input);
	}

      fin.get(input);
    }

    fin.close();

    fout.open("model.txt");

    fout << "Vertex Count: " << (faceCount * 3) << endl;
    fout << endl;
    fout << "Data: " << endl;
    fout << endl;

    for (int i = 0; i < faceIndex; i++)
      {
	vindex = faces[i].v_index1 - 1;
	tindex = faces[i].t_index1 - 1;
	nindex = faces[i].n_index1 - 1;

	fout << vertices[vindex].x << ' ' <<  vertices[vindex].y << ' ' <<  vertices[vindex].z << ' '
	     << texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
	     << normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << endl;

	vindex = faces[i].v_index2 - 1;
	tindex = faces[i].t_index2 - 1;
	nindex = faces[i].n_index2 - 1;

	fout << vertices[vindex].x << ' ' <<  vertices[vindex].y << ' ' <<  vertices[vindex].z << ' '
	     << texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
	     << normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << endl;

	vindex = faces[i].v_index3 - 1;
	tindex = faces[i].t_index3 - 1;
	nindex = faces[i].n_index3 - 1;

	fout << vertices[vindex].x << ' ' <<  vertices[vindex].y << ' ' <<  vertices[vindex].z << ' '
	     << texcoords[tindex].x << ' ' << texcoords[tindex].y << ' '
	     << normals[nindex].x << ' ' << normals[nindex].y << ' ' << normals[nindex].z << endl;	
      }

    fout.close();

    if (vertices)
      {
	delete[] vertices;
	vertices = nullptr;
      }

    if (texcoords)
      {
	delete[] texcoords;
	texcoords = nullptr;
      }

    if (normals)
      {
	delete[] normals;
	normals = nullptr;
      }

    if (faces)
      {
	delete[] faces;
	faces = nullptr;
      }

    return true;
}
