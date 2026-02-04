#!/usr/bin/env python3
"""
Mermaid Diagram to PNG Converter
Converts all .md files containing Mermaid diagrams to PNG images.
Requires: npm install -g @mermaid-js/mermaid-cli
"""

import os
import subprocess
import sys
from pathlib import Path

def check_mmdc_installed():
    """Check if mermaid-cli (mmdc) is installed."""
    try:
        result = subprocess.run(['mmdc', '--version'],
                              capture_output=True,
                              text=True)
        return result.returncode == 0
    except FileNotFoundError:
        return False

def convert_md_to_png(md_file, output_dir):
    """Convert a single Mermaid .md file to PNG."""
    filename = md_file.stem
    output_file = output_dir / f"{filename}.png"

    # Run mmdc command with dark theme and transparent background
    cmd = [
        'mmdc',
        '-i', str(md_file),
        '-o', str(output_file),
        '-b', 'transparent',
        '-t', 'dark'
    ]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            return True, f"Created {output_file.name}"
        else:
            return False, f"Failed: {result.stderr}"
    except subprocess.TimeoutExpired:
        return False, "Timeout (30s)"
    except Exception as e:
        return False, str(e)

def main():
    print("=" * 50)
    print("Mermaid Diagram to PNG Converter")
    print("=" * 50)
    print()

    # Check if mmdc is installed
    if not check_mmdc_installed():
        print("ERROR: mermaid-cli is not installed!")
        print()
        print("Please install it first:")
        print("  npm install -g @mermaid-js/mermaid-cli")
        print()
        return 1

    print("Found mermaid-cli, starting conversion...")
    print()

    # Get the script directory
    script_dir = Path(__file__).parent

    # Create output directory
    output_dir = script_dir / 'png'
    output_dir.mkdir(exist_ok=True)

    # Find all .md files (excluding README.md)
    md_files = [f for f in script_dir.glob('*.md')
                if f.name != 'README.md']

    if not md_files:
        print("No diagram files found!")
        return 1

    # Process each file
    success_count = 0
    failed_count = 0

    for md_file in sorted(md_files):
        print(f"Converting {md_file.name}...")
        success, message = convert_md_to_png(md_file, output_dir)

        if success:
            print(f"  [OK] {message}")
            success_count += 1
        else:
            print(f"  [FAILED] {message}")
            failed_count += 1
        print()

    # Summary
    print("=" * 50)
    print("Conversion complete!")
    print(f"Success: {success_count} | Failed: {failed_count}")
    print(f"Output location: {output_dir}")
    print("=" * 50)

    return 0 if failed_count == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
