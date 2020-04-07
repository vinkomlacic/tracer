# TODO

## t_error
### Error reporting (low priority)
*   Consider creating a stack data structure to hold the error from lower levels of code so the
    whole error stack can be reported in order to better display the cause of the error.
    
## Logging
### Fix putting functions in macros
*   If a function that uses logs is put into the log (e.g. `DEBUG(get_pid("tracee"))`) the output
    will be messed up a bit because the DEBUG tag will be around the lower level output of the logging.

## Testing (low priority)
*   Write test cases in a separate document.
*   Test support is needed so choose a library for this.
*   After this write the test cases in C code.
 
## Documentation
*   Create the report

## Scanning for prerequisite tools
*   nm - inspects target binary and libc.so
*   cat - read process memory and similar important files
*   grep - filters output for easier managing (consider using scanf instead for this)
*   ldd - lists objects dependencies for dynamic linking
*   pgrep - get process ID from it's name

## Virus
### Support injecting virus function from another binary
*   Virus is injected as the code from a static function in the main file.
    This could be made like loading a binary and executing this code.
    