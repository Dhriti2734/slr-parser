# app.py
# SLR Parser : Streamlit Frontend

import streamlit as st
import sys
import os


sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import backend_runner

st.set_page_config(
    page_title="SLR Parser",
    page_icon="🔍",
    layout="wide",
    initial_sidebar_state="expanded",
)

st.markdown("""
<style>
/* Main header */
.main-title {
    font-size: 2.2rem;
    font-weight: 700;
    color: #1a1a2e;
    margin-bottom: 0.2rem;
}
.subtitle {
    font-size: 1rem;
    color: #555;
    margin-bottom: 1.5rem;
}

/* Section headers */
.section-title {
    font-size: 1.1rem;
    font-weight: 600;
    color: #16213e;
    padding: 6px 12px;
    background: #e8f4f8;
    border-left: 4px solid #0f3460;
    border-radius: 4px;
    margin-bottom: 0.5rem;
}

/* Status badges */
.badge-ok  { background:#d4edda; color:#155724; padding:4px 10px;
             border-radius:12px; font-size:0.85rem; }
.badge-err { background:#f8d7da; color:#721c24; padding:4px 10px;
             border-radius:12px; font-size:0.85rem; }

/* Output code block */
.output-box {
    background: #0d1117;
    color: #c9d1d9;
    padding: 16px;
    border-radius: 8px;
    font-family: 'Courier New', monospace;
    font-size: 0.88rem;
    white-space: pre;
    overflow-x: auto;
    line-height: 1.5;
}

/* Tip box */
.tip-box {
    background: #fff8e1;
    border-left: 4px solid #ffc107;
    padding: 8px 14px;
    border-radius: 4px;
    font-size: 0.88rem;
    color: #555;
}
</style>
""", unsafe_allow_html=True)


with st.sidebar:
    st.markdown("## ⚙️ Grammar Input")
    st.markdown("Enter grammar rules, one per line.")
    st.markdown("Use `->` for arrow and `|` for alternatives.")

    DEFAULT_GRAMMAR = """\
E -> E + T | E - T | T
T -> T * F | T / F | F
F -> ( E ) | id | num"""

    grammar_text = st.text_area(
        "Grammar Rules",
        value=DEFAULT_GRAMMAR,
        height=200,
        help="Example: E -> E + T | T",
        key="grammar_input",
    )

    st.markdown("---")
    st.markdown("## 📝 Input String")
    st.markdown("Tokens separated by spaces.")

    DEFAULT_INPUT = "id + num * id"
    input_string = st.text_input(
        "Input String",
        value=DEFAULT_INPUT,
        help="Separate tokens with spaces: id + num * id",
        key="input_string",
    )

    st.markdown("---")

    found, path = backend_runner.check_binary()
    if found:
        st.markdown(
            f'<span class="badge-ok">✅ C++ backend ready</span>',
            unsafe_allow_html=True,
        )
        st.caption(f"`{os.path.basename(path)}`")
    else:
        st.markdown(
            '<span class="badge-err">❌ C++ binary not found</span>',
            unsafe_allow_html=True,
        )
        st.caption("Compile with: `g++ -std=c++11 ...`")

    st.markdown("---")
    st.markdown("### 📚 Grammar Format")
    st.markdown("""
```
E -> E + T | T
T -> T * F | F
F -> ( E ) | id | num
```
- `->` separates LHS and RHS
- `|` separates alternatives
- Use `eps` for epsilon (ε)
- Terminals are lowercase or symbols
- Non-terminals are uppercase
    """)



# MAIN AREA
st.markdown('<div class="main-title">🔍 SLR Parser</div>', unsafe_allow_html=True)
st.markdown(
    '<div class="subtitle">Grammar → FIRST/FOLLOW → LR(0) → SLR Table → Parse → Parse Tree → AST</div>',
    unsafe_allow_html=True,
)
st.markdown("---")


def run_and_show(command: str, need_input: bool = False, label: str = "Output"):
    """
    Run the C++ backend with given command and display results.
    """
    inp = input_string if need_input else ""

    with st.spinner(f"Running {command}..."):
        result = backend_runner.run_command(command, grammar_text, inp)

    if result["success"]:
        if result["output"].strip():
            # Show the raw output in a monospace block
            st.markdown(f'<div class="section-title">📄 {label}</div>',
                        unsafe_allow_html=True)
            st.code(result["output"], language=None)
        else:
            st.info("No output generated. Check your grammar.")

        # Show any stderr warnings
        if result["error"].strip():
            st.warning(f"Warnings:\n{result['error']}")
    else:
        st.error("**Error from C++ backend:**")
        st.code(result["error"], language=None)
        if result["output"].strip():
            st.markdown("**Partial output:**")
            st.code(result["output"], language=None)

def tip(text: str):
    st.markdown(f'<div class="tip-box">💡 {text}</div>', unsafe_allow_html=True)
    st.markdown("")



# SECTION 1: GRAMMAR OVERVIEW
with st.expander("📖 Grammar Overview", expanded=False):
    st.markdown("Click to parse and display your grammar rules and symbols.")
    if st.button("Show Grammar", key="btn_grammar"):
        run_and_show("grammar", label="Grammar Rules & Symbols")



# SECTION 2: FIRST & FOLLOW SETS
st.markdown("---")
st.markdown('<div class="section-title">1️⃣ FIRST & FOLLOW Sets</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown("""
**FIRST(A)** = set of terminals that can begin strings derived from A

**FOLLOW(A)** = set of terminals that can appear immediately after A
    """)
with col2:
    btn_ff = st.button("▶ Compute FIRST/FOLLOW", key="btn_ff", use_container_width=True)

if btn_ff:
    tip("FIRST sets tell us what tokens can start each non-terminal. FOLLOW sets help build the SLR reduce actions.")
    run_and_show("first_follow", label="FIRST and FOLLOW Sets")



# SECTION 3: LR(0) ITEM SETS
st.markdown("---")
st.markdown('<div class="section-title">2️⃣ LR(0) Item Sets (States)</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown("""
Each **LR(0) state** is a set of *items* — grammar rules with a dot (`.`) 
showing how far we've parsed. The dot moves right as we consume tokens.

Example: `E -> E . + T` means we've seen `E`, now expecting `+ T`.
    """)
with col2:
    btn_lr0 = st.button("▶ Build LR(0) States", key="btn_lr0", use_container_width=True)

if btn_lr0:
    tip("The dot (.) shows the parser's position in a rule. GOTO transitions connect states.")
    run_and_show("lr0", label="LR(0) Item Sets")


# SECTION 4: SLR PARSING TABLE
st.markdown("---")
st.markdown('<div class="section-title">3️⃣ SLR Parsing Table (ACTION + GOTO)</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown("""
The **ACTION table** tells the parser what to do for each (state, terminal) pair:
- **s**N = SHIFT to state N  
- **r**N = REDUCE using rule N  
- **acc** = Accept (input valid)

The **GOTO table** tells the parser which state to go to after a reduce.
    """)
with col2:
    btn_slr = st.button("▶ Build SLR Table", key="btn_slr", use_container_width=True)

if btn_slr:
    tip("s = shift, r = reduce, acc = accept. Conflicts (two entries in same cell) mean the grammar is not SLR(1).")
    run_and_show("slr_table", label="SLR Parsing Table")


# SECTION 5: STEP-BY-STEP PARSE
st.markdown("---")
st.markdown('<div class="section-title">4️⃣ Step-by-Step Parse Trace</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown(f"""
Parse the input string **`{input_string}`** step by step.

Each row shows:
- **STACK**: current parse stack (states + symbols)  
- **INPUT**: remaining input tokens  
- **ACTION**: what the parser does (SHIFT / REDUCE / ACCEPT / ERROR)
    """)
with col2:
    btn_parse = st.button("▶ Parse Input", key="btn_parse", use_container_width=True)

if btn_parse:
    if not input_string.strip():
        st.warning("⚠️ Please enter an input string in the sidebar.")
    else:
        tip(f"Parsing: `{input_string}` — tokens must be separated by spaces.")
        run_and_show("parse", need_input=True, label="Parse Trace")


# SECTION 6: PARSE TREE
st.markdown("---")
st.markdown('<div class="section-title">5️⃣ Parse Tree (Concrete Syntax Tree)</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown("""
The **Parse Tree** (Concrete Syntax Tree) shows every step of the derivation.
Every grammar rule application appears as a node with its RHS symbols as children.

This is the **complete, uncompressed** derivation tree.
    """)
with col2:
    btn_tree = st.button("▶ Build Parse Tree", key="btn_tree", use_container_width=True)

if btn_tree:
    if not input_string.strip():
        st.warning("⚠️ Please enter an input string in the sidebar.")
    else:
        tip("The parse tree shows ALL grammar rule applications. The AST (next) simplifies this by removing intermediate nodes.")
        run_and_show("parse_tree", need_input=True, label="Parse Tree")


# SECTION 7: ABSTRACT SYNTAX TREE (AST)
st.markdown("---")
st.markdown('<div class="section-title">6️⃣ Abstract Syntax Tree (AST)</div>',
            unsafe_allow_html=True)

col1, col2 = st.columns([3, 1])
with col1:
    st.markdown("""
The **AST** simplifies the parse tree by:
- Removing intermediate non-terminal chains (E→T→F→id becomes just `id`)
- Making operators the **root** of sub-expressions  
- Removing grouping symbols `(` and `)`

The AST is what compilers actually use for code generation.

**Example:** `id + num * id` becomes:
```
+
+-- id
+-- *
    +-- num
    +-- id
```
    """)
with col2:
    btn_ast = st.button("▶ Build AST", key="btn_ast", use_container_width=True)

if btn_ast:
    if not input_string.strip():
        st.warning("⚠️ Please enter an input string in the sidebar.")
    else:
        tip("The AST shows the semantic structure. Operators are roots, operands are leaves.")
        run_and_show("ast", need_input=True, label="Parse Tree + AST")


# SECTION 8: RUN ALL (convenience button)
st.markdown("---")
st.markdown('<div class="section-title">🚀 Run Everything</div>',
            unsafe_allow_html=True)

st.markdown("Run all 6 steps in sequence with one click.")

btn_all = st.button("▶▶ Run All Steps", key="btn_all", use_container_width=False)

if btn_all:
    if not input_string.strip():
        st.warning("⚠️ Please enter an input string in the sidebar.")
    else:
        steps = [
            ("grammar",      False, "Grammar Rules"),
            ("first_follow", False, "FIRST & FOLLOW Sets"),
            ("lr0",          False, "LR(0) Item Sets"),
            ("slr_table",    False, "SLR Parsing Table"),
            ("parse",        True,  "Parse Trace"),
            ("parse_tree",   True,  "Parse Tree"),
            ("ast",          True,  "AST"),
        ]

        for cmd, need_inp, lbl in steps:
            st.markdown(f"### {lbl}")
            run_and_show(cmd, need_input=need_inp, label=lbl)
            st.markdown("")


# FOOTER
st.markdown("---")
st.markdown("""
<div style="text-align:center; color:#888; font-size:0.82rem; padding: 10px;">
    SLR Parser Project &nbsp;|&nbsp; C++ Backend + Python Streamlit Frontend<br>
    Grammar → FIRST/FOLLOW → LR(0) States → SLR Table → Parse Trace → Parse Tree → AST
</div>
""", unsafe_allow_html=True)
