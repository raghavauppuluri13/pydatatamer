import os
import sys

from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import os


class BuildPybind(build_ext):

    def run(self):
        # Change directory to 'data_tamer' and run make
        os.chdir('pydatatamer')
        subprocess.check_call(['make'])
        # Change back to the original directory
        os.chdir('..')
        # Proceed with the standard build_ext
        super().run()


setup(
    name="pydatatamer",
    version="0.0.1",
    packages=find_packages(),
    description="Python bindings for MCAP files for data_tamer_cpp",
    url="https://github.com/raghavauppuluri13/pydatatamer",
    author="Raghava Uppuluri",
    install_requires=[
        "numpy",
        "pybind11",
    ],
    cmdclass={
        'build_ext': BuildPybind,
    },
)
