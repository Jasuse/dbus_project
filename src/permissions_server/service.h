
#include <iostream>
#include <mutex>

#include "SQLiteCpp/SQLiteCpp.h"
#include "adaptor.h"

enum Permissions { SystemTime = 0, MAX };
class PermissionsService
    : public sdbus::AdaptorInterfaces<PermissionsServiceAdaptor> {
   public:
    PermissionsService(sdbus::IConnection& connection,
                       sdbus::ObjectPath object);
    ~PermissionsService();

   protected:
    void RequestPermission(sdbus::Result<>&& result, int permission) override;
    void CheckApplicationHasPermission(sdbus::Result<bool>&& result,
                                       std::string path,
                                       int permission) override;

   private:
    SQLite::Database db;
    std::mutex db_mutex;

    bool CheckPermissionInternal(const SQLite::Database& db,
                                 const std::string& path, int permission);
};