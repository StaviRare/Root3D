try:
    from colorama import Fore, Style, init
    init(autoreset=True)
    use_color = True
except ImportError:
    use_color = False

# Since this logger is used across the board, 
# We want it to work even if you haven't downloaded the colorama package.

def print_regular(message):
    if use_color:
        print(f"{Style.BRIGHT}{message}{Style.RESET_ALL}")
    else:
        print(f"{message}")

def print_warning(message):
    if use_color:
        print(f"{Fore.YELLOW}{Style.BRIGHT}{message}{Style.RESET_ALL}")
    else:
        print(f"{message}")

def print_error(message):
    if use_color:
        print(f"{Fore.RED}{Style.BRIGHT}{message}{Style.RESET_ALL}")
    else:
        print(f"{message}")

def print_success(message):
    if use_color:
        print(f"{Fore.GREEN}{Style.BRIGHT}{message}{Style.RESET_ALL}")
    else:
        print(f"{message}")