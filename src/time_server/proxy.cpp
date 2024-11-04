#include "proxy.h"

PermissionsProxy::PermissionsProxy(sdbus::IProxy& proxy) : proxy(proxy) {    
}

bool PermissionsProxy::CheckApplicationHasPermission(std::string path, int permission) {
    bool result = false;
    proxy.callMethod("CheckApplicationHasPermission")
         .onInterface(INTERFACE_NAME)
         .withArguments(path, permission)
         .storeResultsTo(result);
    return result;
}

void PermissionsProxy::RequestPermission(int permission) {
    proxy.callMethod("RequestPermission")
         .onInterface(INTERFACE_NAME)
         .withArguments(permission);
}