# ft_irc

*This project has been created as part of the 42 curriculum by ezeppa and vbonnard.*

---

## Description

ft_irc is an IRC (Internet Relay Chat) server written in C++98, compliant with the IRC protocol as defined in RFC 1459. The server allows multiple clients to connect simultaneously, authenticate with a password, exchange private messages, and create or join channels with full operator privilege management.

The goal of this project is to learn network programming with TCP/IP sockets, non-blocking I/O multiplexing using `epoll`, and the implementation of a real-world application-layer protocol.

### Features

**Authentication & Registration**
- `PASS` — Server password authentication
- `NICK` — Set or change nickname
- `USER` — Register user information
- `CAP` — Capability negotiation (modern client compatibility)
- `QUIT` — Clean disconnection

**Channel Management**
- `JOIN` — Join a channel (creates it if it doesn't exist; first user becomes operator)
- `PART` — Leave a channel
- `TOPIC` — View or set the channel topic
- `NAMES` — List channel members

**Channel Modes**
- `+i` — Invite-only channel
- `+t` — Only operators can change the topic
- `+k` — Set a channel password (key)
- `+l` — Set a user limit on the channel
- `+o` — Grant or revoke operator privileges

**Operator Commands**
- `KICK` — Remove a user from a channel
- `INVITE` — Invite a user to an invite-only channel

**Messaging**
- `PRIVMSG` — Send messages to users or channels

**Utilities**
- `PING/PONG` — Connection keepalive
- `WHO` — List users

### Technical Choices

- **C++98** strict compliance (compiled with `-Wall -Wextra -Werror -std=c++98`)
- **Non-blocking I/O** using Linux native `epoll`
- **BNF Parser** integrated for strict IRC protocol validation per RFC 1459 grammar
- **Standard IRC numeric replies** for all responses and errors

### Project Structure

```
ft_irc/
├── include/              # Header files
│   ├── Server.hpp        # Main server class
│   ├── Client.hpp        # Client state management
│   ├── Channel.hpp       # Channel management
│   ├── CommandManager.hpp# Command dispatch
│   ├── Logger.hpp        # Logging system
│   ├── network/
│   │   └── IrcReplies.hpp# IRC numeric reply codes
│   └── bnf/              # BNF Parser headers
├── src/                  # Implementation
│   ├── main.cpp          # Entry point
│   ├── Server.cpp        # Server logic (epoll loop, client handling)
│   ├── Client.cpp        # Client logic
│   ├── Channel.cpp       # Channel logic
│   ├── Logger.cpp        # Logging
│   ├── commands/         # Command handlers
│   │   ├── cmd_auth.cpp      # PASS, NICK, USER, CAP, QUIT, WHO, PING, PONG
│   │   ├── cmd_channel.cpp   # JOIN, PART, TOPIC, NAMES
│   │   ├── cmd_mode.cpp      # MODE (user and channel)
│   │   ├── cmd_ops.cpp       # KICK, INVITE
│   │   └── cmd_privmsg.cpp   # PRIVMSG
│   └── bnf/              # BNF Parser sources
└── Makefile              # Build rules (all, clean, fclean, re)
```

---

## Instructions

### Compilation

```bash
make        # Build the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Full rebuild
```

**Requirements:**
- C++ compiler with C++98 support (`c++`, `g++`, or `clang++`)
- `make`
- Linux (uses native `epoll` for I/O multiplexing)

### Execution

```bash
./ircserv <port> <password>
```

- `<port>` — TCP port to listen on (e.g. `6667`)
- `<password>` — Password required for client authentication

**Example:**
```bash
./ircserv 6667 mypassword
```

### Connecting with irssi (reference client)

```bash
irssi
```

Then inside irssi:
```
/connect 127.0.0.1 6667 mypassword
/nick YourNick
/join #channel
/msg #channel Hello everyone!
```

See `MANUAL_TESTS.md` for a complete manual testing guide with irssi.

### Connecting with nc (raw protocol testing)

```bash
nc 127.0.0.1 6667
```

Then type:
```
PASS mypassword
NICK YourNick
USER yourname 0 * :Your Real Name
JOIN #channel
PRIVMSG #channel :Hello everyone!
```

> The server is compatible with any RFC 1459-compliant IRC client (irssi, WeeChat, HexChat, etc.).

### Server Console

The server provides an interactive console for monitoring and management:

| Command | Description |
|---------|-------------|
| `help` | Display available commands |
| `quit` | Gracefully shut down the server |
| `clear` | Clear the terminal |
| `clients` | List all connected clients |
| `channels` | List all active channels |
| `client <fd\|nick>` | Show details for a specific client |
| `log show` | Display current log filter state |
| `log all` / `log none` | Enable / disable all log types |
| `log <type> <on\|off\|toggle>` | Toggle a specific log type (in, out, info, error) |

### Automated Tests

```bash
./run_tests.sh      # Run all tests
./run_tests.sh -v   # Verbose output
./run_tests.sh -s   # Start server only (for manual testing)
```

---

## Resources

### IRC Protocol
- [RFC 1459 — Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459) — Original IRC specification
- [RFC 2812 — IRC Client Protocol](https://tools.ietf.org/html/rfc2812) — Updated client protocol
- [Modern IRC Documentation](https://modern.ircdocs.horse/) — Contemporary reference

### Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — TCP/IP sockets tutorial
- [epoll(7) man page](https://man7.org/linux/man-pages/man7/epoll.7.html) — Linux I/O multiplexing

### Reference Client
- **irssi** — Terminal IRC client used during 42 evaluation

### AI Usage

AI (GitHub Copilot) was used as a productivity tool for the following tasks:

- **Documentation**: README, test documentation, and project analysis
- **Code review**: Identifying potential issues and suggesting structural improvements
- **Test scenarios**: Brainstorming edge cases and writing the automated test script
- **Error formatting**: IRC numeric reply messages and error handling patterns

The following parts were developed **without AI assistance**:
- Core server architecture and design
- IRC protocol implementation (all command handlers)
- `epoll` event loop and non-blocking I/O management
- Client and channel state management
- BNF parser integration and IRC grammar definition

All AI-generated content was reviewed, tested, and adapted to project requirements. Every component can be explained in detail during evaluation.

---

## Authors

- **ezeppa** — [ezeppa@student.42.fr](mailto:ezeppa@student.42.fr)
- **vbonnard** — [vbonnard@student.42.fr](mailto:vbonnard@student.42.fr)
