#include "adaptor.h"

TimeServiceAdaptor::TimeServiceAdaptor(sdbus::IObject& object)
    : object(object) {}

void TimeServiceAdaptor::RegisterAdaptor() {
    object
        .addVTable(sdbus::registerMethod("GetSystemTime")
                       .implementedAs(
                           [this](sdbus::Result<uint64_t>&& result) -> void {
                               GetSystemTime(std::move(result));
                           }))
        .forInterface(INTERFACE_NAME);
}