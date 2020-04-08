# Tracer
Tracer is a C program which attaches to a running process (Tracee) and takes control of the execution flow.

## Installation
Use make in order to build the program.
```bash
make
```

## Log levels
There are five log levels (trace, debug, info, warn, error).
Default log levels are defined in the makefile (info and warn) and error log level can't be suppressed.
To suppress or enable other log levels, build the file as follows specifying log levels you want to output.
```bash
make MACROS=TRACE_ENABLE DEBUG_ENABLE INFO_ENABLE
```

## Compilers
You can change the compiler which is used to build the program like so:
```bash
make CC=clang
```
The default compiler is gcc.

## Building the tracee
There is a source code provided in the project folder which can make a suitable Tracee program for testing.
```bash
make tracee
```

## Usage
```bash
./tracer -p <process_name> -b <path_to_process_binary> -e <entry_function> -c 
```
For more information use the ``-h`` option.