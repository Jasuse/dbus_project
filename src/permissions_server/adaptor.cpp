#include "adaptor.h"

PermissionsServiceAdaptor::PermissionsServiceAdaptor(sdbus::IObject& object)
    : object(object) {}

void PermissionsServiceAdaptor::RegisterAdaptor() {
    object
        .addVTable(sdbus::registerMethod("RequestPermission")
                       .implementedAs([this](sdbus::Result<>&& result,
                                             int permission) -> void {
                           RequestPermission(std::move(result), permission);
                       }),
                   sdbus::registerMethod("CheckApplicationHasPermission")
                       .implementedAs([this](sdbus::Result<bool>&& result,
                                             std::string path, int permission) {
                           return CheckApplicationHasPermission(
                               std::move(result), path, permission);
                       }))
        .forInterface(INTERFACE_NAME);
}