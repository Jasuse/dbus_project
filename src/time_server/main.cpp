
#include <array>
#include <chrono>
#include <iostream>
#include <string>

#include "sdbus-c++/sdbus-c++.h"

std::string get_path_from_ls(std::string ls_out);
std::string run_cmd(const char* cmd);

enum Permissions { SystemTime = 0 };

// TECH DEBT: move all of this to class
int main() {
    // TO DO : shouldn`t we contain     \/\/\/ in meson file?
    sdbus::ServiceName serviceName{"com.system.time"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/time"};
    auto object = sdbus::createObject(*connection, std::move(objectPath));

    sdbus::ServiceName permissionsName{"com.system.permissions"};
    sdbus::ObjectPath permissionsObjectPath{"/com/system/permissions"};
    sdbus::InterfaceName permissionsInterfaceName{"com.system.Permissions"};

    auto proxy = sdbus::createProxy(std::move(permissionsName),
                                    std::move(permissionsObjectPath));

    auto get_system_time_impl = [&object, &proxy,
                                 &permissionsInterfaceName]() -> uint64_t {
        auto msg = object->getCurrentlyProcessedMessage();

        std::string cmd = "readlink -f /proc/";
        cmd.append(std::to_string(msg.getCredsPid()));
        cmd.append("/exe");
        std::string path = run_cmd(cmd.c_str());
        bool has_perm = false;

        try {
            proxy->callMethod("CheckApplicationHasPermission")
                .onInterface(permissionsInterfaceName)
                .withArguments(path, 0)
                .storeResultsTo(has_perm);
        } catch (const sdbus::Error& err) {
            if (err.getName() == "org.freedesktop.DBus.Error.ServiceUnknown") {
                // THINK ABOUT IT : who is responsible for throwing/handling
                // "Not Available" error?
                throw sdbus::Error(
                    sdbus::Error::Name{"com.system.permissions.NotAvailable"},
                    "Permission server is not available");
            } else {
                throw sdbus::Error(
                    sdbus::Error::Name{"com.system.time.InternalError"},
                    "Unhandled exception occured!");
            }
        }

        if (!has_perm) {
            throw sdbus::Error(
                sdbus::Error::Name{"com.system.permissions.UnauthorizedAccess"},
                "Application has no permission");
        }

        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    };

    object
        ->addVTable(sdbus::registerMethod("GetSystemTime")
                        .implementedAs(std::move(get_system_time_impl)))
        .forInterface("com.system.Time");

    connection->enterEventLoop();
}

std::string run_cmd(const char* cmd) {
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

std::string get_path_from_ls(std::string pwdx_out) {
    auto separator_idx = pwdx_out.find(">");
    if (separator_idx == std::string::npos) {
        return "";
    }
    return std::string(pwdx_out.cbegin() + separator_idx + 1 + 1,
                       pwdx_out.cend());
}
