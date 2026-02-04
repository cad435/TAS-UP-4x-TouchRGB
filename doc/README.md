# OpenKNX Framework Dataflow Documentation

This directory contains detailed diagrams explaining the OpenKNX framework's architecture and dataflow for the TAS-UP-4x-TouchRGB project.

## Diagram Files

1. **[01-Architecture-Layers.md](01-Architecture-Layers.md)**
   - Overall system structure showing all layers from user application to physical KNX bus
   - Shows how components are organized hierarchically

2. **[02-Incoming-Message-Flow.md](02-Incoming-Message-Flow.md)**
   - Complete path of a KNX telegram from the bus to your application modules
   - Shows all protocol layers and decision points

3. **[03-Outgoing-Message-Flow.md](03-Outgoing-Message-Flow.md)**
   - How your module actions become KNX telegrams sent to the bus
   - Shows encoding and wrapping at each protocol layer

4. **[04-Main-Loop-Cycle.md](04-Main-Loop-Cycle.md)**
   - The continuous execution loop on both RP2040 cores
   - Shows module execution order and parallel processing

5. **[05-GroupObject-State-Machine.md](05-GroupObject-State-Machine.md)**
   - State machine showing GroupObject communication flag (ComFlag) transitions
   - Critical for understanding KNX synchronization

6. **[06-Button-Press-Example.md](06-Button-Press-Example.md)**
   - Real-world sequence diagram showing a button press flowing through the system
   - Demonstrates local LED update and KNX transmission happening in parallel

7. **[07-Module-Integration-Pattern.md](07-Module-Integration-Pattern.md)**
   - Class diagram showing how to create custom modules
   - Shows inheritance and the facade pattern

8. **[08-Flash-Memory-Layout.md](08-Flash-Memory-Layout.md)**
   - Organization of flash memory for device data, parameters, and application state
   - Shows ETS configuration storage and runtime data persistence

9. **[09-Dual-Core-Architecture.md](09-Dual-Core-Architecture.md)**
   - RP2040-specific dual-core processing architecture
   - Shows workload split between Core 0 (KNX) and Core 1 (LEDs/sensors)

### XML Structure & OpenKNXproducer Build System

10. **[10-XML-File-Structure.md](10-XML-File-Structure.md)**
   - XML file relationships and dependencies
   - Shows how main XML references external module libraries
   - Generated output files (knxprod.xml, knxprod.h)

11. **[11-Module-Hierarchy.md](11-Module-Hierarchy.md)**
   - Module dependency tree showing all 6 modules
   - Library versions and module types
   - Integration with OpenKNX::Facade

12. **[12-OpenKNXproducer-Build-Pipeline.md](12-OpenKNXproducer-Build-Pipeline.md)**
   - Complete build process from XML to knxprod
   - Template instantiation for multi-channel modules
   - Variable substitution and KO offset calculation

13. **[13-KO-Communication-Objects.md](13-KO-Communication-Objects.md)**
   - KO number space layout (0-650)
   - Module-specific KO ranges
   - Input/Output directions and DPT types

14. **[14-Parameter-Memory-Layout.md](14-Parameter-Memory-Layout.md)**
   - Flash memory organization for TTD module parameters
   - Parameter types and default values
   - Generated firmware header definitions

15. **[15-Data-Flow-LED-Control.md](15-Data-Flow-LED-Control.md)**
   - Sequence diagram: KNX telegram → LED update
   - Shows ETS configuration flow to hardware
   - Incoming data path example

16. **[16-Data-Flow-Touch-Output.md](16-Data-Flow-Touch-Output.md)**
   - Sequence diagram: Touch input → KNX telegram
   - Shows hardware sensing to bus transmission
   - Outgoing data path example

## How to Convert Diagrams to PNG

### Automatic Conversion (Batch Processing)

Two scripts are provided to automatically convert all diagrams to PNG:

**Windows Batch Script:**
```batch
# First, install mermaid-cli globally (one time only)
npm install -g @mermaid-js/mermaid-cli

# Then run the batch script from the doc folder
generate-diagrams.bat
```

**Python Script (Cross-platform):**
```bash
# First, install mermaid-cli globally (one time only)
npm install -g @mermaid-js/mermaid-cli

# Then run the Python script
python generate_diagrams.py
```

Both scripts will:
- Check if mermaid-cli is installed
- Convert all diagram .md files to PNG
- Save output to `doc/png/` folder
- Use dark theme with transparent background
- Process all 16 diagrams automatically
- Generate high-resolution PNGs (4x scale)

### VSCode Preview
1. Install the "Markdown Preview Mermaid Support" extension
2. Open any diagram file
3. Press `Ctrl+Shift+V` to open preview
4. Right-click the diagram and copy/screenshot

### Online Alternative
1. Visit https://mermaid.live
2. Copy the mermaid code block from any file
3. Paste into the editor
4. Click "Actions" → "PNG" to download

### Manual Command Line
```bash
npm install -g @mermaid-js/mermaid-cli
mmdc -i 01-Architecture-Layers.md -o 01-Architecture-Layers.png -b transparent -t dark
```

## Quick Reference

### Key Components
- **GroupObject**: Central communication hub (all data flows through it)
- **Facade**: Central coordinator managing all modules
- **Module**: Your custom code inherits from this base class
- **ComFlag**: State flag controlling GroupObject synchronization

### Data Flow Summary
1. **Incoming**: KNX Bus → TPUART → DataLink → Network → Transport → Application → GroupObject → Your Module
2. **Outgoing**: Your Module → GroupObject → Application → Transport → Network → DataLink → TPUART → KNX Bus

### Processing Model
- **Core 0**: Main loop handling KNX protocol, modules, and logic
- **Core 1**: High-speed loop handling LED animations (50 FPS) and sensor I2C reads
- **Event-Driven**: No polling, everything triggered by callbacks

## Understanding the Architecture

The OpenKNX framework uses a layered architecture where:

1. **Your application code** (in `src/`) extends the Module base class
2. **The OpenKNX Facade** coordinates all modules and provides services (time, flash, logging)
3. **Feature Modules** (Logic, Buttons, Sensors) provide reusable functionality
4. **The KNX Application Layer** manages GroupObjects (communication objects)
5. **The KNX Protocol Stack** handles all bus communication details
6. **The Physical Layer** (TPUART) interfaces with the KNX bus

This separation means you can focus on your application logic while the framework handles all the complex KNX protocol details automatically.

## For More Information

See the comprehensive analysis in the initial exploration that generated these diagrams (available in the conversation history).
