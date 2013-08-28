import sys
from distutils.core import setup, Extension

DESCRIPTION = ''
with open("README.rst", "r") as f:
    DESCRIPTION = f.read()

defines = []

unprocessed_args = []

for arg in sys.argv[1:]:
    if arg == "--pygibson-debug":
        defines.append(('PYGIBSON_DEBUG', "YEAH"))
        continue
    unprocessed_args.append(arg)

sys.argv[1:] = unprocessed_args  # Ludvig Ericson says it is a hack

_pygibson = Extension(
    name="_pygibson",
    sources=[
        "src/_pygibson.c",
        "src/libgibsonclient/src/gibson.c"
    ],
    include_dirs=[
        "src",
        "src/libginsonclient/src"
    ],
    define_macros=defines
)

setup(
    name='pygibson',
    description="Python client for gibson cache server",
    long_description=DESCRIPTION,
    author="Anton Baklanov",
    author_email="antonbaklanov@gmail.com",
    url="https://github.com/bak1an/pygibson",
    license="MIT",
    ext_modules=[_pygibson],
    package_dir={'': "src"},
    py_modules=['pygibson'],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: BSD",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
        "Programming Language :: Python",
        "Programming Language :: Python :: Implementation :: CPython",
        "Topic :: Software Development :: Libraries"
    ],
    version="0.2.0"
)
