#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"
#include "pmm.h"

#define FRAME_SIZE 0x1000
#define ALIGN_UP(x,a)   (((x) + ((a)-1)) & ~((a)-1))
#define ALIGN_DOWN(x,a) ((x) & ~((a)-1))

extern uint8_t _kernel_end[]; // linker symbol

static uint32_t *pmm_bitmap = 0;
static uint32_t pmm_frames = 0; // number of frames tracked

static inline void bit_set(uint32_t frame)   { pmm_bitmap[frame>>5] |=  (1u << (frame & 31)); }
static inline void bit_clear(uint32_t frame) { pmm_bitmap[frame>>5] &= ~(1u << (frame & 31)); }
static inline uint32_t bit_test(uint32_t frame){ return pmm_bitmap[frame>>5] & (1u << (frame & 31)); }

static void mark_range_used(uint32_t base, uint32_t end) {
    base = ALIGN_DOWN(base, FRAME_SIZE);
    end  = ALIGN_UP(end, FRAME_SIZE);
    for (uint32_t a = base; a < end; a += FRAME_SIZE) bit_set(a / FRAME_SIZE);
}

static void mark_range_free(uint32_t base, uint32_t end) {
    base = ALIGN_UP(base, FRAME_SIZE);
    end  = ALIGN_DOWN(end, FRAME_SIZE);
    for (uint32_t a = base; a < end; a += FRAME_SIZE) bit_clear(a / FRAME_SIZE);
}

void pmm_init(uint32_t mb_info_ptr)
{
    multiboot_info_t *mbi = (multiboot_info_t*)mb_info_ptr;

    // Need mmap
    if (!(mbi->flags & (1u << 6))) {
        // No mmap => can't safely PMM. Hard halt.
        for (;;) __asm__ volatile("hlt");
    }

    // 1) Find top of usable RAM below 4GB
    uint32_t top = 0;
    multiboot_memory_map_t *m = (multiboot_memory_map_t*)mbi->mmap_addr;
    uint32_t end = mbi->mmap_addr + mbi->mmap_length;

    while ((uint32_t)m < end) {
        if (m->type == 1) {
            uint64_t base = m->base_addr;
            uint64_t len  = m->length;
            uint64_t e64 = base + len;

            if (base < 0x100000000ULL) {
                if (e64 > 0x100000000ULL) e64 = 0x100000000ULL; // clamp to 4GB
                if ((uint32_t)e64 > top) top = (uint32_t)e64;
            }
        }
        m = (multiboot_memory_map_t*)((uint32_t)m + m->size + 4);
    }

    // frames we track
    pmm_frames = top / FRAME_SIZE;

    // 2) Place bitmap right after kernel, aligned
    uint32_t bitmap_bytes = (pmm_frames + 7) / 8;
    uint32_t bitmap_base  = ALIGN_UP((uint32_t)_kernel_end, FRAME_SIZE);
    uint32_t bitmap_end   = ALIGN_UP(bitmap_base + bitmap_bytes, FRAME_SIZE);
    pmm_bitmap = (uint32_t*)bitmap_base;

    // 3) Mark all USED initially (1s)
    uint32_t bitmap_words = ALIGN_UP(bitmap_bytes, 4) / 4;
    for (uint32_t i = 0; i < bitmap_words; i++) pmm_bitmap[i] = 0xFFFFFFFFu;

    // 4) Free only type==1 regions
    m = (multiboot_memory_map_t*)mbi->mmap_addr;
    while ((uint32_t)m < end) {
        if (m->type == 1) {
            uint64_t base = m->base_addr;
            uint64_t len  = m->length;
            uint64_t e64  = base + len;

            if (base < 0x100000000ULL) {
                if (e64 > 0x100000000ULL) e64 = 0x100000000ULL;
                mark_range_free((uint32_t)base, (uint32_t)e64);
            }
        }
        m = (multiboot_memory_map_t*)((uint32_t)m + m->size + 4);
    }

    // 5) Re-reserve critical regions
    // Null page + low BIOSy area (optional but wise): keep first 1MB used
    mark_range_used(0x00000000, 0x00100000);

    // Kernel is loaded at 2MB in your linker script
    mark_range_used(0x00200000, (uint32_t)_kernel_end);

    // Bitmap itself must be used
    mark_range_used(bitmap_base, bitmap_end);

    // Multiboot info structure lives somewhere in RAM; reserve at least its first page.
    // (Later you can reserve more precisely; this is safe and small.)
    mark_range_used(mb_info_ptr, mb_info_ptr + FRAME_SIZE);

    // Modules, if any
    if (mbi->flags & (1u << 3)) { // mods_* valid
        // multiboot modules struct is 16 bytes each in MB1
        mark_range_used(mbi->mods_addr, mbi->mods_addr + mbi->mods_count * 16);
    }
}

uint32_t pmm_alloc_frame(void)
{
    if (!pmm_bitmap) return 0;

    uint32_t words = (pmm_frames + 31) / 32;
    for (uint32_t i = 0; i < words; i++) {
        uint32_t w = pmm_bitmap[i];
        if (w != 0xFFFFFFFFu) {
            for (uint32_t b = 0; b < 32; b++) {
                uint32_t frame = i * 32 + b;
                if (frame >= pmm_frames) return 0;
                if (!bit_test(frame)) {
                    bit_set(frame);
                    return frame * FRAME_SIZE;
                }
            }
        }
    }
    return 0;
}

void pmm_free_frame(uint32_t phys_addr)
{
    uint32_t frame = phys_addr / FRAME_SIZE;
    if (frame < pmm_frames) bit_clear(frame);
}