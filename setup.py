import sys
from distutils.core import setup, Extension

defines = []

unprocessed_args = []

for arg in sys.argv[1:]:
    if arg == "--pygibson-debug":
        defines.append(("PYGIBSON_DEBUG", "YEAH"))
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
    description='Python client for gibson cache server',
    author='Anton Baklanov',
    author_email='antonbaklanov@gmail.com',
    url='https://github.com/bak1an/pygibson',
    license='MIT',
    ext_modules=[_pygibson],
    package_dir={'': 'src'},
    py_modules=['pygibson'],
    version="0.1.0"
)
