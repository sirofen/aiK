#pragma once

#include <QObject>

#include <aik/global_vars.hpp>
#include <aik/aik_types.hpp>

#include <memory>

enum class AIK_INIT_APPROACH {
    CREATE,
    OPEN
};

class shared_mutex;
class shared_memory;

class aik {
public:
    aik();
    ~aik();

    void start();

    int read_shared_values(DISPATCH_SHARED& _pdispatch_shared_struct);
    int write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct);

    bool init_shared_mutex(LPCTSTR name, AIK_INIT_APPROACH init_appr);
    bool init_shared_memory(LPCTSTR name, AIK_INIT_APPROACH init_appr);

//private:
    std::unique_ptr<shared_mutex> m_shared_mutex;
    std::unique_ptr<shared_memory> m_shared_memory;
};
