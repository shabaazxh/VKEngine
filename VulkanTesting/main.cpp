#include "vulkanengine.h"
#include <iostream>


int main() {

	VE::vulkanEngine engine(true);

	try {
		engine.run();
	} 
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}