
#include "sdbus-c++/sdbus-c++.h"
class TimeServiceAdaptor {
   public:
    static constexpr const char* INTERFACE_NAME = "com.system.Time";

   protected:
    TimeServiceAdaptor(sdbus::IObject& object);
    TimeServiceAdaptor(const TimeServiceAdaptor&) = delete;
    TimeServiceAdaptor& operator=(const TimeServiceAdaptor&) = delete;

    TimeServiceAdaptor(TimeServiceAdaptor&&) = delete;
    TimeServiceAdaptor& operator=(TimeServiceAdaptor&&) = delete;

    ~TimeServiceAdaptor() = default;

    void RegisterAdaptor();
    virtual void GetSystemTime(sdbus::Result<uint64_t>&& res) = 0;

   private:
    sdbus::IObject& object;
};