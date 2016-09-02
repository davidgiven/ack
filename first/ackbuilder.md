ackbuilder
==========

# ackbuilder

## What is it?

ackbuilder is a very small build tool inspired by [bazel](https://bazel.io/)
which uses either make or [ninja](https://ninja-build.org/) as a backend.

It supports fully parallelisable builds (in both make and ninja), as well as
hopefully-robust support for rules which generate more than one output file,
which is something make is very, very bad at.

It was written because the ACK is a really horribly complex thing to build and
there wasn't anything else.

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

Rule properties are typed and can be specified to be required or optional (or have a default value).

    definerule("sort",
	  {
	     srcs = { type="targets" },
		 numeric = { type="boolean", default=false }
	  }
	  ...omitted...

The most common one is `targets`. When the rule is invoked, ackbuilder will
resolve these for you so that when your callback fires, the property is a
flattened list of target objects.


