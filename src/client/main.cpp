
#include <iostream>
#include <string>
#include "sdbus-c++/sdbus-c++.h"

int main() {

    sdbus::ServiceName destination {"com.system.permissions"};
    sdbus::ObjectPath objectPath {"/com/system/permissions"};

    sdbus::InterfaceName interfaceName{"com.system.Permissions"};

    auto proxy = sdbus::createProxy(std::move(destination), std::move(objectPath));

    {
        std::string res;
        try {
            proxy->callMethod("RequestPermission")
                .onInterface(interfaceName)
                .withArguments("0").storeResultsTo(res);
            std::cout << "RequestPermission returned" << res << "\n";
        } catch (const sdbus::Error& err) {
            std::cout << err.getName() << " " << err.getMessage() << "\n"; 
        }
    }
    
    while(true) {}
}