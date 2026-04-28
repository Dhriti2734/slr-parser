
set -e  

echo "======================================"
echo "  SLR Parser — Build Script"
echo "======================================"

if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ not found. Please install g++:"
    echo "  Ubuntu/Debian: sudo apt install g++"
    echo "  Mac:           brew install gcc"
    echo "  Windows:       Install MinGW or use WSL"
    exit 1
fi

echo "[1/3] Checking source files..."
SOURCES=(
    "src/main.cpp"
    "src/grammar/grammar.cpp"
    "src/parser/lr0.cpp"
    "src/parser/slr_table.cpp"
    "src/parser/parser.cpp"
    "src/parse_tree/parse_tree.cpp"
    "src/ast/ast.cpp"
)

for f in "${SOURCES[@]}"; do
    if [ ! -f "$f" ]; then
        echo "ERROR: Missing source file: $f"
        exit 1
    fi
done
echo "   All source files found."

echo "[2/3] Compiling C++ backend..."
g++ -std=c++11 -I src \
    src/main.cpp \
    src/grammar/grammar.cpp \
    src/parser/lr0.cpp \
    src/parser/slr_table.cpp \
    src/parser/parser.cpp \
    src/parse_tree/parse_tree.cpp \
    src/ast/ast.cpp \
    -o slr_parser \
    -Wall -Wextra -O2

echo "   Compiled successfully -> slr_parser"

echo "[3/3] Copying binary next to app..."
cp slr_parser app/slr_parser
echo "   Copied -> app/slr_parser"

echo ""
echo "======================================"
echo "  Build complete!"
echo "======================================"
echo ""
echo "To run the Streamlit frontend:"
echo "  cd app"
echo "  pip install streamlit"
echo "  streamlit run app.py"
echo ""
echo "To test the C++ backend directly:"
echo "  echo 'first_follow"
echo "  3"
echo "  E -> E + T"
echo "  E -> T"
echo "  T -> id"
echo "  ' | ./slr_parser"
echo ""
