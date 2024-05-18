import build_utils

# Main Program
args = build_utils.parse_arguments()
success = build_utils.build(args.configuration, args.platform)

if success:
    build_utils.run(args.configuration, args.platform)
else:
    print("** Build failed. Cannot run the application.")