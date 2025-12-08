# Implementation Roadmap: XIAO STM32G431

This document outlines the steps to take the XIAO STM32G431 project from its current state to manufacturing at JLCPCB.

## Phase 1: Schematic Design
- [x] **Analyze Netlist**: Review `hardware/xiao_stm32g431.net` to understand the intended connectivity. (Fixed pin mapping to match Proposal).
- [ ] **Create KiCad Project**: Initialize a new KiCad project (`.kicad_pro`).
- [ ] **Schematic Capture**: Create the schematic (`.kicad_sch`) based on the netlist.
    - Verify pinouts for STM32G431.
    - Ensure decoupling capacitors, crystals (if any), and programming headers are present.
- [ ] **JLCPCB Part Selection**:
    - Select components available in JLCPCB's Parts Library (LCSC).
    - Prioritize "Basic Parts" to minimize cost, or "Extended Parts" where necessary.
    - Assign LCSC Part Numbers (e.g., `C12345`) to a custom field (e.g., `LCSC`) in KiCad symbols.
- [ ] **ERC (Electrical Rules Check)**: Run KiCad ERC to catch electrical errors.

## Phase 2: PCB Layout
- [ ] **Board Setup**: Define board outline and constraints (min track width, clearance, via sizes) matching JLCPCB capabilities.
    - *Reference*: [JLCPCB Capabilities](https://jlcpcb.com/capabilities/Capabilities)
- [ ] **Component Placement**: Arrange components logically to minimize trace length and noise.
- [ ] **Routing**: Route traces.
    - Power and Ground planes/polygons.
    - Signal traces.
    - Differential pairs (if any, e.g., USB).
- [ ] **DRC (Design Rules Check)**: Run KiCad DRC to ensure compliance with manufacturing limits.

## Phase 3: Manufacturing Preparation
- [ ] **Gerber Files**: Generate Gerber files (Copper, Solder Mask, Silkscreen, Drill, Edge Cuts).
- [ ] **Drill Files**: Generate Excellon drill files.
- [ ] **Assembly Files (PCBA)**:
    - **BOM (Bill of Materials)**: Export BOM including LCSC part numbers. Format as required by JLCPCB. See [PARTLIST.md](hardware/PARTLIST.md) for selected parts.
    - **CPL (Component Placement List)**: Export Pick and Place file. Format as required by JLCPCB (Designator, Mid X, Mid Y, Layer, Rotation).
- [ ] **3D Viewer Check**: Verify component alignment and footprint orientation.

## Phase 4: Ordering
- [ ] **Upload to JLCPCB**: Upload Gerber archive.
- [ ] **PCBA Configuration**: Select "Assemble your PCB board".
- [ ] **Upload BOM & CPL**: Upload the generated assembly files.
- [ ] **Review**: Check the visual preview of the PCBA to ensure parts are rotated correctly.
- [ ] **Order**: Submit order.

## Software
- [x] **Arduino Examples**: Created examples for CORDIC (SinCos, Polar), FMAC (FIR), OpAmp (Buffer), and Comparator (DAC).
- [x] **CI/CD**: Added GitHub Actions workflow (`arduino_build.yml`) to compile Arduino examples on every push.
