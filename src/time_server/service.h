
#include <iostream>
#include <mutex>

#include "adaptor.h"
#include "proxy.h"

enum Permissions { SystemTime = 0 };
class TimeService : public sdbus::AdaptorInterfaces<TimeServiceAdaptor>,
                    public sdbus::ProxyInterfaces<PermissionsProxy> {
   public:
    TimeService(sdbus::IConnection& connection, sdbus::ObjectPath object);
    ~TimeService();

   protected:
    void GetSystemTime(sdbus::Result<uint64_t>&& res);
};