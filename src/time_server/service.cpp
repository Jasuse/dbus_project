#include "service.h"

#include <filesystem>

TimeService::TimeService(sdbus::IConnection& connection, sdbus::ObjectPath path)
    : AdaptorInterfaces(connection, std::move(path)),
      ProxyInterfaces(sdbus::ServiceName{"com.system.permissions"},
                      sdbus::ObjectPath{"/com/system/permissions"}) {
    RegisterAdaptor();
}

TimeService::~TimeService() {
    unregisterAdaptor();
    unregisterProxy();
}

void TimeService::GetSystemTime(sdbus::Result<uint64_t>&& result) {
    auto msg = getObject().getCurrentlyProcessedMessage();
    std::thread([this, result = std::move(result), msg = std::move(msg)]() {
        std::string str_path = "";
        str_path.append("/proc/");
        str_path.append(std::to_string(msg.getCredsPid()));
        str_path.append("/exe");
        std::filesystem::path p(std::move(str_path));
        std::string path = std::filesystem::read_symlink(p);
        bool has_perm = false;

        try {
            // ASK: sdbus-cpp claims this is thread safe (commonly)
            // should we put mutex here, just in case
            // or rebuild algorithm alltogether?
            has_perm =
                CheckApplicationHasPermission(path, Permissions::SystemTime);
        } catch (const sdbus::Error& err) {
            if (err.getName() == "org.freedesktop.DBus.Error.ServiceUnknown") {
                result.returnError(sdbus::Error(
                    sdbus::Error::Name{"com.system.permissions.NotAvailable"},
                    "Permission server is not available"));
            } else {
                result.returnError(sdbus::Error(
                    sdbus::Error::Name{"com.system.time.InternalError"},
                    "Unhandled exception occured!"));
            }
        }

        if (!has_perm) {
            result.returnError(sdbus::Error(
                sdbus::Error::Name{"com.system.permissions.UnauthorizedAccess"},
                "Application has no permission"));
        }

        result.returnResults(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());
    }).detach();
}