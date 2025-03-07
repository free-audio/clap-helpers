project('clap-helpers', ['c','cpp'],
    version : '1.2.3',
    meson_version : '>= 0.58.0',
    default_options : [ 
        'c_std=c11', 
        'cpp_std=c++20', 
        'buildtype=release' ])

clap_dep = dependency ('clap')
clap_helpers_dep = declare_dependency (
    include_directories: include_directories ('include')
)
