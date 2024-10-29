project('dbus_project', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

sdbus_dep = dependency('sdbus-c++')

executable('dbus_project',
           'dbus_project.cpp', dependencies: [sdbus_dep])

executable('dbus_client', 'dbus_client.cpp', dependencies: [sdbus_dep])