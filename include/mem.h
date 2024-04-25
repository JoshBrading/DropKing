#ifndef MEM_H
#define MEM_H
#include <iostream>

class MemoryManager
{
public:
    static MemoryManager& instance()
    {
        static MemoryManager instance;
        return instance;
    }
    static int get_usage() { return instance().i_get_usage();}
    static void add_usage(const int bytes) { instance().i_add_usage(bytes);}
    static void remove_usage(const int bytes) { instance().i_remove_usage(bytes);}
private:
    MemoryManager() = default;
        
    int total_allocated = 0;
    int total_free = 0;
    
    void i_add_usage(const int bytes)
    {
        total_allocated += bytes;
    }
    void i_remove_usage(const int bytes)
    {
        total_free += bytes;
    }
    
    int i_get_usage() const
    {
        std::cout << "Memory Used: " << total_allocated - total_free << " bytes.\n";
        return total_allocated - total_free;
    }
};
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
// ReSharper disable CppParameterNamesMismatch
void* operator new(const size_t size)  // NOLINT(misc-new-delete-overloads)
{
    MemoryManager::add_usage(static_cast<int>(size));
    return malloc(size);
}

void operator delete(void* memory, const size_t size)  // NOLINT(clang-diagnostic-implicit-exception-spec-mismatch)
{
    MemoryManager::remove_usage(static_cast<int>(size));
    free(memory);
}
// ReSharper restore CppParameterNamesMismatch
// ReSharper restore CppNonInlineFunctionDefinitionInHeaderFile
#endif // MEM_H
