# TODO

## ~~1. process_info~~
### ~~Get rid of dependency on the target name~~
*   Work it out so everything works with PID.
    Piping to pgrep is expensive and that's what happens every time you call `get_pid(pid_t)`.
### ~~Add a specific error~~
*   `get_function_code`: make a specific error when the function is too big (`> MAX_CODE_LEN`).
### ~~Refactor into smaller functions~~
*   The module needs refactoring


## 2. t_error
### ~~Contextual error reporting~~
*   Add support for context reporting in error messages. Currently, what happens is that
    only a message is printed corresponding to the error code. Using macros and structs,
    actual line number and file could be reported with a customized message.
### ~~t_strerror~~
*   Define `t_strerror` function which is analogous to the strerror from `string.h` header 
    (returns the error message for a certain error code).
### t_error struct
*   Optimize t_error struct to minimize padding.
### Error reporting (low priority)
*   Consider creating a stack data structure to hold the error from lower levels of code so the
    whole error stack can be reported in order to better display the cause of the error.


## 3. process_control
### ~~General refactoring~~
*   This module needs complete refactoring and maybe needs to be separated into two or three
    modules (ptrace wrapper and an API for calling functions
    (e.g JAVA function interfaces like runnable, consumer, producer, function)).
    
    **comment:** an interface for function calling like this turned out to be an overkill and a complication
    to implement so it's discarded


## 4. procmem
### ~~General refactoring~~
*   This module needs a bit of refactoring because a lot of duplicated code is found.
    It would be nice to extract functions that open, prepare and close a file.
    
## 5. pstate_t and pstate
### ~~Struct initialization~~
*   Check how exactly are structs initialized. It is possible that `create_pstate()` is
    redundant. Maybe using designated (or not) initializers could be beneficial.
### ~~Validation~~ 
*   Write down illegal pstate states and make functions that validate this state. Declare error codes and 
    define what exactly is an uninitialized state and how state must look like to be valid.    
### PSTATE struct
*   Optimize pstate_t struct to minimize padding.


## 6. options
### ~~Support more options~~
*   Currently not everything is covered by the options (actually just the cleanup mode is covered) so
    it's required to add more options to avoid hardcoding stuff like target process and target functions.
### Help option
*   Another note, a help option is needed as well so define exactly how the help should look like and
    whether it should be printed on error (maybe only on some errors?) or only when `-h` option is
    specified.
    
## 7. Logging
### Fix putting functions in macros
*   If a function that uses logs is put into the log (e.g. `DEBUG(get_pid("tracee"))`) the output
    will be messed up a bit because the DEBUG tag will be around the lower level output of the logging.
### Cleanup logs
*   Clear unnecessary logging outputs
*   Clearly define what should be put on each log level and enforce (note: TRACE level is not used much)
### ~~Add colors to console output~~
*   It is possible to add colors to the output. 
    Suggestion: color code various outputs and style filenames and line numbers. 
    
## 7. Warnings
### ~~Clean up warnings~~
*   It is important to resolve all of the warnings that are specified in the warnings file provided.
    Either correct them or suppress if they are actually a false positive.
*   This should be done on both Clang and GCC compilers to cover more cases.

## 8. Testing (low priority)
*   Write test cases in a separate document.
*   Test support is needed so choose a library for this.
*   After this write the test cases in C code.

## 9. Run dynamic analysis
*   ~~ASAN~~
*   MSAN (Clang only)
*   ~~UbSAN~~
 
## 10. Documentation
*   Write API instructions and add comments to clarify actions where necessary
*   Create the report

## 11. tracer
### ~~Divide program into parts~~
As it is now the program inspects the binary and the process memory several times unnecessarily.
This could be optimized if the program flow is organized a bit better.
Proposed organization is placed below.
#### ~~Part I: Scanning the CLI options~~
#### ~~Part II: Inspecting the target binary~~
#### ~~Part III: Hooking up to the running process~~
#### ~~Part IV: Inspecting the target process and shared libraries~~
#### ~~Part V: Injecting virus~~
#### ~~Part VI.a: Cleanup option~~
*   Executes the virus
*   Scrubs the virus from memory
*   Reverts the memory permissions as they were before (heap permissions)
*   Frees the allocated memory for the virus
#### ~~Part VI.b: Virus injection option~~
*   Creates a trampoline in the entry function which calls the virus instead of the function
    each time the function is called
#### ~~Part VII: Reverting the process to a state of execution where it was before the tracer attached~~
#### ~~Part VIII: Releasing the traced process~~

## 12. Scanning for prerequisite tools
*   nm - inspects target binary and libc.so
*   cat - read process memory and similar important files
*   grep - filters output for easier managing (consider using scanf instead for this)
*   ldd - lists objects dependencies for dynamic linking
*   pgrep - get process ID from it's name

## 13. Virus
### Support injecting virus function from another binary
*   Virus is injected as the code from a static function in the main file.
    This could be made like loading a binary and executing this code.
    