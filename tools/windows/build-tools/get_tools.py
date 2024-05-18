import os
import sys
sys.path.append('../../_utils')
from tool_getter import download_and_extract

FILE_ID = '1eSJMa0XB8muNzrxBxhDvdXaIoJFO2hEz'
OUTPUT_PATH = os.path.abspath("resources")

# Main Program
download_and_extract(FILE_ID, OUTPUT_PATH)