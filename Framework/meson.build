cnc_tools_framework_include_directories = include_directories('.')

cnc_tools_framework = library(
    'cnc_tools_framework',
    [
        'platformCode.c',
        'storage.c',
        'timing.c',
    ],
    dependencies: threads_dep,
    include_directories: cnc_tools_framework_include_directories,
    install: true,
)

cnc_tools_framework_dep = declare_dependency(
    include_directories: cnc_tools_framework_include_directories,
    link_with: cnc_tools_framework,
    dependencies: threads_dep,
)

subdir('tests')