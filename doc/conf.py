# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file does only contain a selection of the most common options. For a
# full list see the documentation:
# http://www.sphinx-doc.org/en/master/config


# -- Path setup --------------------------------------------------------------


# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
import os


# -- Project information -----------------------------------------------------


# These options are set when cmake configures this file.
project = "@CMAKE_PROJECT_NAME@"
version = "@CMAKE_PROJECT_VERSION@"
release = "@PROJECT_RELEASE@"
author = "@PROJECT_AUTHORS@"
copyright = "@PROJECT_COPYRIGHT@"
description = "@CMAKE_PROJECT_DESCRIPTION@"
source_dir = "@SPHINX_SOURCE_DIR@"
debug_docs = "@CMAKE_BUILD_TYPE@" == "Debug"


# Read the docs executes conf.py directly and so cmake won't have
# configured the above variables :cry: In this case we do the most
# hacky thing possible and search the CMakeLists.txt files for the
# information. If you have a better idea please please tell me.
read_the_docs_build = os.environ.get("READTHEDOCS", None) == "True"
if read_the_docs_build:

    def strip_comments(line):
        # TODO: Support # signs that are in strings and so are not comments.
        return line.split("#")[0].strip()

    cmake_file_path = os.path.abspath(os.path.join("..", "CMakeLists.txt"))
    with open(cmake_file_path) as f:
        text = f.read()

    # The information we're searching for looks like:
    #
    # project(
    #     Template-C
    #     VERSION 0.0.1
    #     DESCRIPTION "A cookie-cutter project for writing C code"
    # )
    # set(PROJECT_AUTHORS "Matt Law")
    # set(PROJECT_RELEASE ${CMAKE_PROJECT_VERSION}-alpha)
    # set(PROJECT_COPYRIGHT "2021, ${PROJECT_AUTHORS}")
    #
    import re

    project_block_match = re.search("project\(([.\s\n\w\"\-\#]*)\)", text)
    if not project_block_match:
        raise RuntimeError(f"Failed to find project definition in {cmake_file_path}")

    print(project_block_match.group(0))

    assert len(project_block_match.groups()) == 1
    project_block = project_block_match.group(1).strip()

    project = strip_comments(project_block.split()[0].strip("\""))

    version_match = re.search("version\s+(.*)\s", project_block, re.IGNORECASE)
    version = strip_comments(version_match.group(1).strip("\"")) if version_match else "0.0.0-unknown"

    description_match = re.search("description\s+(.*)\s", project_block, re.IGNORECASE)
    description = strip_comments(description_match.group(1).strip("\"")) if description_match else ""

    author = re.search("set\(\s*PROJECT_AUTHORS\s+(.*)\)\n", text).group(1)
    author = author.strip("\"")

    release = re.search("set\(\s*PROJECT_RELEASE\s+(.*)\)\n", text).group(1)
    release = release.replace("${CMAKE_PROJECT_VERSION}", version).strip("\"")

    copyright = re.search("set\(\s*PROJECT_COPYRIGHT\s+(.*)\)\n", text).group(1)
    copyright = copyright.replace("${PROJECT_AUTHORS}", author).strip("\"")

    source_dir = "."
    debug_docs = False

    print(f"""Running under read the docs with config:

    project={project}
    version={version}
    description={description}
    author={author}
    release={release}
    copyright={copyright}
    """)

    output_dir = os.path.abspath(os.path.join("..", "build", "doc", "doxygen"))
    input_dir = os.path.abspath(os.path.join("..", "src", "include"))

    # Get the input headers from the project.
    input_files = [
        os.path.join(dirpath, filename)
        for dirpath, _dirnames, filenames in os.walk(input_dir)
            for filename in filenames
    ]

    # Configure doxygen
    with open("doxyfile.in") as file :
        filedata = file.read()

    filedata = filedata.replace("@DOXYGEN_INPUT@", " ".join(input_files)) \
                       .replace("@DOXYGEN_OUTPUT_DIRECTORY@", output_dir) \
                       .replace("@CMAKE_PROJECT_NAME@", project)          \
                       .replace("@CMAKE_PROJECT_VERSION@", version)       \
                       .replace("@CMAKE_PROJECT_DESCRIPTION@", description)

    with open(os.path.join(output_dir, "doxyfile"), "w") as file:
        file.write(filedata)

    # Run doxygen to generate xml.
    import subprocess
    subprocess.run("doxygen", check=True, cwd=output_dir)

    # Tell breathe where to find the generated XML.
    breathe_projects = {project: os.path.join(output_dir, "xml")}


title = f"{project} Documentation"


# -- General configuration ---------------------------------------------------


# If your documentation needs a minimal Sphinx version, state it here.
needs_sphinx = "3.0"

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosectionlabel",
    # Useful for making a console prompt in a code-block un-selectable.
    # https://github.com/sbrunner/sphinx-prompt/
    "sphinx-prompt",
    # The most important extension of all; emoji in documentation.
    # https://sphinxemojicodes.readthedocs.io/en/stable/
    "sphinxemoji.sphinxemoji",
    # https://www.sphinx-doc.org/en/master/usage/extensions/todo.html
    "sphinx.ext.todo",
    # Breathe Configuration
    # https://breathe.readthedocs.io/en/latest/
    "breathe",
]

if debug_docs:
    extensions += ["sphinx.ext.duration"]

breathe_default_project = project
todo_include_todos = debug_docs

# Add any paths that contain templates here, relative to this directory.
templates_path = [os.path.join(source_dir, "_templates")]

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
# source_suffix = ['.rst', '.md']
source_suffix = ".rst"

# The master toctree document.
master_doc = "index"

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = None

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path .
exclude_patterns = []

# The name of the Pygments (syntax highlighting) style to use.
# Some ones I like: sphinx, default, material, vs
pygments_style = "vs"


# -- Options for HTML output -------------------------------------------------


# Configure the appearance of the HTML, see:
# 1. https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# Patch the RTD theme to leave sufficient space between C/C++ types and variables.
html_static_path = [os.path.join(source_dir, "_static")]

# Use an image for the browser tab icon (favicon) and the top-left of the nav bar (logo).
html_favicon = os.path.join(html_static_path[0], "favicon.svg")
html_logo = os.path.join(html_static_path[0], "logo.svg")

# Only show the link to the .rst files in debug mode.
html_show_sourcelink = debug_docs

# Pick a theme for the docs. There's lots of good themes out there.
# This configuration supports two themes:
#   1. Read the docs: https://sphinx-rtd-theme.readthedocs.io/en/stable/
#   2. Material for Sphinx: https://bashtage.github.io/sphinx-material/
# Having tried both, read the docs seems more polished.
use_rdt_theme = True
if use_rdt_theme:
    import sphinx_rtd_theme
    extensions += ["sphinx_rtd_theme"]
    html_theme = "sphinx_rtd_theme"
    html_css_files  = ["style.css"]
    html_theme_options = {
        'collapse_navigation': False,
        'sticky_navigation': True,
        'navigation_depth': 3,
        'includehidden': False,
        'titles_only': False,
        'style_external_links': False,
    }
else: # Use the materials theme
    html_theme = "sphinx_material"
    html_theme_options = {
        # Set the name of the project to appear in the navigation.
        'nav_title': "Readme",
        "touch_icon": html_logo,
        # Set you GA account ID to enable tracking
        # 'google_analytics_account': 'UA-XXXXX',
        # Specify a base_url used to generate sitemap.xml. If not
        # specified, then no sitemap will be built.
        # 'base_url': 'https://project.github.io/project',
        # Set the color and the accent color
        'color_primary': 'blue',
        'color_accent': 'light-blue',
        # Set the repo location to get a badge with stats
        # 'repo_url': 'https://github.com/project/project/',
        'repo_name': project,
        # Visible levels of the global TOC; -1 means unlimited
        # 'globaltoc_depth': 3,
        'globaltoc_collapse': False,        # If False, expand all TOC entries
        'globaltoc_includehidden': False,   # If True, show hidden TOC entries
        'html_minify': not debug_docs,
        'css_minify': not debug_docs,
        'html_prettify': debug_docs,
        "version_dropdown": False
    }

    html_sidebars = {
        "**": ["logo-text.html", "globaltoc.html", "localtoc.html", "searchbox.html"]
    }

# This should be a super nice feature where we can link to manpages using a role.
# But it currently does *not* work :( and instead :manpage:`ls(1)` just links to the root manpage.
# Commented out for now in the hope that Sphinx fixes this in the future.
# Worked around using the sphinxcontrib.manpage package and the :linuxman: role.
# manpages_url = "https://man7.org/linux/man-pages/man3/"
linux_man_url_regex = "https://man7.org/linux/man-pages/man$num/$topic.$num.html"


# -- Options for HTMLHelp output ---------------------------------------------


# Output file base name for HTML help builder.
htmlhelp_basename = f'{project}doc'


# -- Options for LaTeX output ------------------------------------------------


latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, f'{project}.tex', title, author, 'manual'),
]


# -- Options for manual page output ------------------------------------------


# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, project, title, [author], 1),
]


# -- Options for Texinfo output ----------------------------------------------


# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, project, title, author, project, description, 'Miscellaneous'),
]


# -- Options for AppleHelp output --------------------------------------------


applehelp_bundle_id = "0" # set me


# -- Custom roles ------------------------------------------------------------


from docutils import nodes


def setup(app):
    # Define a set of custom roles so that we can link to external websites easily.
    # https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html
    # https://www.sphinx-doc.org/en/master/extdev/appapi.html#sphinx.application.Sphinx.add_role
    app.add_role("cppref", cppref_link("https://en.cppreference.com/w/%s"))
    app.add_role("mscrt", auto_link("https://docs.microsoft.com/en-us/cpp/c-runtime-library/{}"))
    app.add_role("man7", man7_link("https://man7.org/linux/man-pages/man{section}/{page}.{section}.html"))
    return {
        "version": version,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }


def cppref_link(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        assert name == "cppref"

        use_short_text = text.startswith("~")
        if use_short_text:
            text = text[1:] # strip the ~ character.

        url = pattern % (text,)
        if use_short_text:
            text = text.rsplit("/", 1)[-1:][0]

        node = nodes.reference(rawtext, text, refuri=url, **options)
        return [node], []
    return role


def man7_link(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        assert name == "man7"

        use_short_text = text.startswith("~")
        if use_short_text:
            text = text[1:] # strip the ~ character.

        parts = text.rsplit(".")
        if len(parts) != 2:
            msg = inliner.reporter.error(
                f"man7 role expects format a of '<page>.<section>' (e.g. ls.1); "
                f"'{text}' is invalid.", line=lineno)
            prb = inliner.problematic(rawtext, rawtext, msg)
            return [prb], [msg]

        section, number = parts
        text = section if use_short_text else f"{section}({number})"
        url = pattern.format(page=section, section=number)

        node = nodes.reference(rawtext, text, refuri=url, **options)
        return [node], []
    return role


def auto_link(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        url = pattern.format(text)
        node = nodes.reference(rawtext, text, refuri=url, **options)
        return [node], []
    return role
