import sys
import build_utils

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_error

# Main Program
args = build_utils.parse_arguments()
success = build_utils.build(args.configuration)

if success:
    build_utils.run(args.configuration)
else:
    print_error("** Build failed. Cannot run the application.")