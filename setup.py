from distutils.core import setup, Extension


_pygibson = Extension(
    name="_pygibson",
    sources=[
        "src/_pygibson.c",
        "src/libgibsonclient/src/gibson.c"
    ],
    include_dirs=[
        "src",
        "src/libginsonclient/src"
    ])

setup(
    name="pygibson",
    version="0.0.1",
    ext_modules=[_pygibson]
)
