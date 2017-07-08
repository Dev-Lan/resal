#include <iostream>
#include "App.hpp"
#include "G3D/G3DALL.h"
// Tells C++ to invoke command-line main() function even on OS X and Win32.
// G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
  (void)argc; (void)argv;
  GApp::Settings settings(argc, argv);
  
  // Change the window and other startup parameters by modifying the
  // settings class.  For example:
  settings.window.width       = 1024; 
  settings.window.height      = 768;
  settings.window.resizable = true;
  //settings.film.enabled = false;
  


  return App(settings).run();
  // int numSegments = 8;
  // for (int i=0; i < numSegments; i++) {
  // 	float percent = (float) i / (float) numSegments;
  // 	float angle = percent * twoPi();
  // 	float x = sin(angle);
  // 	float y = cos(angle);
  // 	std::cout << "i = " << i << std::endl;
  // 	std::cout << "(x,y) : angle = (" << x << "," << y << ") : " << angle << std::endl;
  // 	float calcAngle = atan(y / x);
  // 	if (y < 0) {
  // 		calcAngle += pi();
  // 	}
  // 	 else if (x < 0) {
  // 		calcAngle += twoPi();
  // 	}
  // 	std::cout << "calcAngle = " << calcAngle << std::endl;
  // }


  return 0;
}

