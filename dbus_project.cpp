
#include <iostream>
#include "sdbus-c++/sdbus-c++.h"
int main(int argc, char **argv) {
    sdbus::ServiceName serviceName {"com.system.permissions"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath {"/com/system/permissions"};
    auto permissions = sdbus::createObject(*connection, std::move(objectPath));

    auto request_perm = [&permissions](const std::string& path) {
        auto msg = permissions->getCurrentlyProcessedMessage();
        std::cout << msg.getCredsPid() << "\n";
        throw sdbus::Error(sdbus::Error::Name{"com.system.permissions.UnauthorizedAccess"}, "Application has no permissions");
    };
    permissions->addVTable(
        sdbus::registerMethod("RequestPermission").implementedAs(std::move(request_perm)))
        .forInterface("com.system.Permissions");
    
    connection->enterEventLoop();
}
