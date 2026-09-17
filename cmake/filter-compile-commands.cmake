# Filter generated grit .c entries out of compile_commands.json so clangd
# infers C++ header flags from .cc TUs instead of arm-none-eabi-gcc.
#
# Usage: cmake -DCOMPILE_COMMANDS=<path> -P filter-compile-commands.cmake

if(NOT COMPILE_COMMANDS)
  message(FATAL_ERROR "COMPILE_COMMANDS must be set")
endif()

if(NOT EXISTS "${COMPILE_COMMANDS}")
  message(STATUS "compile_commands.json not found yet: ${COMPILE_COMMANDS}")
  return()
endif()

find_program(_PYTHON3 python3 REQUIRED)

execute_process(
  COMMAND ${_PYTHON3} -c
    "import json,sys;\
p=sys.argv[1];\
db=json.load(open(p));\
kept=[e for e in db if not str(e.get('file','')).endswith('.c')];\
json.dump(kept, open(p,'w'), indent=2);\
print(len(kept))"
    ${COMPILE_COMMANDS}
  RESULT_VARIABLE _filter_rc
  OUTPUT_VARIABLE _filter_out
  ERROR_VARIABLE _filter_err
)

if(NOT _filter_rc EQUAL 0)
  message(FATAL_ERROR "Failed to filter compile_commands.json: ${_filter_err}")
endif()

message(STATUS "Filtered .c entries from ${COMPILE_COMMANDS} (kept ${_filter_out} entries)")
