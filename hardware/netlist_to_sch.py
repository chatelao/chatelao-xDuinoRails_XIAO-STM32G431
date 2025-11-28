import sys
import uuid
import re

def tokenize(text):
    # Simple comment removal
    lines = text.split('\n')
    lines = [l.split(';')[0] for l in lines]
    text = '\n'.join(lines)

    # Regex for tokens: parens, quoted strings (handling simple escaped quotes if needed, but simple greedy for now), or other words
    # Note: This regex for string "[^"]*" might fail on escaped quotes, but suffices for typical netlists
    pattern = re.compile(r'\(|\)|"[^"]*"|[^\s()]+')
    return pattern.findall(text)

def parse_sexp(tokens):
    if len(tokens) == 0:
        return []

    # If we are at top level and have multiple expressions, we need to wrap them?
    # But this function is usually called recursively.
    # We will consume tokens from the list.

    token = tokens.pop(0)

    if token == '(':
        res = []
        while len(tokens) > 0 and tokens[0] != ')':
            res.append(parse_sexp(tokens))
        if len(tokens) > 0:
            tokens.pop(0) # pop ')'
        return res
    elif token == ')':
        raise Exception("Unexpected )")
    else:
        # Strip quotes from strings?
        # KiCad values are usually quoted. We might want to keep them stripped for processing.
        if token.startswith('"') and token.endswith('"'):
            return token[1:-1]
        return token

def find_by_key(data, key):
    # data is a list. We look for a sublist starting with key.
    if isinstance(data, list):
        if len(data) > 0 and data[0] == key:
            return data
        for item in data:
            if isinstance(item, list):
                res = find_by_key(item, key)
                if res: return res
    return None

def find_direct_child(data, key):
    # Look only in direct children
    if isinstance(data, list):
        for item in data:
            if isinstance(item, list) and len(item) > 0 and item[0] == key:
                return item
    return None

def find_all_by_key(data, key):
    res = []
    if isinstance(data, list):
        if len(data) > 0 and data[0] == key:
            res.append(data)
        for item in data:
            if isinstance(item, list):
                res.extend(find_all_by_key(item, key))
    return res

class NetlistParser:
    def __init__(self, content):
        tokens = tokenize(content)
        # The file contains one top-level s-expression (export ...) usually
        self.tree = parse_sexp(tokens)

    def get_components(self):
        # (components (comp (ref ...) ...) ...)
        # self.tree is ['export', ...]

        comps_block = find_by_key(self.tree, 'components')
        if not comps_block: return []

        comps = []
        for item in comps_block:
            if isinstance(item, list) and len(item) > 0 and item[0] == 'comp':
                ref_node = find_direct_child(item, 'ref')
                val_node = find_direct_child(item, 'value')
                fp_node = find_direct_child(item, 'footprint')

                ref = ref_node[1] if ref_node and len(ref_node) > 1 else "?"
                value = val_node[1] if val_node and len(val_node) > 1 else "?"
                footprint = fp_node[1] if fp_node and len(fp_node) > 1 else ""

                comps.append({
                    'ref': ref,
                    'value': value,
                    'footprint': footprint
                })
        return comps

    def get_nets(self):
        # (nets (net (code 1) (name GND) (node (ref J1) (pin A1)) ...) ...)
        nets_block = find_by_key(self.tree, 'nets')
        if not nets_block: return []

        nets = []
        for item in nets_block:
            if isinstance(item, list) and len(item) > 0 and item[0] == 'net':
                code_node = find_direct_child(item, 'code')
                name_node = find_direct_child(item, 'name')

                code = code_node[1] if code_node else ""
                name = name_node[1] if name_node else ""

                nodes = []
                # nodes are children of 'net' item
                for child in item:
                    if isinstance(child, list) and len(child) > 0 and child[0] == 'node':
                        ref_node = find_direct_child(child, 'ref')
                        pin_node = find_direct_child(child, 'pin')
                        ref = ref_node[1] if ref_node else ""
                        pin = pin_node[1] if pin_node else ""
                        nodes.append((ref, pin))

                nets.append({'code': code, 'name': name, 'nodes': nodes})
        return nets

def generate_schematic(components, nets, filename):
    # 1. Map RefDes -> Pin List
    comp_pins = { c['ref']: {} for c in components }

    # We also need to capture pin names if possible, but netlist only gives net names.
    # We will use net names as labels.

    for net in nets:
        net_name = net['name']
        for ref, pin in net['nodes']:
            if ref in comp_pins:
                comp_pins[ref][pin] = net_name
            else:
                # Might happen if component list is incomplete or mismatch
                pass

    lines = []
    lines.append('(kicad_sch (version 20211123) (generator "netlist_to_sch")')
    lines.append('  (uuid "{}")'.format(uuid.uuid4()))
    lines.append('  (paper "A4")')

    lines.append('  (lib_symbols')

    # Generate symbols
    components.sort(key=lambda x: x['ref'])

    for c in components:
        ref = c['ref']
        val = c['value']
        pins = comp_pins[ref]

        # Sort pins: try integer sort, else string
        def pin_key(p):
            # Split into alpha and numeric parts?
            # e.g. "A1", "12".
            # Simple approach: if digit, int. Else tuple.
            if p.isdigit():
                return (0, int(p))
            else:
                return (1, p)

        sorted_pins = sorted(pins.keys(), key=pin_key)

        count = len(sorted_pins)
        # Heuristic for box size
        # 1 pin per 2.54mm (100 mil)
        # Min height 5.08
        height = max(5.08, (count + 1) * 2.54)
        width = 15.24 # 600 mils

        lib_id = f"Imported:{ref}"

        lines.append(f'    (symbol "{lib_id}" (in_bom yes) (on_board yes)')
        lines.append(f'      (property "Reference" "{ref}" (id 0) (at 0 {height/2 + 2.54} 0)')
        lines.append(f'        (effects (font (size 1.27 1.27))))')
        lines.append(f'      (property "Value" "{val}" (id 1) (at 0 {height/2 + 5.08} 0)')
        lines.append(f'        (effects (font (size 1.27 1.27))))')

        # Symbol Graphics (Unit 1, Style 1)
        # We put graphics in a separate sub-symbol "Name_1_1" usually?
        # KiCad 6 format:
        # (symbol "Name_0_1" ... graphics ...)
        # (symbol "Name_1_1" ... pins ...)
        # Actually, "0_0" or "0_1" is for body?
        # Let's check format specs or examples.
        # Usually graphics are in `(symbol "Name_0_1" ...)` (unit 0? shared?)
        # Or `(symbol "Name_1_1" ...)` for unit 1.
        # I'll put everything in `_0_0` or `_1_1`.
        # Safe bet: `(symbol "{lib_id}_0_1"` for graphics (shared by all units)
        # And `(symbol "{lib_id}_1_1"` for pins of unit 1.

        lines.append(f'      (symbol "{lib_id}_0_1"')
        lines.append(f'        (rectangle (start {-width/2} {height/2}) (end {width/2} {-height/2})')
        lines.append(f'          (stroke (width 0) (type default) (color 0 0 0 0))')
        lines.append(f'          (fill (type background)))')
        lines.append(f'      )')

        lines.append(f'      (symbol "{lib_id}_1_1"')

        # Place pins on left side
        y_pos = height/2 - 2.54
        for p in sorted_pins:
            # Pin at left edge
            # (at x y angle)
            # angle 0 is Right.
            lines.append(f'        (pin input line (at {-width/2 - 2.54} {y_pos} 0) (length 2.54)')
            lines.append(f'          (name "{p}" (effects (font (size 1.27 1.27))))')
            lines.append(f'          (number "{p}" (effects (font (size 1.27 1.27))))')
            lines.append(f'        )')
            y_pos -= 2.54

        lines.append(f'      )') # End symbol unit
        lines.append(f'    )') # End symbol def

    lines.append('  )') # End lib_symbols

    # Instances
    # Grid
    x_start = 50.8
    y_start = 50.8
    x_spacing = 50.8
    y_spacing = 60.0 # Increased spacing
    cols = 6

    for i, c in enumerate(components):
        ref = c['ref']
        val = c['value']
        lib_id = f"Imported:{ref}"

        row = i // cols
        col = i % cols
        x = x_start + col * x_spacing
        y = y_start + row * y_spacing

        lines.append(f'  (symbol (lib_id "{lib_id}") (at {x} {y} 0) (unit 1)')
        lines.append(f'    (in_bom yes) (on_board yes)')
        lines.append(f'    (uuid "{uuid.uuid4()}")')
        lines.append(f'    (property "Reference" "{ref}" (id 0) (at {x} {y - 5} 0))')
        lines.append(f'    (property "Value" "{val}" (id 1) (at {x} {y - 2.5} 0))')
        lines.append(f'  )')

        # Wiring
        pins = comp_pins[ref]
        # Must match sort order
        def pin_key(p):
            if p.isdigit(): return (0, int(p))
            else: return (1, p)
        sorted_pins = sorted(pins.keys(), key=pin_key)

        count = len(sorted_pins)
        height = max(5.08, (count + 1) * 2.54)
        width = 15.24

        # Calculate pin Y positions again (top to bottom)
        # Started at height/2 - 2.54
        curr_y = height/2 - 2.54

        for p in sorted_pins:
            net_name = pins[p]

            # Pin position (relative to symbol center)
            rel_pin_x = -width/2 - 2.54
            rel_pin_y = curr_y # Positive is Up? No, in KiCad Y down is positive.
            # But in library definition:
            # `(rectangle (start -w/2 h/2) ...)`
            # If h/2 is positive.
            # Usually KiCad library Y-axis: Y positive is UP. (Mathematical)
            # KiCad Schematic Y-axis: Y positive is DOWN. (Screen)
            # This flip causes confusion.
            # If I assumed Y positive is UP in library:
            # `start -w/2 h/2` (Top Left)
            # `pin (at ... h/2 - 2.54)` (Near Top)
            # When placed in Schematic at (X, Y):
            # The library Y is flipped? No.
            # KiCad transforms: Schematic Y = Instance Y - Library Y.
            # Wait.
            # Let's assume standard behavior:
            # If I define pin at (0, 100) in Lib.
            # Instance at (1000, 1000).
            # Pin appears at (1000, 900) (Screen coords).
            # So Library Y positive = Up.

            # So `rel_pin_y = curr_y`.
            # Screen Y = Instance Y - rel_pin_y.

            screen_pin_x = x + rel_pin_x
            screen_pin_y = y - rel_pin_y

            # Label
            label_x = screen_pin_x - 2.54
            label_y = screen_pin_y

            # Wire
            lines.append(f'  (wire (pts (xy {screen_pin_x} {screen_pin_y}) (xy {label_x} {label_y}))')
            lines.append(f'    (stroke (width 0) (type default) (color 0 0 0 0))')
            lines.append(f'    (uuid "{uuid.uuid4()}"))')

            # Label
            lines.append(f'  (label "{net_name}" (at {label_x} {label_y} 0)')
            lines.append(f'    (effects (font (size 1.27 1.27)) (justify right)))')

            curr_y -= 2.54

    lines.append(')')

    with open(filename, 'w') as f:
        f.write('\n'.join(lines))

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python netlist_to_sch.py <input.net> <output.kicad_sch>")
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile, 'r') as f:
        content = f.read()

    parser = NetlistParser(content)
    comps = parser.get_components()
    nets = parser.get_nets()

    print(f"Found {len(comps)} components and {len(nets)} nets.")

    generate_schematic(comps, nets, outfile)
    print(f"Generated {outfile}")
