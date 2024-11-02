
#include <iostream>
#include <string>
#include <filesystem>
#include "sdbus-c++/sdbus-c++.h"

int main(int argc, const char** argv) {

    sdbus::ServiceName destination {"com.system.permissions"};
    sdbus::ObjectPath objectPath {"/com/system/permissions"};

    sdbus::InterfaceName interfaceName{"com.system.Permissions"};

    auto proxy = sdbus::createProxy(std::move(destination), std::move(objectPath));

    {
        std::string res;
        try {
            // hack?
            std::string abs_path = std::string(std::filesystem::current_path()) + "/" + std::string(argv[0]);

            bool has_perm = false;
            std::cout << "Checking permission for file : " << abs_path << "\n";
            proxy->callMethod("CheckApplicationHasPermission")
                    .onInterface(interfaceName)
                    .withArguments(abs_path, 0)
                    .storeResultsTo(has_perm);
                
            std::cout << "Permission check for current program returned : " << has_perm << "\n";

            std::cout << "Requesting permission\n";
            proxy->callMethod("RequestPermission")
                .onInterface(interfaceName)
                .withArguments(0);

            proxy->callMethod("CheckApplicationHasPermission")
                    .onInterface(interfaceName)
                    .withArguments(abs_path, 0)
                    .storeResultsTo(has_perm);
                
            std::cout << "Permission check for current program returned : " << has_perm << "\n";

        } catch (const sdbus::Error& err) {
            std::cout << err.getName() << " " << err.getMessage() << "\n"; 
        }
    }
    
    static_cast<void>(argc);
}