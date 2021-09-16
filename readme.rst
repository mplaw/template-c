Template-C
==========

A cookie-cutter project for writing C code.

This project's documentation is hosted here `template-c.readthedocs.io`_.


|:fast_forward:| Quick start
----------------------------

.. code-block:: console

    $ git clone <url/to>/template-c.git # Download the project.
    $ cd template-c
    $ rm -r .git && git init  # Make it your own project.
    $ ...                     # Make some changes.
    $ git add . && git commit # Commit your changes.
    $ mkdir build && cd build
    $ cmake -GNinja ..        # Configure the build system.
    $ ninja -j16              # Build the code.
    $ ctest -j16              # Run the tests.


|:rocket:| Features
-------------------

- `CMake`_ build system.
- Documentation and test-coverage generation.
- Small stand-alone utility library including a command line interface.
- Small stand-alone test library for unit tests and micro-benchmarks.
- Sensible default configuration for the compiler, `clang-tidy`_ and `clang-format`_.


|:tools:| Tools
---------------

To use this project you'll need:

- A C-compiler, such as `clang`_.
  Any C-compiler will work although `clang`_ works on all platforms and comes with some
  tools that this project supports.
- `cmake`_:
  A cross-platform build-generator tool.
- `ninja`_/`make`_:
  A build tool that integrates with `cmake`_.

On Windows all of these tools can be obtained by installing clang for Windows via Visual Studio,
and most Linux distributions have a system packages for the tools that can be installed easily.

Additionally, the following tools will be helpful when using this project:

- `python 3.6`_ and `Doxygen`_:
  For generating documentation (see the :ref:`Docs` section).
- `clang-tidy`_:
  A static analysis tool for finding bugs in programs without executing the code.
- `clang-format`_:
  A tool for automatically formatting code.

.. note::

  You may need to add these tools to your path to use them easily.


|:building_construction:| Build
-------------------------------

This project uses the `CMake`_ build system generator. The build process is split
into two stages:

#. **Configure**: The build system is generated based on the platform and user
   configuration by running ``cmake``.
#. **Build**: The code is compiled using a build tool such as: ``make`` or ``ninja``.

To generate a release build run:

.. code-block:: console

    $ mkdir build && cd build
    $ cmake -GNinja ..        # configure
    $ ninja -j16              # build

.. note::

  This example uses the ``ninja`` build tool although a different tool can be
  used if its name is passed to cmake with the ``-G`` flag.

In addition to the standard `CMake options`_, the configure stage supports the
following options:

================= ================================================================
Option            Description
================= ================================================================
``BUILD_DOCS``    Build documentation.
``BUILD_TESTING`` Build test code.
``COVERAGE``      Needed to generate test coverage reports; see :ref:`|:see_no_evil:| Coverage`.
``CLANG_TIDY``    Run clang-tidy along-side every compilation step.
``COLOR``         Add colour to the build output.
``UWP``           Compile for the `Universal Windows Platform`_.
================= ================================================================

Any of these options can be set to ``ON`` to enable or ``OFF`` to disable.

For example to create a debug build with test coverage and documentation you
could run:

.. code-block:: console

    $ mkdir build && cd build
    $ cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCS=ON -DBUILD_TESTING=ON -DCOVERAGE=ON ..
    $ ninja -j16


|:computer:| Visual Studio Code
-------------------------------

The project includes a ``.vscode`` folder with some settings to integrate it
nicely with the Microsoft Visual Studio Code IDE. The following extensions
are recommended when using this project:

- `C/C++ <https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools>`_.
- `clang-tidy for VSCode <https://marketplace.visualstudio.com/items?itemName=notskm.clang-tidy>`_.
- `cmake tools <https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools>`_.

The settings included in the ``.vscode`` folder assume that the tools are
locatable using the ``PATH`` environment variable.


|:construction:| To do
----------------------

This project is still very much a work-in-progress and perhaps always will be.
The currently outstanding to-do items are:

- Profiling.
- Make benchmarks much better.
- Continue fleshing out string library.
- Finish CLI: support more actions, unify terminology, make code pretty, more tests.
- Help breathe support doxygen tag files so we can reference cppreference without embedded rst.
- Help sphinx fix man page references so we can use them :)
- Packaging.
- VSCode: make sure things are configured using ``compile_commands.json``.
- Emoji that are compatible with GitHub's reST renderer.
- Test project & documentation on multiple platforms (doesn't look like webp images load in safari).

although there are some more to-do comments scattered throughout the code.


|:pray:| Thanks
---------------

Huge thank you to the authors of all the tools that this project uses.

.. _cmake: https://cmake.org/cmake/help/latest/index.html
.. _CMake: https://cmake.org/cmake/help/latest/index.html
.. _CMake options: https://cmake.org/cmake/help/latest/manual/cmake.1.html#options
.. _clang: https://clang.llvm.org/docs/index.html
.. _clang-tidy: https://clang.llvm.org/extra/clang-tidy/
.. _clang-format: https://clang.llvm.org/docs/ClangFormat.html
.. _Doxygen: https://www.doxygen.nl/index.html
.. _Microsoft Visual Studio Code: https://code.visualstudio.com/
.. _Python 3.6: https://docs.python.org/3.6/
.. _Universal Windows Platform: https://docs.microsoft.com/en-us/windows/uwp/
.. _ninja: https://ninja-build.org/
.. _make: https://www.gnu.org/software/make/
.. _template-c.readthedocs.io: https://template-c.readthedocs.io/en/latest/index.html
