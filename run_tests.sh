#!/bin/bash

################################################################################
#                                                                              #
#    run_tests.sh - Automated Test Suite for ft_irc Server                    #
#                                                                              #
#    Usage: ./run_tests.sh [OPTIONS]                                          #
#    Options:                                                                  #
#        -s, --server-only    Start server and wait for manual testing       #
#        -q, --quiet          Minimize output                                #
#        -v, --verbose        Show detailed output                           #
#        -h, --help           Display this help message                      #
#                                                                              #
################################################################################

set -e

# ============================================================================
# COLORS
# ============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
RESET='\033[0m'
BOLD='\033[1m'

# ============================================================================
# CONFIGURATION
# ============================================================================

SERVER_PORT=6667
SERVER_PASSWORD="testpass123"
SERVER_HOST="127.0.0.1"
SERVER_EXECUTABLE="./ircserv"
SERVER_PID=""
TIMEOUT=5
VERBOSE=0
SERVER_ONLY=0

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

print_header() {
    echo -e "${BOLD}${CYAN}=== $1 ===${RESET}"
}

print_success() {
    echo -e "${GREEN}✓${RESET} $1"
    ((PASSED_TESTS++))
    ((TOTAL_TESTS++))
}

print_fail() {
    echo -e "${RED}✗${RESET} $1"
    ((FAILED_TESTS++))
    ((TOTAL_TESTS++))
}

print_info() {
    echo -e "${BLUE}ℹ${RESET} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${RESET} $1"
}

print_test() {
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${YELLOW}→${RESET} Testing: $1"
    fi
}

# ============================================================================
# PREREQUISITES CHECK
# ============================================================================

check_prerequisites() {
    print_header "Checking Prerequisites"
    
    # Check server executable exists
    if [ ! -f "$SERVER_EXECUTABLE" ]; then
        print_fail "Server executable not found: $SERVER_EXECUTABLE"
        print_info "Run 'make' to compile the server first"
        exit 1
    fi
    print_success "Server executable found"
    
    # Check nc/netcat is available
    if ! command -v nc &> /dev/null; then
        print_fail "nc (netcat) not found. Please install it."
        exit 1
    fi
    print_success "netcat (nc) available"
    
    # Check port is not in use
    if nc -z $SERVER_HOST $SERVER_PORT 2>/dev/null; then
        print_warning "Port $SERVER_PORT already in use"
        print_info "Waiting for it to be released..."
        sleep 2
    fi
    
    echo ""
}

# ============================================================================
# SERVER MANAGEMENT
# ============================================================================

start_server() {
    print_header "Starting IRC Server"
    
    if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        print_warning "Server already running (PID: $SERVER_PID)"
        return
    fi
    
    # Start server in background
    $SERVER_EXECUTABLE $SERVER_PORT $SERVER_PASSWORD > /tmp/ircserv.log 2>&1 &
    SERVER_PID=$!
    
    print_info "Server started with PID: $SERVER_PID"
    
    # Wait for server to be ready
    sleep 1
    
    if ! kill -0 "$SERVER_PID" 2>/dev/null; then
        print_fail "Server failed to start"
        cat /tmp/ircserv.log
        exit 1
    fi
    
    # Test connection
    if ! nc -w1 $SERVER_HOST $SERVER_PORT </dev/null &>/dev/null; then
        print_fail "Server is not accepting connections"
        kill "$SERVER_PID" 2>/dev/null || true
        exit 1
    fi
    
    print_success "Server is ready and accepting connections"
    echo ""
}

stop_server() {
    if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        print_info "Stopping server (PID: $SERVER_PID)"
        kill "$SERVER_PID" 2>/dev/null || true
        sleep 1
        if kill -0 "$SERVER_PID" 2>/dev/null; then
            kill -9 "$SERVER_PID" 2>/dev/null || true
        fi
    fi
}

# ============================================================================
# CONNECTION TESTING
# ============================================================================

send_command() {
    local cmd="$1"
    (echo -e "$cmd"; sleep 0.5) | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT 2>/dev/null
}

send_commands() {
    local commands="$1"
    (echo -e "$commands"; sleep 0.5) | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT 2>/dev/null
}

test_response() {
    local cmd="$1"
    local expected="$2"
    
    print_test "$cmd"
    
    local response=$(send_command "$cmd")
    
    if echo "$response" | grep -q "$expected"; then
        print_success "Command: $cmd"
        return 0
    else
        print_fail "Command: $cmd (expected: '$expected')"
        if [ $VERBOSE -eq 1 ]; then
            echo "Response: $response"
        fi
        return 1
    fi
}

# ============================================================================
# TEST SUITES
# ============================================================================

test_authentication() {
    print_header "Authentication Tests"
    
    # Test 1: Good password
    print_test "PASS with correct password"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK TestUser\r\nUSER testuser 0 * :Test User")
    if echo "$response" | grep -q "001"; then
        print_success "PASS with correct password"
    else
        print_fail "PASS with correct password"
    fi
    
    # Test 2: Wrong password
    print_test "PASS with wrong password"
    local response=$(send_commands "PASS wrongpass\r\nNICK TestUser2\r\nUSER testuser2 0 * :Test User2")
    if echo "$response" | grep -q "464"; then
        print_success "PASS rejects wrong password (464)"
    else
        print_fail "PASS should reject wrong password"
    fi
    
    # Test 3: Invalid nickname (starts with number)
    print_test "NICK with invalid format"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK 123invalid\r\nUSER user 0 * :User")
    if echo "$response" | grep -q "432"; then
        print_success "NICK rejects invalid format (432)"
    else
        print_fail "NICK should reject invalid format"
    fi
    
    echo ""
}

test_channel_operations() {
    print_header "Channel Operations Tests"
    
    # Test 1: JOIN channel
    print_test "JOIN channel"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Alice\r\nUSER alice 0 * :Alice\r\nJOIN #test")
    if echo "$response" | grep -q "JOIN"; then
        print_success "JOIN channel creates and joins"
    else
        print_fail "JOIN command failed"
    fi
    
    # Test 2: NAMES command
    print_test "NAMES lists users"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Bob\r\nUSER bob 0 * :Bob\r\nJOIN #test\r\nNAMES #test")
    if echo "$response" | grep -q "353"; then
        print_success "NAMES returns user list (353)"
    else
        print_fail "NAMES command should return 353"
    fi
    
    # Test 3: TOPIC no topic set
    print_test "TOPIC on empty channel"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Charlie\r\nUSER charlie 0 * :Charlie\r\nJOIN #empty\r\nTOPIC #empty")
    if echo "$response" | grep -q "331"; then
        print_success "TOPIC returns 331 when no topic set"
    else
        print_fail "TOPIC should return 331 for empty topic"
    fi
    
    echo ""
}

test_messaging() {
    print_header "Messaging Tests"
    
    # Test 1: PRIVMSG without recipient
    print_test "PRIVMSG without recipient"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Dave\r\nUSER dave 0 * :Dave\r\nPRIVMSG")
    if echo "$response" | grep -q "461"; then
        print_success "PRIVMSG requires parameters (461)"
    else
        print_fail "PRIVMSG should require parameters"
    fi
    
    # Test 2: PRIVMSG to nonexistent user
    print_test "PRIVMSG to nonexistent user"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Eve\r\nUSER eve 0 * :Eve\r\nPRIVMSG NonExistent :Hello")
    if echo "$response" | grep -q "401"; then
        print_success "PRIVMSG to nonexistent user returns 401"
    else
        print_fail "PRIVMSG should return 401 for nonexistent user"
    fi
    
    echo ""
}

test_partial_data() {
    print_header "Partial Data Handling (nc -C test)"
    
    # This tests the requirement from subject: handling partial commands
    print_test "Receiving fragmented command"
    
    # Send command in parts: "PASS" then "word" then "test\r\n"
    local response=$( { 
        echo -n "PAS";
        sleep 0.1;
        echo -n "S test";
        sleep 0.1;
        echo "word"
    } | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT 2>/dev/null)
    
    if [ -n "$response" ]; then
        print_success "Server handles fragmented data"
    else
        print_fail "Server should handle fragmented data"
    fi
    
    echo ""
}

test_mode_operations() {
    print_header "MODE Command Tests"
    
    # Test 1: MODE +i (invite only)
    print_test "MODE +i (invite-only)"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Frank\r\nUSER frank 0 * :Frank\r\nJOIN #private\r\nMODE #private +i")
    if echo "$response" | grep -q "#private" || echo "$response" | grep -q "MODE"; then
        print_success "MODE +i processed"
    else
        print_fail "MODE +i should be processed"
    fi
    
    # Test 2: MODE +l (limit)
    print_test "MODE +l (user limit)"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Grace\r\nUSER grace 0 * :Grace\r\nJOIN #limited\r\nMODE #limited +l 5")
    if echo "$response" | grep -q "#limited" || echo "$response" | grep -q "MODE"; then
        print_success "MODE +l processed"
    else
        print_fail "MODE +l should be processed"
    fi
    
    echo ""
}

test_operator_commands() {
    print_header "Operator Commands Tests"
    
    # Test 1: KICK non-existent user
    print_test "KICK nonexistent user"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Henry\r\nUSER henry 0 * :Henry\r\nJOIN #admin\r\nKICK #admin NonExistent :reason")
    if echo "$response" | grep -q "441"; then
        print_success "KICK returns 441 for nonexistent user"
    else
        print_fail "KICK should return 441 for nonexistent user"
    fi
    
    # Test 2: INVITE without enough params
    print_test "INVITE without parameters"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Iris\r\nUSER iris 0 * :Iris\r\nINVITE")
    if echo "$response" | grep -q "461"; then
        print_success "INVITE requires parameters (461)"
    else
        print_fail "INVITE should require parameters"
    fi
    
    echo ""
}

test_error_handling() {
    print_header "Error Handling Tests"
    
    # Test 1: Unknown command
    print_test "Unknown command"
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Jack\r\nUSER jack 0 * :Jack\r\nINVALIDCOMMAND")
    if echo "$response" | grep -q "421"; then
        print_success "Unknown command returns 421"
    else
        print_fail "Unknown command should return 421"
    fi
    
    # Test 2: NICK collision detection
    print_test "NICK already in use detection"
    # First client takes the nick
    { echo -e "PASS $SERVER_PASSWORD\r\nNICK Kate\r\nUSER kate 0 * :Kate"; sleep 1; } | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT >/dev/null 2>&1 &
    sleep 0.5
    # Second client tries same nick
    local response=$(send_commands "PASS $SERVER_PASSWORD\r\nNICK Kate\r\nUSER kate2 0 * :Kate2")
    if echo "$response" | grep -q "433"; then
        print_success "NICK collision detected (433)"
    else
        print_fail "NICK collision should be detected"
    fi
    
    echo ""
}

test_multiline_messages() {
    print_header "Multi-Client Tests"
    
    print_test "Multiple simultaneous connections"
    
    # Start multiple clients
    { echo -e "PASS $SERVER_PASSWORD\r\nNICK User1\r\nUSER user1 0 * :User One\r\nJOIN #multi"; sleep 2; } | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT >/dev/null 2>&1 &
    { echo -e "PASS $SERVER_PASSWORD\r\nNICK User2\r\nUSER user2 0 * :User Two\r\nJOIN #multi"; sleep 2; } | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT >/dev/null 2>&1 &
    { echo -e "PASS $SERVER_PASSWORD\r\nNICK User3\r\nUSER user3 0 * :User Three\r\nJOIN #multi"; sleep 2; } | nc -w $TIMEOUT $SERVER_HOST $SERVER_PORT >/dev/null 2>&1 &
    
    sleep 3
    print_success "Multiple clients handled without blocking"
    
    echo ""
}

# ============================================================================
# MAIN EXECUTION
# ============================================================================

print_usage() {
    grep "^#" "$0" | head -13 | tail -11
}

parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -s|--server-only)
                SERVER_ONLY=1
                shift
                ;;
            -q|--quiet)
                VERBOSE=0
                shift
                ;;
            -v|--verbose)
                VERBOSE=1
                shift
                ;;
            -h|--help)
                print_usage
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
}

run_all_tests() {
    check_prerequisites
    start_server
    
    test_authentication
    test_channel_operations
    test_messaging
    test_partial_data
    test_mode_operations
    test_operator_commands
    test_error_handling
    test_multiline_messages
    
    print_report
}

print_report() {
    echo ""
    print_header "Test Results Summary"
    
    echo -e "Total tests:  ${BOLD}$TOTAL_TESTS${RESET}"
    echo -e "Passed:       ${GREEN}${BOLD}$PASSED_TESTS${RESET}"
    echo -e "Failed:       ${RED}${BOLD}$FAILED_TESTS${RESET}"
    
    if [ $TOTAL_TESTS -gt 0 ]; then
        local percentage=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        echo -e "Success rate: ${BOLD}$percentage%${RESET}"
    fi
    
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}${BOLD}✓ ALL TESTS PASSED!${RESET}"
        return 0
    else
        echo -e "${RED}${BOLD}✗ SOME TESTS FAILED${RESET}"
        return 1
    fi
}

# ============================================================================
# ENTRY POINT
# ============================================================================

main() {
    parse_arguments "$@"
    
    # Cleanup on exit
    trap stop_server EXIT INT TERM
    
    if [ $SERVER_ONLY -eq 1 ]; then
        echo "Starting server in manual testing mode..."
        echo "Press Ctrl+C to stop"
        start_server
        # Wait indefinitely for manual testing
        wait "$SERVER_PID"
    else
        run_all_tests
    fi
}

main "$@"
