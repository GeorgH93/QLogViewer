# QLogViewer

A Qt6-based log file viewer with profile-based parsing and filtering capabilities.

## Features

- **Profile-based parsing**: Automatically detects log formats using configurable regex patterns
- **Multi-format support**: Handles various log formats via YAML profile definitions
- **Async file loading**: Non-blocking UI when loading large log files
- **Filtering**: Filter log entries by level, text content, or regex patterns
- **Multi-line message support**: Properly handles log entries spanning multiple lines
- **Tab-based interface**: View multiple log files simultaneously
- **Drag & drop**: Open files by dragging them into the application window

## Requirements

- CMake 3.15+
- C++17 compiler
- Qt6 (Widgets, Concurrent modules)
- yaml-cpp (fetched automatically)

## Building

### Using CMake (Recommended)

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Using XMake (Alternative)

```bash
xmake build
```

### Running

```bash
# CMake
./build/QLogViewer

# XMake
xmake run
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Usage

1. **Open a log file**: Use `File → Open` or drag and drop files into the window
2. **View logs**: The main view shows filtered entries; the bottom panel shows the full log
3. **Navigate**: Click in the filtered view to highlight the corresponding line in the full view
4. **Multiple files**: Open multiple files in tabs via `File → Open` or drag multiple files

## Log Profiles

Log profiles are YAML files that define how to parse log files. They are stored in the application data directory.

### Profile Structure

```yaml
Name: MyLogProfile
Priority: 1
DetectionRegex: "^\\d{4}-\\d{2}-\\d{2}"
DetectionRange: 10
Entries.Regex: "^(?<date>\\d{4}-\\d{2}-\\d{2})\\s+(?<time>\\d{2}:\\d{2}:\\d{2})\\s+(?<level>\\w+)\\s+(?<message>.*)"
Entries.NewEntryStartRegex: "^\\d{4}-\\d{2}-\\d{2}"
SystemInfo.VersionRegex: "Version:\\s*(?<version>[\\d.]+)"
SystemInfo.DeviceRegex: "Device:\\s*(?<device>.+)"
SystemInfo.OsRegex: "OS:\\s*(?<os>.+)"
SystemInfo.LinesToCheck: 50
LogLevels:
  - Name: ERROR
    FontColor: "#FF0000"
    BackgroundColor: "#FFE0E0"
  - Name: WARN
    FontColor: "#FF8800"
  - Name: INFO
    FontColor: "#008800"
  - Name: DEBUG
    FontColor: "#888888"
```

### Supported Regex Capture Groups

- `date` - Date portion of the log entry
- `time` - Time portion of the log entry
- `level` - Log level (e.g., ERROR, WARN, INFO, DEBUG)
- `message` - The main log message content
- `thread` - Thread identifier (optional)
- `subsys` - Subsystem/module name (optional)
- `where` - Source location (function, line number, etc.)

## Filter Presets

Filters can be saved as presets in log profiles:

```yaml
FilterPresets:
  - Name: Errors Only
    Enabled: true
    Mode: Include
    IncludedLevels:
      - ERROR
  - Name: Exclude Debug
    Mode: Exclude
    IncludedLevels:
      - DEBUG
    SearchText: "verbose"
    CaseSensitive: false
```

## License

GNU General Public License v3.0 or later. See individual source files for license headers.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests: `ctest --output-on-failure`
5. Submit a pull request
