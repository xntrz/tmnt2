"""
---------------------------------------------------------------------
| Description:                                                      |
---------------------------------------------------------------------
    Script to convert macro names to IDs using conversion rules.
    (written via deepseek)
    
---------------------------------------------------------------------
| Usage format:                                                     |
---------------------------------------------------------------------
    python path2id.py --rule <rule_file> --dir <sources_dir> [--exclude <filename>]
    
"""
import re
import sys
import os


class ConversionRule:
    def __init__(self, macro_names, header_file, table_name, path_ignore_level):
        self.macro_names: list[str] = macro_names           # List of macro names to trigger conversion
        self.header_file: str = header_file                 # Path to .h file containing the table
        self.table_name: str = table_name                   # Name of the table in the .h file
        self.path_ignore_level: int = path_ignore_level     # How many path levels to ignore (0 = whole path)
        self.name_to_id: dict[str, int] = None              # Dictionary mapping names to IDs


    def __post_init__(self):
        if self.name_to_id is None:
            self.name_to_id = {}


class NameToIdConverter:
    def __init__(self, rule_file: str, verbose_flag: bool = False):
        """ Initialize converter with rules from file. """        
        self.rules: list[ConversionRule] = []
        self.macro_to_rule: dict[str, ConversionRule] = {}
        self.verbose = verbose_flag
        self._load_rules(rule_file)
    

    def _print_info(self, msg: str):
        """Prints debug msg."""
        if not self.verbose:
            return
        
        print(msg)


    def _load_rules(self, rule_file: str):
        """Load conversion rules from file."""
        if not os.path.exists(rule_file):
            raise FileNotFoundError(f"Rule file not found: {rule_file}")
        
        with open(rule_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove comments (lines starting with //)
        lines = content.split('\n')
        cleaned_lines = []
        for line in lines:
            # Remove inline comments too
            if '//' in line:
                line = line[:line.index('//')]
            cleaned_lines.append(line.strip())
        
        cleaned_content = '\n'.join(cleaned_lines)
        
        # Parse rules using regex
        # Each rule is between { and },
        rule_pattern = r'\{([^}]+)\}'
        matches = re.finditer(rule_pattern, cleaned_content)
        
        for match in matches:
            rule_content = match.group(1).strip()
            if not rule_content:
                continue
                
            # Split by semicolons and clean up
            parts = [part.strip().rstrip(';') for part in rule_content.split(';') if part.strip()]
            
            if len(parts) < 3:
                print(f"Warning: Invalid rule format: {rule_content}")
                continue
            
            # Parse macro names (can be comma-separated list)
            macro_names_str = parts[0]
            macro_names = [name.strip() for name in macro_names_str.split(',') if name.strip()]
            
            header_file = parts[1]
            table_name = parts[2]
            
            # Parse path ignore level (default to 0 if not specified)
            path_ignore_level = 0
            if len(parts) >= 4:
                try:
                    path_ignore_level = int(parts[3])
                except ValueError:
                    print(f"Warning: Invalid path ignore level: {parts[3]}, using 0")
                    path_ignore_level = 0
            
            rule = ConversionRule(macro_names, header_file, table_name, path_ignore_level)
            self.rules.append(rule)
            
            # Map each macro name to its rule
            for macro in macro_names:
                self.macro_to_rule[macro] = rule
        
        print(f"Loaded {len(self.rules)} rules")
        print(f"Macros registered: {list(self.macro_to_rule.keys())}")
    

    def _normalize_path(self, path: str, ignore_level: int) -> str:
        """
        Normalize a path by removing specified number of leading path components.

        Returns:
            Normalized path with levels removed
        """
        if ignore_level <= 0:
            return path
        
        # Split path by directory separators
        parts = path.replace('\\', '/').split('/')
        
        # Remove empty parts (in case of double slashes or trailing slashes)
        parts = [p for p in parts if p]
        
        # Remove the specified number of leading parts
        if ignore_level < len(parts):
            return '/'.join(parts[ignore_level:])
        else:
            # If ignore_level is >= number of parts, return just the last part
            # or empty string if no parts left
            if parts:
                return parts[-1]
            else:
                return ""
    

    def _parse_h_file(self, header_file: str, table_name: str) -> dict[str, int]:
        """
        Parse .h file to extract name-to-ID mapping from specified table.
        ID is the index (position) in the array, starting from 0.

        Returns:
            Dictionary mapping names to indices
        """
        name_to_id = {}
        
        if not os.path.exists(header_file):
            print(f"Warning: Header file not found: {header_file}")
            return name_to_id
        
        with open(header_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find the table definition
        # Pattern to match: table_name[] = { ... };
        table_pattern = rf'{re.escape(table_name)}\s*\[\s*\]\s*=\s*\{{(.*?)}}\s*;'
        match = re.search(table_pattern, content, re.DOTALL | re.MULTILINE)
        
        if not match:
            # Try alternative pattern without "const" keyword
            table_pattern = rf'(?:const\s+)?{re.escape(table_name)}\s*\[\s*\]\s*=\s*\{{(.*?)}}\s*;'
            match = re.search(table_pattern, content, re.DOTALL | re.MULTILINE)
        
        if not match:
            # Try pattern for static tables
            table_pattern = rf'static\s+(?:const\s+)?{re.escape(table_name)}\s*\[\s*\]\s*=\s*\{{(.*?)}}\s*;'
            match = re.search(table_pattern, content, re.DOTALL | re.MULTILINE)
        
        if not match:
            print(f"Warning: Table '{table_name}' not found in {header_file}")
            return name_to_id
        
        table_content = match.group(1)
        
        # Clean up the table content - remove comments and normalize whitespace
        # Remove single-line comments
        table_content = re.sub(r'//.*', '', table_content)
        # Remove multi-line comments
        table_content = re.sub(r'/\*.*?\*/', '', table_content, flags=re.DOTALL)
        
        # Try to parse as simple string array first (const char* table[])
        # Look for quoted strings directly in the array
        string_pattern = r'"([^"]+)"'
        strings = re.findall(string_pattern, table_content)
        
        if strings:
            # Found simple string array
            for idx, string in enumerate(strings):
                name_to_id[string] = idx
            print(f"  Parsed {len(strings)} strings from simple string array")
            return name_to_id
        
        # Also try single quotes
        string_pattern = r"'([^']+)'"
        strings = re.findall(string_pattern, table_content)
        
        if strings:
            # Found simple string array with single quotes
            for idx, string in enumerate(strings):
                name_to_id[string] = idx
            print(f"  Parsed {len(strings)} strings from simple string array (single quotes)")
            return name_to_id
        
        # If no simple strings found, try to parse as struct array
        # Split into individual entries by looking for },{ or standalone }
        entries = []
        current_entry = ""
        brace_count = 0
        
        for char in table_content:
            if char == '{':
                brace_count += 1
                if brace_count == 1:
                    current_entry = char
                else:
                    current_entry += char
            elif char == '}':
                brace_count -= 1
                current_entry += char
                if brace_count == 0:
                    entries.append(current_entry.strip())
                    current_entry = ""
            elif brace_count > 0:
                current_entry += char
        
        # Extract the first string (name/path) from each entry
        for idx, entry in enumerate(entries):
            # Look for the first quoted string in the entry
            string_match = re.search(r'"([^"]+)"', entry)
            if string_match:
                name = string_match.group(1)
                name_to_id[name] = idx
            else:
                # Try with single quotes
                string_match = re.search(r"'([^']+)'", entry)
                if string_match:
                    name = string_match.group(1)
                    name_to_id[name] = idx
        
        print(f"  Parsed {len(entries)} entries from struct array")
        return name_to_id
    
    
    def load_all_tables(self):
        """Load all conversion tables from .h files."""
        for rule in self.rules:
            if not rule.name_to_id:
                print(f"Loading table {rule.table_name} from {rule.header_file}...")
                rule.name_to_id = self._parse_h_file(rule.header_file, rule.table_name)
                if rule.name_to_id:
                    print(f"  Loaded {len(rule.name_to_id)} entries from {rule.table_name}")
                else:
                    print(f"  Warning: No entries loaded from {rule.table_name}")
    

    def convert_macro(self, macro_name: str, param: str) -> str:
        """
        Convert a macro call to its corresponding ID (array index).

        Returns:
            String representation of ID, or original macro call if not found
        """
        if macro_name not in self.macro_to_rule:
            self._print_info(f"  Macro '{macro_name}' not in macro_to_rule")
            return f"{macro_name}(\"{param}\")"
        
        rule = self.macro_to_rule[macro_name]
        
        # Remove quotes if present
        clean_param = param.strip('"\'')
        
        # Normalize the path based on ignore level
        normalized_param = self._normalize_path(clean_param, rule.path_ignore_level)

        # Convert to table format
        normalized_param = normalized_param.lower()
        
        # Debug output
        self._print_info(f"  Converting {macro_name}(\"{clean_param}\") with ignore level {rule.path_ignore_level}")
        self._print_info(f"    Normalized: \"{normalized_param}\"")
        
        # Try to find the normalized path in the table
        if normalized_param in rule.name_to_id:
            result = str(rule.name_to_id[normalized_param])
            self._print_info(f"    Found at index {result}")
            return result
        else:
            # Also try the original path (for backward compatibility)
            if clean_param in rule.name_to_id:
                result = str(rule.name_to_id[clean_param])
                self._print_info(f"    Found original path at index {result}")
                return result
            
            self._print_info(f"    Warning: '{normalized_param}' (from '{clean_param}') not found in {rule.table_name}")
            # Show some entries from the table to help debug
            for i, (name, idx) in enumerate(list(rule.name_to_id.items())[:10]):
                self._print_info(f"    Table has: [{idx}] = \"{name}\"")
            
            return f"{macro_name}(\"{param}\")"
    
    
    def convert_file(self, input_file: str, excludes: list[str], output_file: str = None):
        """ Convert all macros in input file and write to output file. """
        if os.path.basename(input_file) in excludes:
            return
        
        if input_file in excludes:
            return
        
        if not os.path.exists(input_file):
            raise FileNotFoundError(f"Input file not found: {input_file}")
        
        if output_file is None:
            output_file = input_file
        
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Get all macro names
        macro_names = list(self.macro_to_rule.keys())
        if not macro_names:
            self._print_info(f"  No macros registered for conversion")
            return
        
        self._print_info(f"  Looking for macros: {macro_names}")
        
        # Pattern to match macro calls: MACRO_NAME("parameter")
        macro_pattern = rf'\b({"|".join(re.escape(m) for m in macro_names)})\s*\(\s*"([^"]+)"\s*\)'
        
        # Debug: find all matches first
        matches = list(re.finditer(macro_pattern, content))
        self._print_info(f"  Found {len(matches)} macro calls with double quotes")
        
        if matches:
            for match in matches:
                macro_name = match.group(1)
                param = match.group(2)
                self._print_info(f"    Found: {macro_name}(\"{param}\")")
        
        def replace_macro(match):
            macro_name = match.group(1)
            param = match.group(2)
            return self.convert_macro(macro_name, param)
        
        # Convert all macro calls with double quotes
        converted_content = re.sub(macro_pattern, replace_macro, content)
        
        # Also handle macros with single quotes
        macro_pattern_single = rf'\b({"|".join(re.escape(m) for m in macro_names)})\s*\(\s*\'([^\']+)\'\s*\)'
        
        # Debug: find single quote matches
        matches_single = list(re.finditer(macro_pattern_single, converted_content))
        self._print_info(f"  Found {len(matches_single)} macro calls with single quotes")
        
        if matches_single:
            for match in matches_single:
                macro_name = match.group(1)
                param = match.group(2)
                self._print_info(f"    Found: {macro_name}('{param}')")
        
        converted_content = re.sub(macro_pattern_single, replace_macro, converted_content)
        
        # Check if anything changed
        if converted_content == content:
            self._print_info(f"  No changes made to {input_file}")
        else:
            # Write output
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(converted_content)
            
            self._print_info(f"  Successfully converted {input_file} -> {output_file}")
    

    def convert_directory(self, directory: str, excludes: list[str], extensions: list[str] = None):
        """ Convert all files in directory with given excludes & extensions. """
        if extensions is None:
            extensions = ['.c', '.cpp', '.h', '.hpp', '.cc', '.cxx']
        
        converted_count = 0
        for root, dirs, files in os.walk(directory):
            for file in files:
                if any(file.endswith(ext) for ext in extensions):
                    input_path = os.path.join(root, file)
                    print(f"Processing {input_path}...")
                    self.convert_file(input_path, excludes)
                    converted_count += 1
        
        print(f"\nConverted {converted_count} files in {directory}\n")


def set_cwd():
    cwd = os.path.dirname(os.path.abspath(__file__))
    os.chdir(cwd)


def main():
    # set_cwd()

    rule = ""
    directory = ""
    exclude = []
    verbose = False

    for i, arg in enumerate(sys.argv):
        # Check for rule set file
        if sys.argv[i] == '--rule': 
            rule = sys.argv[i + 1]
        
        # Check for dir path
        if sys.argv[i] == '--dir':
            directory = sys.argv[i + 1]

        # Check for exclude files
        if sys.argv[i] == '--exclude':
            exclude.append(sys.argv[i + 1])

        # Check for verbose flag
        if sys.argv[i] == '--verbose':
            verbose = True

    # Initialize converter
    try:
        converter = NameToIdConverter(rule, verbose)
        converter.load_all_tables()
    except FileNotFoundError as e:
        print(f"Error: {e}")
        sys.exit(1)

    # Convert dir
    converter.convert_directory(directory, exclude)


if __name__ == "__main__":
    main()