#include "vulkanengine.h"
#include <iostream>


int main() {

#ifdef  NDEBUG
	const bool enableValidationLayer = false;
#else
	const bool enableValidationLayer = true;
#endif //  NDEBUG

	VE::vulkanEngine engine(enableValidationLayer);

	try {
		engine.run();
	} 
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}