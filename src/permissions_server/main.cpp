
#include <iostream>
#include "sdbus-c++/sdbus-c++.h"
int main() {
    // TO DO : shouldn`t we contain     \/\/\/ in meson file?
    sdbus::ServiceName serviceName {"com.system.permissions"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath {"/com/system/permissions"};
    auto permissions = sdbus::createObject(*connection, std::move(objectPath));

    auto request_perm_impl = [&permissions]() {
        auto msg = permissions->getCurrentlyProcessedMessage();
        std::cout << msg.getCredsPid() << "\n";
        throw sdbus::Error(sdbus::Error::Name{"com.system.permissions.UnauthorizedAccess"}, "Application has no permissions");
    };
    permissions->addVTable(
        sdbus::registerMethod("RequestPermission").implementedAs(std::move(request_perm_impl)))
        .forInterface("com.system.Permissions");
    
    connection->enterEventLoop();
}
