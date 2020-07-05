#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory> 
#include <vector>
#include <fenv.h>
#include <chrono>

#include "../include/Math/Timer.h"
#include "../include/Ray_Tracer/image.h"
#include "../include/Photon_Map/photon_map.h"
#include "../include/Scene/scene.h"
#include "../include/Ray_Tracer/ray_tracer.h"
#include "../include/Math/math_calc.h"

using namespace std;

/******************************************************************
* Main routine: Computation of the image (2x2 subpixels)
* Key parameters
* - Image dimensions: width, height 
* - Number of samples per subpixel
* - Number of photons in caustics photon map
* - Number of photons in global photon map
* Rendered result saved as PPM image file
*******************************************************************/

int main() {
    //enable catching segmentation fault
    feenableexcept(FE_INVALID | FE_OVERFLOW);

    //parameters
    int width = 1024;
    int height = 768;
    int samples = 1;
    int photons = 200 * 100;
	int photons_caustics = 100 * 100000;

    Timer timer = { "Main" };

    scene scene;
    ray_tracer ray_tracer(30, 300, 2, 60, Color(Vector(0,0,0)));

    Vector trans = Vector(50, 5, 100.0);
	double scale = 80.0;
    /* Add given object to the scene */
	scene.add_OBJ_file("Objects/water_smoother.obj", trans, scale);


    /* Set camera origin and viewing direction (negative z direction) */
    Ray camera(Vector(50.0, 52.0, 295.6), Vector(0.0, -0.042612, -1.0).normalized());

    /* Image edge vectors for pixel sampling */
    Vector cx = Vector(width * 0.5135 / height);
    Vector cy = (cx.cross(camera.dir)).normalized() * 0.5135;

	Image img(width, height);
	Random random = Random(123456);
	photon_map photon_map_global;
	photon_map photon_map_caustics;

	std::cout << "Photon map generation ";

	photon_map_global.generate_map(&scene, &photon_map_global, photons, &random, true);
	photon_map_global.build_photon_map();

	photon_map_caustics.generate_map(&scene, &photon_map_caustics, photons_caustics, &random, false);
	photon_map_caustics.build_photon_map();

	std::cout << "successfull!" << std::endl;

	Color accumulated_color;

    /* Loop over image rows */
    for (int y = 0; y < height; y++) {
        /* Loop over row pixels */
        for (int x = 0; x < width; x++) {
            img.setColor(x, y, Color());

            /* 2x2 subsampling per pixel */
            for (int sub_y = 0; sub_y < 2; sub_y++) {
                for (int sub_x = 0; sub_x < 2; sub_x++) {
                    accumulated_color = Color();

                    /* Compute radiance at subpixel using multiple samples */
                    for (int s = 0; s < samples; s++) {
                        const double r1 = 2.0 * random.rand_double();
                        const double r2 = 2.0 * random.rand_double();

                        /* Transform uniform into non-uniform filter samples */
                        double ix;
                        if (r1 < 1.0)
                            ix = sqrt(r1) - 1.0;
                        else
                            ix = 1.0 - sqrt(2.0 - r1);

                        double iy;
                        if (r2 < 1.0)
                            iy = sqrt(r2) - 1.0;
                        else
                            iy = 1.0 - sqrt(2.0 - r2);

                        /* Ray direction into scene from camera through sample subsampling */
                        Vector dir = cx * ((sub_x + x + (0.5 + ix) / 2.0) / width - 0.5) +
                                     cy * ((sub_y + y + (0.5 + iy) / 2.0) / height - 0.5) +
                                     camera.dir;
						
                        /* Extend camera ray to start inside the box */
                        Vector start = camera.org + dir * 130.0;

                        dir = dir.normalized();

						/* monte carlo path tracing */
						// accumulated_color = accumulated_color +
                        //                       ray_tracer.trace_ray(&scene, Ray(start, dir), 0, &random, &photon_map_global , 1, false, false, true)/samples;
						
						/* direct illumination */
						accumulated_color = accumulated_color +
                                               ray_tracer.trace_ray(&scene, Ray(start, dir), 0, &random, &photon_map_global , 1, false, true, false)/samples;

                        // /* global illumination */
                        accumulated_color = accumulated_color +
                                               ray_tracer.trace_ray(&scene, Ray(start, dir), 0, &random, &photon_map_global , 1, false, false, false)/samples;
						/* caustics */
						accumulated_color = accumulated_color +
                                              ray_tracer.trace_ray(&scene, Ray(start, dir), 0, &random, &photon_map_caustics , 1, true, false, false)/samples;

                    }
					/*subsampling*/					
					accumulated_color = accumulated_color.clamp() * 0.25;

					img.addColor(x, y, accumulated_color);
        
                }
            }
        }
        cout << "\rRendering (" << samples * 4 << " spp) " << (100.0 * y / (height - 1)) << "%     ";
    }
    cout << endl;

    img.Save("image.ppm");
}