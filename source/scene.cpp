#include "../include/Scene/scene.h"
#include "../OBJ-Loader/OBJ_Loader.h"

using namespace std;


Sphere spheres[] = 
			{
				Sphere(1.5, Vector(50, 81.6 - 16.5, 81.6), Vector(4, 4, 4) * 100, Vector(), DIFF,1), /* Light */
				Sphere(16.5, Vector(40, 16.5, 47), Vector(), Vector(1, 1, 1) * .999, SPEC,1), /* Mirror sphere */
				//Sphere(10.0, Vector(45, 10.0, 105), Vector(), Vector(1, 1, 1) * .999, TRAN,1,1,1,1), /* Translucent sphere */
				//Sphere(10.0, Vector(20.0, 50.0, 90.0), Vector(), Vector(1, 1, 1) * .999, GLOS,1,1,1,1), /* Glossy sphere */
				Sphere(12, Vector(80, 12, 80), Vector(), Color(0.75, 0.75, 0.75), REFR,1.52), /* Glas sphere */


				Sphere(1e5,     Vector(1e5 + 1.0,     40.8,          81.6), Vector(0, 0, 0), Color(0.25,  0.75,  0.25), DIFF, 0),
				Sphere(1e5,     Vector(-1e5 + 99.0,   40.8,          81.6), Vector(0, 0, 0), Color(0.25,  0.25,  0.75), DIFF, 0),
				Sphere(1e5,     Vector(50.0,          40.8,           1e5), Vector(0, 0, 0), Color(0.75,  0.75,  0.75), DIFF, 0),
				Sphere(1e5,     Vector(50.0,          40.8,  -1e5 + 170.0), Vector(0, 0, 0), Color(0.01,  0.01,  0.01), DIFF, 0),
				Sphere(1e5,     Vector(50.0,           1e5,          81.6), Vector(0, 0, 0), Color(0.75,  0.75,  0.75), DIFF, 0),
				Sphere(1e5,     Vector(50.0,   -1e5 + 81.6,          81.6), Vector(0, 0, 0), Color(0.75,  0.75,  0.75), DIFF, 0),
			};

scene :: scene()
    {
		surfaces.clear();
		lights.clear();
		const int n = int(sizeof(spheres) / sizeof(Sphere));
		for (int i = 0; i < n; i++)
		{
			add_sphere(&spheres[i]);
		}
	}

scene :: scene (const char* filename, Vector translation, double scale) 
    {
		surfaces.clear();
		lights.clear();
	}

scene :: ~scene () 
    {
		
	}

bool scene :: intersect(const Ray &ray, double &t, int &id) 
    {
        t = 1e20;
        for (int i = 0; i < (int)surfaces.size(); i++) {
            double d = surfaces[i]->intersect(ray);
            if (d > 0.0 && d < t) {
                t = d;
                id = i;
            }
        }

        return t < 1e20;
    }

void scene :: add_sphere(Sphere* sphere) 
    {
		if (sphere->emission.max() >= 1.0) {
			lights.push_back(sphere);
			emission = emission + sphere->emission;
		}
		surfaces.push_back(sphere);
	}


void scene :: add_triangle(Triangle* triangle) 
    {
		surfaces.push_back(triangle);
	}

void scene :: add_OBJ_file(const char* filename, Vector translation, double scale) {
		objl::Loader Loader;

		// Load .obj File
		bool loadout = Loader.LoadFile(filename);
		if (!loadout) {
			std::cout << "Can not open OBJ file correctly or is not in the correct directory" << std::endl;
		}
		else {
			// Go through each loaded mesh and out its contents
			for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
			{
				// Copy one of the loaded meshes to be our current mesh
				objl::Mesh curMesh = Loader.LoadedMeshes[i];

				// Print Mesh Name
				std::cout << "Mesh " << i+1 << ": " << curMesh.MeshName << "\n";

				// Go through every 3rd index and construct the
				// triangle that these indices represent
				for (unsigned int j = 0; j < curMesh.Indices.size(); j += 3)
				{
					
					Triangle* tris = new Triangle(
						Vector( // vertex p0
							curMesh.Vertices[curMesh.Indices[j]].Position.X,
							curMesh.Vertices[curMesh.Indices[j]].Position.Y,
							curMesh.Vertices[curMesh.Indices[j]].Position.Z) * scale + translation,

						Vector( // vertex p1
							curMesh.Vertices[curMesh.Indices[j + 1]].Position.X,
							curMesh.Vertices[curMesh.Indices[j + 1]].Position.Y,
							curMesh.Vertices[curMesh.Indices[j + 1]].Position.Z) * scale + translation,

						Vector( // vertex p2
							curMesh.Vertices[curMesh.Indices[j + 2]].Position.X,
							curMesh.Vertices[curMesh.Indices[j + 2]].Position.Y,
							curMesh.Vertices[curMesh.Indices[j + 2]].Position.Z) * scale + translation,

						Vector(), // emission 

						Vector(1, 1, 1) * .999, // transparent water
						//Vector(0.6118, 0.8275, 0.8588), // blueish water
						1.33,
						REFR
					);
					add_triangle(tris);
				}
			}
			std::cout << "Surfaces in scene: " << surfaces.size() << std::endl;
		}
	}

