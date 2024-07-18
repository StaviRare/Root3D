import os
import sys

# Project-specific imports
sys.path.append('../../utilities/')
from toolchain import download_and_extract_toolchain

# Global path definitions
FILE_ID = '1eitjBEWx7fTki69AKCXIOAeDrhNPUq-9'
OUTPUT_PATH = os.path.abspath("resources/build_tools")

# Main Program
download_and_extract_toolchain(FILE_ID, OUTPUT_PATH)
