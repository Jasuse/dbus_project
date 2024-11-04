#include "sdbus-c++/sdbus-c++.h"
class PermissionsProxy {
    public:
        static constexpr const char* INTERFACE_NAME = "com.system.Permissions";
    protected:
        PermissionsProxy(sdbus::IProxy& proxy);
        PermissionsProxy(const PermissionsProxy&) = delete;
        PermissionsProxy& operator= (const PermissionsProxy&) = delete;

        PermissionsProxy(PermissionsProxy&&) = delete;
        PermissionsProxy& operator=(PermissionsProxy&&) = delete;

        ~PermissionsProxy() = default;
    public:
        void RequestPermission(int permission);
        bool CheckApplicationHasPermission(std::string path, int permission);
    private:
        sdbus::IProxy& proxy;
};