# TODO

## process_info
*   Get rid of dependency on the target name.
    Work it out so everything works with PID.
    Piping to pgrep is expensive and that's what happens every time you call `get_pid(pid_t)`.
*   `get_function_code`: make a specific error when the function is too big (`> MAX_CODE_LEN`).

## t_error
*   Add support for context reporting in error messages. Currently, what happens is that
    only a message is printed corresponding to the error code. Using macros and structs,
    actual line number and file could be reported with a customized message.
*   Define `t_strerror` function which analogous to the strerror from `string.h` header 
    (outputs the error message).

## process_control
*   This module needs complete refactoring and maybe needs to be separated into two or three
    modules (ptrace wrapper and an API for calling functions
    (e.g JAVA function interfaces like runnable, consumer, producer, function)).
*   Validation of pstate is important before calling functions.

## procmem
*   This module needs a bit of refactoring because a lot of duplicated code is found.
    It would be nice to extract functions that open, prepare and close a pipe.
    
## pstate_t and pstate
*   Check how exactly are structs initialized. It is possible that `create_pstate()` is
    redundant.
*   Write down illegal pstate states and make functions that validate this state.
