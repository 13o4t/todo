# Todo

A simple todo list app for managing [todo.txt](https://github.com/todotxt/todo.txt) file, support plugins.

## Usage

```
Usage: todo [OPTIONS] SUBCOMMAND

Options:
  -h,--help                   Print this help message and exit
  -f,--file TEXT              Output filepath
  --log_level INT:INT in [0 - 6] [2] 
                              Set log level

Subcommands:
  add                         Add todo task
  done                        Done todo task
  del                         Delete todo task
  list                        List todos
  archive                     Archive task
```

## Plugins

See [plugins/README.md](plugins/README.md)