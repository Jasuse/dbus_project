
#include "sdbus-c++/sdbus-c++.h"
class PermissionsServiceAdaptor {
   public:
    static constexpr const char* INTERFACE_NAME = "com.system.Permissions";

   protected:
    PermissionsServiceAdaptor(sdbus::IObject& object);
    PermissionsServiceAdaptor(const PermissionsServiceAdaptor&) = delete;
    PermissionsServiceAdaptor& operator=(const PermissionsServiceAdaptor&) =
        delete;

    PermissionsServiceAdaptor(PermissionsServiceAdaptor&&) = delete;
    PermissionsServiceAdaptor& operator=(PermissionsServiceAdaptor&&) = delete;

    ~PermissionsServiceAdaptor() = default;

    void RegisterAdaptor();
    virtual void RequestPermission(sdbus::Result<>&& result,
                                   int permission) = 0;
    virtual void CheckApplicationHasPermission(sdbus::Result<bool>&& result,
                                               std::string path,
                                               int permission) = 0;

   private:
    sdbus::IObject& object;
};