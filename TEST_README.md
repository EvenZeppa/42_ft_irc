# Test Suite Quick Start Guide

## Running Tests

### Quick Test
```bash
make test
```

This will:
1. Compile all test files
2. Link them with the implementation
3. Run the complete test suite
4. Display results

### Manual Build and Run
```bash
# Build test runner
make test_runner

# Run tests
./test_runner

# Save output to file
./test_runner > test_results.txt 2>&1
```

### Clean and Rebuild
```bash
# Clean everything and rebuild tests
make retest

# Clean all build artifacts
make fclean
```

## Test Structure

```
test/
├── main_test.cpp           # Test runner entry point
├── integration_test.cpp    # Integration tests
├── client/
│   └── client_test.cpp     # Client unit tests (49 tests)
├── channel/
│   └── channel_test.cpp    # Channel unit tests (52 tests)
└── server/
    └── server_test.cpp     # Server unit tests (36 tests)
```

## Test Categories

### Client Tests (49)
- Constructors
- Getters/Setters  
- Registration flow (PASS/NICK/USER)
- Modes
- Buffers
- Channel membership
- Edge cases

### Channel Tests (52)
- Constructors
- Getters/Setters
- Modes (+i, +t, +k, +l)
- Members, operators, invited
- Topic management
- Edge cases

### Server Tests (36)
- Constructors
- Getters/Setters
- Client CRUD operations
- Channel CRUD operations
- Integration scenarios
- Edge cases

### Integration Tests (15)
- Server-Client integration
- Server-Channel integration
- Client-Channel integration
- Full system workflows

## Expected Output

All tests passing shows:
```
╔════════════════════════════════════════╗
║     IRC SERVER TEST SUITE              ║
║  Testing Server, Client, and Channel  ║
╚════════════════════════════════════════╝

[... test execution ...]

╔════════════════════════════════════════╗
║  ✓ ALL TESTS PASSED SUCCESSFULLY!     ║
╚════════════════════════════════════════╝
```

## Troubleshooting

### Tests Won't Compile
- Ensure C++98 compliance: `c++ -std=c++98`
- Check for missing implementation files
- Verify all headers are accessible

### Tests Fail
- Check test output for specific assertion failures
- Review TEST_SUMMARY.md for test descriptions
- Ensure implementation matches expected behavior

### Tests Hang
- Check for infinite loops in implementation
- Verify file descriptor handling
- Use `timeout` command: `timeout 30 ./test_runner`

## Test Metrics

- **Total Tests**: 152
- **Lines of Test Code**: ~2000+
- **Test Execution Time**: < 10 seconds
- **Code Coverage**: All public methods

## Continuous Integration

Add to CI pipeline:
```yaml
- name: Run Tests
  run: |
    cd irc
    make test
```

## Documentation

See `TEST_SUMMARY.md` for comprehensive test documentation.
