import build_utils

# Main Program
args = build_utils.parse_arguments()
build_utils.build(args.configuration, args.platform)