#include "service.h"

static std::string run_cmd(const char* cmd);

TimeService::TimeService(sdbus::IConnection& connection,
                                       sdbus::ObjectPath path)
    : AdaptorInterfaces(connection, std::move(path)),
      ProxyInterfaces(sdbus::ServiceName{"com.system.permissions"}, sdbus::ObjectPath{"/com/system/permissions"}) {
    RegisterAdaptor();
}

TimeService::~TimeService() {
    unregisterAdaptor();
    unregisterProxy();
}

void TimeService::GetSystemTime(sdbus::Result<uint64_t>&& result) {
    auto msg = getObject().getCurrentlyProcessedMessage();
    std::thread([this, result = std::move(result), msg = std::move(msg)]() {
        std::string cmd = "readlink -f /proc/";
        cmd.append(std::to_string(msg.getCredsPid()));
        cmd.append("/exe");
        std::string path = run_cmd(cmd.c_str());
        bool has_perm = false;

        try {
            // ASK: sdbus-cpp claims this is thread safe (commonly)
            // should we put mutex here, just in case
            // or rebuild algorithm alltogether?
            has_perm = CheckApplicationHasPermission(path, 0);
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

        result.returnResults(std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count());
    
    }).detach();
}

static std::string run_cmd(const char* cmd) {
    std::array<char, 256> buffer;
    std::string res;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Failed to execute cmd!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
           nullptr) {
        res += buffer.data();
    }

    // ltrim
    res.erase(res.begin(),
              std::find_if(res.begin(), res.end(),
                           [](unsigned char ch) { return !std::isspace(ch); }));
    // rtrim
    res.erase(std::find_if(res.rbegin(), res.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              res.end());
    return res;
}
