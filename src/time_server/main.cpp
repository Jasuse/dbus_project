
#include <array>
#include <chrono>
#include <iostream>
#include <string>

#include "sdbus-c++/sdbus-c++.h"
#include "service.h"

int main() {
    auto service_name = sdbus::ServiceName{"com.system.time"};
    auto connection = sdbus::createBusConnection(service_name);

    auto object_path = sdbus::ObjectPath{"/com/system/time"};

    auto service = TimeService(*connection, std::move(object_path));

    connection->enterEventLoop();
}