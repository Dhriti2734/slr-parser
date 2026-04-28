# backend_runner.py

import subprocess
import os
import sys

def get_binary_path():
    base = os.path.dirname(os.path.abspath(__file__))
    # Try common locations
    candidates = [
        os.path.join(base, "slr_parser"),           # Linux/Mac
        os.path.join(base, "slr_parser.exe"),        # Windows
        os.path.join(base, "..", "slr_parser"),
        os.path.join(base, "..", "slr_parser.exe"),
    ]
    for path in candidates:
        if os.path.isfile(path):
            return path
    return None


def run_command(command: str, grammar_text: str, input_string: str = "") -> dict:
    """
    Run the C++ parser with the given command and inputs.

    Args:
        command:      one of: grammar, first_follow, lr0, slr_table,
                               parse, parse_tree, ast
        grammar_text: multiline string of grammar rules
        input_string: the token string to parse (e.g. "id + num")

    Returns:
        dict with keys:
            'success'  : bool
            'output'   : str  (stdout from C++)
            'error'    : str  (stderr or error message)
    """

    binary = get_binary_path()
    if binary is None:
        return {
            "success": False,
            "output": "",
            "error": (
                " C++ binary 'slr_parser' not found!\n\n"
                "Please compile the C++ backend first:\n\n"
                "  g++ -std=c++11 -I src src/main.cpp src/grammar/grammar.cpp "
                "src/parser/lr0.cpp src/parser/slr_table.cpp "
                "src/parser/parser.cpp src/parse_tree/parse_tree.cpp "
                "src/ast/ast.cpp -o slr_parser\n\n"
                "Run this command from the project root directory."
            ),
        }

    grammar_lines = [
        line.strip()
        for line in grammar_text.strip().splitlines()
        if line.strip() and not line.strip().startswith("#")
    ]

    if not grammar_lines:
        return {
            "success": False,
            "output": "",
            "error": " No grammar rules found. Please enter at least one rule.",
        }

    stdin_text = command + "\n"
    stdin_text += str(len(grammar_lines)) + "\n"
    for line in grammar_lines:
        stdin_text += line + "\n"
    stdin_text += input_string + "\n"

    try:
        result = subprocess.run(
            [binary],
            input=stdin_text,
            capture_output=True,
            text=True,
            timeout=15,          
            cwd=os.path.dirname(binary),
        )

        if result.returncode != 0 and result.stderr.strip():
            return {
                "success": False,
                "output": result.stdout,
                "error": result.stderr,
            }

        return {
            "success": True,
            "output": result.stdout,
            "error": result.stderr,
        }

    except subprocess.TimeoutExpired:
        return {
            "success": False,
            "output": "",
            "error": " Timeout: The parser took too long. Check for grammar issues.",
        }
    except FileNotFoundError:
        return {
            "success": False,
            "output": "",
            "error": f" Cannot run binary at: {binary}",
        }
    except Exception as e:
        return {
            "success": False,
            "output": "",
            "error": f" Unexpected error: {str(e)}",
        }


def check_binary() -> tuple:
    """
    Check if the C++ binary exists and is runnable.
    Returns: (found: bool, path: str or None)
    """
    path = get_binary_path()
    return (path is not None), path
