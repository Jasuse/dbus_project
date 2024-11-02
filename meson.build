project('dbus_project', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

sdbus_dep = dependency('sdbus-c++')
sqlite_dep = dependency('sqlitecpp')
executable('PermissionsServer',
           'src/permissions_server/main.cpp', dependencies: [sdbus_dep, sqlite_dep])
executable('Client', 'src/client/main.cpp', dependencies: [sdbus_dep])
executable('TimeServer', 'src/time_server/main.cpp', dependencies : [sdbus_dep])