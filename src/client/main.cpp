
#include <filesystem>
#include <iostream>
#include <string>

#include "sdbus-c++/sdbus-c++.h"

int main() {
    auto time_proxy = sdbus::createProxy(sdbus::ServiceName{"com.system.time"},
                                         sdbus::ObjectPath{"/com/system/time"});
    sdbus::InterfaceName time_interface{"com.system.Time"};

    auto perms_proxy =
        sdbus::createProxy(sdbus::ServiceName{"com.system.permissions"},
                           sdbus::ObjectPath{"/com/system/permissions"});
    sdbus::InterfaceName perms_interface{"com.system.Permissions"};

    bool success = false;
    while (!success) {
        try {
            std::cout << "Trying getting time from com.system.time\n";
            uint64_t time = 0;
            time_proxy->callMethod("GetSystemTime")
                .onInterface(time_interface)
                .storeResultsTo(time);

            std::time_t t_c = time;
            std::cout << std::put_time(std::localtime(&t_c), "%x %T.") << "\n";
            success = true;
        } catch (const sdbus::Error& err) {
            if (err.getName() == "com.system.permissions.UnauthorizedAccess") {
                std::cout
                    << "This app has no permission, requesting permission\n";

                perms_proxy->callMethod("RequestPermission")
                    .onInterface(perms_interface)
                    .withArguments(0);
            } else {
                std::cout << "Got unexpected error!\n"
                          << "Name: " << err.getName() << "\n"
                          << "Message: " << err.getMessage() << "\n";
                return -1;
            }
        }
    }

    return 0;
}