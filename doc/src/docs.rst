Docs
====

|:ballot_box_with_check:| Requirements
--------------------------------------

Documentation (like this web-page you're reading now |:smiley:|) is created
using a combination of three core tools:

   - `Doxygen`_
        A powerful tool for automatically generating API documentation
        for several languages. In this project we use it to generate
        XML from C/C++ code that is fed into the other tools.
   - `Sphinx`_
        A Python tool for generating documentation from Python files
        and reStructuredText (reST/.rst) files. The reST files allow for
        manual documentation and further customisation of the layout.
   - `Breathe`_
        A Python package for bridging Doxygen and Sphinx, so that we
        can use Sphinx to generate documentation for C/C++ code.

The build system will create a Python virtual environment and install the
necessary Python packages (see :file:`doc/requirements.txt`), however you
must install Doxygen and Python for this to work.

.. note::

     If you'd rather not build the documentation then configure your build
     with ``-DBUILD_DOCS=OFF`` to disable the generation of the documentation.


|:scroll:| Building Documentation
---------------------------------

To build the documentation ``cmake`` must be configured with ``-DBUILD_DOCS``.

The build system supports generating multiple output types of documentation.
The ``DOCS_OUT_TYPES`` ``cmake`` option takes a list of formats to build.
For example:

     .. code-block:: console

          $ cmake -GNinja -DBUILD_DOCS=ON -DDOCS_OUT_TYPES='html;man' ..
          $ ninja -j16 docs

will build HTML and man-pages documentation. For a full list of supported
formats please see the name tags of the `Sphinx builders`_. You can also
build each documentation type individually using the corresponding
``docs_<type>`` target.

.. note::

     Some output types, such as ``applehelp`` may require extra configuration
     in the ``doc/conf.py`` file.


|:writing_hand:| Writing documentation
--------------------------------------

To write documentation for your project using this system you need to write
reStructuredText files. The Sphinx projects hosts a `nice introduction to reST
<https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html>`_.

To reference code documentation you can use breathe directives which are
`documented here <https://breathe.readthedocs.io/en/latest/directives.html>`_
and you may find it useful to take a look at some of the ``.rst`` files already
in this project.

And finally to document the C code itself you can use `Doxygen comments`_ and
the `Doxygen special commands`_ that Doxygen supports.

.. note::

     For Doxygen to parse a file it must have a doxygen comment like:

     .. code-block:: c

          /// \file
          /// Your file-level documentation here.

     at the top. Without the ``\file`` directive Doxygen will ignore your file.
     The name of the file can be left blank so that Doxygen uses the file name.

In addition to Doxygen special commands you can also embed reStructuredText
into code comments using the ``\rst``, ``\rst_block`` and ``\rst_end`` commands.
For example to embed a sphinx role inline you can do:

.. code-block:: c

     /// Example description...
     ///
     /// Remember to call \rst :cppref:`~c/memory/free` \rst_end.

Or to embed a block of rst you can do:

.. code-block:: c

     /// Example description...
     ///
     /// \rst
     ///
     /// .. code-block:: c
     ///
     ///       int x = 0;
     ///
     /// \rst_end

.. warning::

     Sadly, it is not currently possible to link to external websites using
     Doxygen tagfiles because breathe doesn't support this. See:
     https://github.com/michaeljones/breathe/issues/328.

In addition to the standard `Sphinx roles`_ this project adds three custom roles:

- ``:cppref:`[~]<url/to/page>``` links to the `cppreference`_ website.
- ``:man7:`[~]<page>.<section>``` links to the `man7`_ website.
- ``:mscrt:`[~]<url/to/page>```: links to `Microsoft's C runtime`_ website.

Prefixing the contents with a ``~`` character will shorten the generated url text to just the final
piece of the url or in the case of the ``:man7:`` role just the ``<page>``. For example:

- ``:cppref:`c/chrono/timespec_get``` creates :cppref:`c/chrono/timespec_get`
- ``:cppref:`~c/chrono/timespec_get``` creates :cppref:`~c/chrono/timespec_get`

.. warning::

     Sphinx's ``:manpage:`` role is currently broken; it doesn't create urls correctly and instead
     will always link to the value of ``manpage_url``, hence the need for the ``:man7:`` role.


.. _Doxygen: https://www.doxygen.nl/index.html
.. _Doxygen comments: https://www.doxygen.nl/manual/docblocks.html
.. _Doxygen special commands: https://www.doxygen.nl/manual/commands.html
.. _Sphinx: https://www.sphinx-doc.org/en/master/
.. _Sphinx roles: https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html
.. _Sphinx builders: https://www.sphinx-doc.org/en/master/usage/builders/index.html
.. _Breathe: https://breathe.readthedocs.io/en/latest/
.. _cppreference: https://en.cppreference.com/
.. _man7: https://man7.org/linux/man-pages/
.. _Microsoft's C runtime: https://docs.microsoft.com/en-us/cpp/c-runtime-library
