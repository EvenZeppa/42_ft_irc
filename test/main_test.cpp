#include <iostream>

// Forward declarations for test functions
int run_client_tests();
int run_channel_tests();
int run_server_tests();
int run_integration_tests();

int main() {
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║     IRC SERVER TEST SUITE              ║" << std::endl;
	std::cout << "║  Testing Server, Client, and Channel  ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;
	
	int failed = 0;
	
	try {
		// Run unit tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "         UNIT TESTS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		
		if (run_client_tests() != 0) failed++;
		if (run_channel_tests() != 0) failed++;
		if (run_server_tests() != 0) failed++;
		
		// Run integration tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "         INTEGRATION TESTS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		
		if (run_integration_tests() != 0) failed++;
		
		std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
		if (failed == 0) {
			std::cout << "║  ✓ ALL TESTS PASSED SUCCESSFULLY!     ║" << std::endl;
		} else {
			std::cout << "║  ✗ SOME TESTS FAILED!                 ║" << std::endl;
		}
		std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
		
		return failed;
	} catch (const std::exception& e) {
		std::cerr << "\n✗ TEST FAILED WITH EXCEPTION: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "\n✗ TEST FAILED WITH UNKNOWN EXCEPTION" << std::endl;
		return 1;
	}
}
