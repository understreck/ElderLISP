project('ElderLISP', 'cpp',
    version : '1.0.1',
    license : 'GPLv3',
    default_options: ['cpp_std=c++20', 'default_library=static'])

cmake = import('cmake')

ctre_proj   = cmake.subproject('ctre')
ctre_dep    = ctre_proj.dependency('ctre')

#catch2_proj = cmake.subproject('Catch2')
#catch2_dep  = catch2_proj.dependency('Catch2')

inc = include_directories('include')

app_sources = ['src/main.cpp']
executable('ElderInterp',
    sources: app_sources,
    include_directories: inc,
    dependencies: [ctre_dep],
    build_by_default: true,
    cpp_pch: ['pch/interp_pch.hpp'],
    cpp_args: ['-Wall', '-Werror'])

configureCompDB = find_program('configure_compilation_database')
run_command(configureCompDB,
    check: true)
