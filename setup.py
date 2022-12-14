import os
import sys
import tempfile

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.dist import Distribution

__version__ = '0.1.0'

def has_flag(compiler, flagname):
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    if has_flag(compiler, '-std=c++14'):
        return '-std=c++14'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    raise RuntimeError('Unsupported compiler -- at least C++11 support '
                       'is needed!')


class BuildExt(build_ext):
    c_opts = {'msvc': ['/EHsc'], 'unix': []}
    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts

        try:
            import pybind11
            assert (
                os.path.isfile(os.path.join(pybind11.get_include(), 'pybind11/pybind11.h')) or
                os.path.isfile(os.path.join(pybind11.get_include(True), 'pybind11/pybind11.h'))
            )
        except:
            import pip
            pip.main(['install', 'pybind11>=2.2'])
            sys.modules.pop('pybind11', None)
            import pybind11

        for ext in self.extensions:
            ext.include_dirs.extend([
                pybind11.get_include(),
                pybind11.get_include(True)
            ])

        super().build_extensions()


setup(
    name='levenshteinsgate',
    version=__version__,
    author='aliakseis',
    url='https://github.com/aliakseis/levenshteinsgate',
    description='Levenshtein distance implementation using a trie for fast string similarity searching',
    long_description='',
    package_data={'': [
        'include/levenshteinsgate/trie.h'
    ]},
    include_package_data=True,
    ext_modules=[
        Extension(
            'levenshteinsgate',
            ['src/levenshteinsgate_module.cpp',],
            include_dirs=[
                os.path.join(os.path.abspath(os.path.dirname(__file__)), 'include')
            ],
            language='c++'
        )
    ],
    install_requires=['pybind11 >=2.2'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False
)
