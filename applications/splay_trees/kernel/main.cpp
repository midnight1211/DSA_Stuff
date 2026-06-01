#include "tree.h"

int main() {
    KernelVMM vmm;

    // Simulate process layout allocation maps
    vmm.mmap_allocate(0x00400000, 0x004FFFFF, "executable_text_segment");
    vmm.mmap_allocate(0x7FFF0000, 0x7FFFFFFF, "process_stack_space");
    vmm.mmap_allocate(0x10000000, 0x1FFFFFFF, "dynamic_heap_allocations");

    // Simulate standard code loops accessing variables inside memory pools
    std::vector<unsigned long long> cpu_instruction_fetches = {
        0x7FFF00A4, // Stack access
        0x7FFF012C, // Close proximity stack access
        0x004012A0, // Text block lookup
        0x7FFF0050  // Stack call
    };

    std::cout << std::hex << std::showbase;
    for (unsigned long long ptr : cpu_instruction_fetches) {
        VMRegion* matched_segment = vmm.resolve_pointer(ptr);
        if (matched_segment) {
            std::cout << "CPU Read OK: Pointer " << ptr << " hits inside -> [" 
                      << matched_segment->mapping_name << "]\n";
        } else {
            std::cout << "KERNEL PANIC: Segmentation Fault at " << ptr << "\n";
        }
    }
    return 0;
}