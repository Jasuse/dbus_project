
#include <iostream>
#include "sdbus-c++/sdbus-c++.h"
#include "SQLiteCpp/SQLiteCpp.h"

#include <string>
#include <array>
#include <mutex>
#include <algorithm>
std::string get_path_from_ls(std::string ls_out);
std::string run_cmd(const char* cmd);

bool check_permission_internal(const SQLite::Database& db, const std::string& path, int permission);

enum Permissions {
    SystemTime = 0
};

//TECH DEBT: move all of this to class
int main() {
    // TO DO : shouldn`t we contain     \/\/\/ in meson file?
    sdbus::ServiceName serviceName {"com.system.permissions"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath {"/com/system/permissions"};
    auto permissions = sdbus::createObject(*connection, std::move(objectPath));

    // SQLite supports multithreading, but when we are not making multiple connections
    // which (I assume) we do
    std::mutex db_mutex;
    SQLite::Database db ("permissions.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    
    // Maybe we can use hashmap(with lifetime?) to cache PID`s with permissions
    db.exec("CREATE TABLE IF NOT EXISTS permissions (path NVARCHAR(255), permissions INTEGER)");
    
    auto request_perm_impl = [&permissions, &db_mutex, &db](int permission) {
        if(permission > 0) {
            throw
                sdbus::Error(sdbus::Error::Name{"com.system.permissions.InvalidPermission"},
                     "There is no such permission");
        }
        auto msg = permissions->getCurrentlyProcessedMessage();

        std::string cmd = "readlink -f /proc/";
        cmd.append(std::to_string(msg.getCredsPid()));
        cmd.append("/exe");
        std::string path = run_cmd(cmd.c_str());

        std::lock_guard<std::mutex> db_guard(db_mutex);
        if(check_permission_internal(db, path, permission)) {
            return;
        }

        SQLite::Statement query {db, "INSERT INTO permissions (path, permissions) VALUES (?,?) "};
        query.bind(1, path);
        query.bind(2, permission);
        try {
            query.exec();
        } catch(const SQLite::Exception& exc) {
            std::cout << "Caught SQLite exception (a bug): " << exc.getErrorStr() ;
        }
    };

    auto check_permission_impl = [&permissions, &db_mutex, &db](std::string path, int permission) {
        if(permission > 0) {
            throw
                sdbus::Error(sdbus::Error::Name{"com.system.permissions.InvalidPermission"},
                     "There is no such permission");
        }

        std::lock_guard<std::mutex> db_guard(db_mutex);
        return check_permission_internal(db, path, permission);
    };

    
    permissions->addVTable(
        sdbus::registerMethod("RequestPermission").implementedAs(std::move(request_perm_impl)),
        sdbus::registerMethod("CheckApplicationHasPermission").implementedAs(std::move(check_permission_impl)))
        .forInterface("com.system.Permissions");
    
    connection->enterEventLoop();
}

std::string run_cmd(const char* cmd) {
    std::array<char, 256> buffer;
    std::string res;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Failed to execute cmd!");
    }
    while(fgets(buffer.data(), static_cast<int> (buffer.size()), pipe.get()) != nullptr) {
        res += buffer.data();
    }

    // ltrim
    res.erase(res.begin(),
                std::find_if(res.begin(), res.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
    // rtrim
    res.erase(
            std::find_if(res.rbegin(), res.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), res.end());
    return res;
}

std::string get_path_from_ls(std::string pwdx_out) {
    auto separator_idx = pwdx_out.find(">");
    if(separator_idx == std::string::npos) {
        return "";
    }
    return std::string(pwdx_out.cbegin() + separator_idx + 1 + 1, pwdx_out.cend());
}

bool check_permission_internal(const SQLite::Database& db, const std::string& path, int permission) {
    SQLite::Statement query {db, "SELECT COUNT(*) FROM permissions WHERE path = (?) AND permissions = (?)"};
    query.bind(1, path);
    query.bind(2, permission);

    int count = 0;
    try {
        if (query.executeStep()) {
            count = query.getColumn(0);
        }
    } catch(const SQLite::Exception& exc) {
        std::cout << "Caught SQLite exception (a bug): " << exc.getErrorStr() ;
    }
    return count > 0;
}