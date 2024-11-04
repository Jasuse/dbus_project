#include "service.h"

static std::string run_cmd(const char* cmd);

PermissionsService::PermissionsService(sdbus::IConnection& connection,
                                       sdbus::ObjectPath path)
    : AdaptorInterfaces(connection, std::move(path)),
      db("permissions.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    RegisterAdaptor();

    db.exec(
        "CREATE TABLE IF NOT EXISTS permissions (path NVARCHAR(255), "
        "permissions INTEGER)");
}

PermissionsService::~PermissionsService() { unregisterAdaptor(); }

void PermissionsService::RequestPermission(sdbus::Result<>&& result,
                                           int permission) {
    auto msg = getObject().getCurrentlyProcessedMessage();
    std::thread([this, result = std::move(result),
                 permission = std::move(permission), msg = std::move(msg)]() {
        if (permission >= Permissions::MAX) {
            result.returnError(sdbus::Error(
                sdbus::Error::Name{"com.system.permissions.InvalidPermission"},
                "There is no such permission"));
            return;
        }

        std::string cmd = "readlink -f /proc/";
        cmd.append(std::to_string(msg.getCredsPid()));
        cmd.append("/exe");
        std::string path = run_cmd(cmd.c_str());

        std::lock_guard<std::mutex> db_guard(this->db_mutex);
        if (CheckPermissionInternal(db, path, permission)) {
            return;
        }

        SQLite::Statement query{
            db, "INSERT INTO permissions (path, permissions) VALUES (?,?) "};
        query.bind(1, path);
        query.bind(2, permission);
        try {
            query.exec();
        } catch (const SQLite::Exception& exc) {
            std::cout << "Caught SQLite exception (a bug): "
                      << exc.getErrorStr();
        }
        result.returnResults();
    }).detach();
}
void PermissionsService::CheckApplicationHasPermission(
    sdbus::Result<bool>&& result, std::string path, int permission) {
    std::thread([this, result = std::move(result), path = std::move(path),
                 permission = std::move(permission)]() {
        if (permission >= Permissions::MAX) {
            result.returnError(sdbus::Error(
                sdbus::Error::Name{"com.system.permissions.InvalidPermission"},
                "There is no such permission"));
            return;
        }

        std::lock_guard<std::mutex> db_guard(db_mutex);
        result.returnResults(CheckPermissionInternal(db, path, permission));
    }).detach();
}

bool PermissionsService::CheckPermissionInternal(const SQLite::Database& db,
                                                 const std::string& path,
                                                 int permission) {
    SQLite::Statement query{db,
                            "SELECT COUNT(*) FROM permissions WHERE path = (?) "
                            "AND permissions = (?)"};
    query.bind(1, path);
    query.bind(2, permission);

    int count = 0;
    try {
        if (query.executeStep()) {
            count = query.getColumn(0);
        }
    } catch (const SQLite::Exception& exc) {
        std::cout << "Caught SQLite exception (a bug): " << exc.getErrorStr();
    }
    return count > 0;
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
