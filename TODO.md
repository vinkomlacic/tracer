# TODO

## 1. process_info
### Get rid of dependency on the target name
*   Work it out so everything works with PID.
    Piping to pgrep is expensive and that's what happens every time you call `get_pid(pid_t)`.

### Add a specific error  
*   `get_function_code`: make a specific error when the function is too big (`> MAX_CODE_LEN`).

## 2. t_error
### Contextual error reporting
*   Add support for context reporting in error messages. Currently, what happens is that
    only a message is printed corresponding to the error code. Using macros and structs,
    actual line number and file could be reported with a customized message.
    
### `t_strerror`
*   Define `t_strerror` function which is analogous to the strerror from `string.h` header 
    (returns the error message for a certain error code).

## 3. process_control
### General refactoring
*   This module needs complete refactoring and maybe needs to be separated into two or three
    modules (ptrace wrapper and an API for calling functions
    (e.g JAVA function interfaces like runnable, consumer, producer, function)).
 
### Validation
*   Validation of pstate is important before calling functions.

## 4. procmem
### General refactoring
*   This module needs a bit of refactoring because a lot of duplicated code is found.
    It would be nice to extract functions that open, prepare and close a file.
    
## 5. pstate_t and pstate
### Struct initialization
*   Check how exactly are structs initialized. It is possible that `create_pstate()` is
    redundant. Maybe using designated (or not) initializers could be beneficial.

### Validation 
*   Write down illegal pstate states and make functions that validate this state. Declare error codes and 
    define what exactly is an uninitialized state and how state must look like to be valid.
