#pragma once
#include <filesystem>

#define PARSER_SCRIPT   "tools/strace_parser.py"
#define ANALYZER_SCRIPT "tools/proc_analyzer.py"
#define DATA_FILE       "ml_data/data.json"
#define PYTHON_PATH     "VENV/bin/python3"


auto abs_parser_script_path = std::filesystem::absolute(PARSER_SCRIPT);