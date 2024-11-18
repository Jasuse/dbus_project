#include <algorithm>
#include <array>
#include <iostream>
#include <mutex>
#include <string>

#include "SQLiteCpp/SQLiteCpp.h"
#include "sdbus-c++/sdbus-c++.h"
#include "service.h"

int main() {
    auto service_name = sdbus::ServiceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(service_name);

    auto object_path = sdbus::ObjectPath{"/com/system/permissions"};

    auto service = PermissionsService(*connection, std::move(object_path));

    connection->enterEventLoop();
}