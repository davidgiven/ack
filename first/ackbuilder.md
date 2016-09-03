# ackbuilder

## What is it?

ackbuilder is a very small build tool inspired by [bazel](https://bazel.io/)
which uses either make or [ninja](https://ninja-build.org/) as a backend.

It supports fully parallelisable builds (in both make and ninja), as well as
hopefully-robust support for rules which generate more than one output file,
which is something make is very, very bad at.

It was written because the ACK is a really horribly complex thing to build and
there wasn't anything else. ackbuilder is pretty rough and ready but it does
sort of work. Be prepared for bugs.

This document is a very rapid brain dump of how the build file works. It
doesn't cover actually running the tool (because that bit's pretty nasty) ---
go look at the top level Makefile to see that for now.

## Basic concepts

Complete example, using the built-in C rules. This should be saved in a file
called `build.lua`:

    cprogram {
      name = 'prog',
      srcs = { "./*.c" },
    }

This defines a rule `prog` which, when built, compiles all the source files in
the same directory as the `build.lua` file into an executable.

Slightly more complex example:

    clibrary {
      name = "library",
      srcs = { "./library.c" },
      hdrs = { "./library.h" },
    }

    cprogram {
      name = 'prog2',
      srcs = { "./prog2.c" },
      deps = { "+library" }
    }

If we move the library into another directory, we can invoke it like this:

    cprogram {
      name = 'prog3',
      srcs = { "./prog3.c" },
      deps = { "path/to/library+library" }
    }

  * Targets starting with `./` are relative to **the current directory** (i.e.
    the one the build file is in).

  * Targets starting with a path are relative to the top directory of the
    project.

  * Targets containing a `+` refer to a named target in another build file. So,
    on encountering the library in `prog3` above, ackbuilder will look for
    `path/to/library/build.lua`, load it, and then try to find a target in it
    called `library`.

**Warning**: files are interpreted from top to bottom; every time a target
referring to another build file is seen for the first time, that file is
interpreted then and there. You can't have circular dependencies (these are
caught and an error is generated). You can't refer to a target defined below
you in the same source file (these are not caught, and just won't be found).

Build files each get their own private global scope. If you randomly set a
variable, it won't be seen by other build files. (Use `vars` for that; see
below.) Build files are only loaded once.

The `cprogram` and `clibrary` rules, by the way, are sophisticated enough to
automatically handle library and header paths. The exported headers by the
library are automatically imported into the program.

## `simplerule` and `normalrule`

These are the building blocks out of which all other rules are made. If you
want to run your own programs, you will be using these.

`simplerule` is the simplest. You give it inputs, and outputs, and commands,
and it does it.

    simplerule {
      name = 'sorted-input',
      ins = { './input.txt' },
      outs = { './output.txt' },
      commands = {
        "sort < %{ins} > %{outs}"
      }
    }

In a command block, `%{...}` will evaluate the Lua expression between the
braces; various useful things are in scope, including the list of inputs and
outputs.

However, this ends up leaving the output file lying around in the project
directory, which we don't want, so we usually use `normalrule` instead.
(`normalrule` is not strictly part of the ackbuilder core; it's in the standard
library along with `cprogram` and `clibrary`.)

    normalrule {
      name = 'sorted-input',
      ins = { './input.txt' },
      outleaves = { 'output.txt' },
      commands = {
        "sort < %{ins} > %{outs}"
      }
    }

Note `outleaves`; there is no `./`. This is a list of leaf filenames. The rule
will create a directory in the object tree and put the files specified in it,
somewhere; you don't care where. You can refer to the output file via the
target name, so:

    normalrule {
      name = 'reversed',
      ins = { '+sorted-input' },
      outleaves = { 'reversed.txt' },
      commands = {
        "rev < %{ins} > %{outs}"
      }
    }

One common use for this is to generate C header or source files.

    normalrule {
      name = 'reversed_h',
      ins = { '+reversed' },
      outleaves = { 'reversed.h' },
      commands = {
        'xxd -i %{ins} > %{outs}'
      }
    }

    cprogram {
      name = 'prog',
      srcs = { './*.c' },
      deps = { '+reversed_h' }
    }

Now you can refer to `reversed.h` in one of your C files and it'll just work
(`+reversed_h`'s output directory gets added to the include path
automatically).

## Defining your own rules

Like this:

    definerule("sort",
      {
        srcs = { type="targets" },
      },
      function(e)
        return normalrule {
          name = e.name,
          ins = e.srcs,
          outleaves = { 'sorted.txt' },
          commands = {
            "sort < %{ins} > %{outs}"
          }
        }
      }
    )

    sort {
      name = 'sorted',
      srcs = { './input.txt' }
    }

You give `definerule()` the name of the rule you want to define, a description
of the properties the rule will take, and a callback that does the work.

You can do anything you like in the callback, including defining as many
targets as you like; but remember that all targets must have unique names, so
for any temporary files you probably want something like `name =
e.name.."/intermediate"` to ensure uniqueness.

The callback should end by returning an invocation of another rule, with `name
= e.name` as above.

Rules are defined whenever a build file containing them is seen. Letting this
happen automatically doesn't always work so you probably want to explicitly
include it:

    include("foo/bar/baz/build.lua")

Rule properties are typed and can be specified to be required or optional (or
have a default value).  If you try to invoke a rule with a property which isn't
declared, or missing a property which should be declared, you'll get an error.

    definerule("sort",
    {
      srcs = { type="targets" },
      numeric = { type="boolean", optional=true, default=false }
    }
    ...omitted...

(The `optional=true` part can be omitted if you specify a default which isn't
`nil`.)

Types include:

  * `targets`: the most common one. When the rule is invoked, ackbuilder will
    resolve these for you so that when your callback fires, the property is a
    flattened list of target objects.

  * `strings`: a Lua table of strings. If the invoker supplies a single string
    which isn't a table, it'll get wrapped in one.

  * `string`: a string.

  * `boolean`: a boolean (either `true` or `false`; nothing else is allowed).

  * `table`: a Lua table.

  * `object`: any Lua value.

## Target objects

When a rule callback is run, any targets it needs will be resolved into target
objects. These are Lua objects with assorted useful stuff in them.

  * `object.is`: contains a set telling you which rules made the object. e.g.
    `object.is.cprogram` is true if `object` was built with `cprogram`. Bear in
    mind that `object.is.normalrule` is _also_ going to be true.

  * `object.dir`: the object's build directory. Only exists if the object was
    built with `normalrule`.

There are other properties (`fullname` and `outs`). Please don't use these; use
`targetnamesof()` and `filenamesof()` as described below.

## The standard library

Your build files are supplied a pile of useful functions.

### Manipulating target lists

A target list is a possibly nested set of tables containing either target
objects or strings. All of these functions will implicitly flatten the list and
resolve any strings into target objects before doing anything else to them.
Most of these functions can be supplied with varargs parameters.

e.g. `targetsof(a, b)` is equivalent to `targetsof({a, b})` is equivalent to
`targetsof({a, {b}})`.

  * `targetsof(...)`: just flattens the list and resolves any string target
    names.

  * `filenamesof(...)`: returns a list of output files for all the supplied
    targets.

  * `targetnamesof(...)`: returns a list of fully qualified target names for
    all the supplied stargets.

  * `selectof(targets, pattern)`: returns only those targets whose outputs
    contain at least one file matching the pattern.

### Manipulating filename lists

Like the target list functions, all of these implicitly flatten any nested
tables. They all return lists; however, as a special exception, if any of the
functions which take varargs parameters have a single parameter which is a
string, they return just a string.

e.g. `abspath({f})` returns a table; `abspath(f)` returns a string.

  * `abspath(...)`: attempts to return the absolute path of its arguments. This
    isn't always possible due to variable references.

  * `basename(...)`: returns the basenames of its arguments (the file part of
    the path).

  * `dirname(...)`: returns the directory name of its arguments.

  * `matching(files, pattern)`: returns only those files which match a Lua
    pattern.

  * `replace(files, pattern, repl)`: performs a Lua pattern replace on the list
    of files.

  * `uniquify(...)`: removes duplicates.

### Other things

  * `include(file)`: loads another build file, if it hasn't been loaded before.

## Variables

There are two types of variable, mostly for hysterical reasons.

### Makefile variables

(Despite the name, these work on ninja too.)

Filenames can contain variable references of the form `$(FOO)`. These are
expanded at build time based on definitions supplied on the ackbuilder command
line.

ackbuilder assumes that these are absolute paths and won't attempt to
manipulate them much.

I want to get rid of these at some point.

### ackbuilder variables

These are expanded by ackbuilder itself.

Every rule invocation contains a magic property, `vars`. When a rule's commands
are executed, the variables provided in the template expansion are calculated
by combining all `vars` settings in the call stack (including the top level
build file).

Easiest to explain with an example:

    cprogram {
      name = 'another_test',
      srcs = { './*.c' },
      vars = {
        cflags = { '-g', '-O3' }
      }
    }

When `cprogram` builds each C file, the command will refer to `%{cflags}`. The
value above will be flattened into a space-separated string and substituted in.

Setting a variable this way will _override_ any definition further up the call
stack. However, you can do this:

    vars.cflags = { '-g' }

    cprogram {
      name = 'another_test',
      srcs = { './*.c' },
      vars = {
        ["+cflags"] = { '-O3' }
      }
    }

Now `cflags` will default to `-g` everywhere, because it's set at the top
level; but when `another_test` is built, it'll be `-g -O3`.

ackbuilder variables are only expanded in command templates, not in filenames.

<!-- # vim: set ts=2 sw=2 expandtab : -->

