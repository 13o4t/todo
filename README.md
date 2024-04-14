# Todo

A simple todo list app for managing [todo.txt](https://github.com/todotxt/todo.txt) file, support plugins.

## Contents

* [Installation](#installation)
  * [Download the latest release](#download-the-latest-release)
  * [Build from source](#build-from-source)
* [Key features](#key-features)
* [Quick start](#quick-start)
* [Usage](#usage)
* [Plugins](#plugins)
* [License](#license)

### Installation

#### Download the latest release

You can [download](https://github.com/13o4t/todo/releases/latest) the latest version for Windows, macOS and Linux.

#### Build from source

```
$ git clone https://github.com/13o4t/todo.git
$ cd todo
$ cmake -S . -B ./build
$ cmake --build ./build --config Release
$ cmake --install ./build --prefix "/path/to/install"
```

### Key features

- Due dates

  Support special tag `due` for specify expiration date, for due tomorrow, can use `due:tomorrow` or `due:tom`.

- Plugins

  See [Plugins](#plugins).

### Quick start

1. Add a task to list.

  ```
  $ todo add "[A] Task for +project @content due:tomorrow"
  ```

2. List tasks.

  ```
  $ todo list
  1 Task for +project @content due:1970-01-01
  ```

3. Mark task completed.

  ```
  $ todo done 1
  ```

### Usage

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

Due date format:
```
due:1970-01-01
due:today due:tod     // due today
due:tomorrow due:tom  // due tomorrow
due:sunday due:sun    // due next sunday
due:monday due:mon    // due next monday
due:tuesday due:tue   // due next tuesday
due:wednesday due:wed // due next wednesday
due:thursday due:thu  // due next thursday
due:friday due:fri    // due next friday
due:saturday due:sat  // due next saturday
due:week              // due this week
due:month             // due this month
```

List with filter:
```
$ todo list +project
$ todo list @context
$ todo list due:week
```

### Plugins

See [plugins/README.md](plugins/README.md)

### License

MIT