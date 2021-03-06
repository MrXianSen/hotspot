/*
 * Copyright (c) 1997, 2009, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

// ReservedSpace is a data structure for reserving a contiguous address range.

class ReservedSpace VALUE_OBJ_CLASS_SPEC {
  friend class VMStructs;
 private:
  char*  _base;
  size_t _size;
  size_t _noaccess_prefix;
  size_t _alignment;
  bool   _special;
  bool   _executable;

  // ReservedSpace
  ReservedSpace(char* base, size_t size, size_t alignment, bool special,
                bool executable);
  void initialize(size_t size, size_t alignment, bool large,
                  char* requested_address,
                  const size_t noaccess_prefix,
                  bool executable);

  // Release parts of an already-reserved memory region [addr, addr + len) to
  // get a new region that has "compound alignment."  Return the start of the
  // resulting region, or NULL on failure.
  //
  // The region is logically divided into a prefix and a suffix.  The prefix
  // starts at the result address, which is aligned to prefix_align.  The suffix
  // starts at result address + prefix_size, which is aligned to suffix_align.
  // The total size of the result region is size prefix_size + suffix_size.
  char* align_reserved_region(char* addr, const size_t len,
                              const size_t prefix_size,
                              const size_t prefix_align,
                              const size_t suffix_size,
                              const size_t suffix_align);

  // Reserve memory, call align_reserved_region() to alignment it and return the
  // result.
  char* reserve_and_align(const size_t reserve_size,
                          const size_t prefix_size,
                          const size_t prefix_align,
                          const size_t suffix_size,
                          const size_t suffix_align);

 protected:
  // Create protection page at the beginning of the space.
  void protect_noaccess_prefix(const size_t size);

 public:
  // Constructor
  ReservedSpace(size_t size);
  ReservedSpace(size_t size, size_t alignment, bool large,
                char* requested_address = NULL,
                const size_t noaccess_prefix = 0);
  ReservedSpace(const size_t prefix_size, const size_t prefix_align,
                const size_t suffix_size, const size_t suffix_align,
                char* requested_address,
                const size_t noaccess_prefix = 0);
  ReservedSpace(size_t size, size_t alignment, bool large, bool executable);
	// a new ReservedSpace constructor for limiting address space less than 4G.
	ReservedSpace(size_t size, size_t alignment, bool large, char* requested_address, bool executable);

  // Accessors
  char*  base()            const { return _base;      }
  size_t size()            const { return _size;      }
  size_t alignment()       const { return _alignment; }
  bool   special()         const { return _special;   }
  bool   executable()      const { return _executable;   }
  size_t noaccess_prefix() const { return _noaccess_prefix;   }
  bool is_reserved()       const { return _base != NULL; }
  void release();

  // Splitting
  ReservedSpace first_part(size_t partition_size, size_t alignment,
                           bool split = false, bool realloc = true);
  ReservedSpace last_part (size_t partition_size, size_t alignment);

  // These simply call the above using the default alignment.
  inline ReservedSpace first_part(size_t partition_size,
                                  bool split = false, bool realloc = true);
  inline ReservedSpace last_part (size_t partition_size);

  // Alignment
  static size_t page_align_size_up(size_t size);
  static size_t page_align_size_down(size_t size);
  static size_t allocation_align_size_up(size_t size);
  static size_t allocation_align_size_down(size_t size);
};

ReservedSpace
ReservedSpace::first_part(size_t partition_size, bool split, bool realloc)
{
  return first_part(partition_size, alignment(), split, realloc);
}

ReservedSpace ReservedSpace::last_part(size_t partition_size)
{
  return last_part(partition_size, alignment());
}

// Class encapsulating behavior specific of memory space reserved for Java heap
class ReservedHeapSpace : public ReservedSpace {
public:
  // Constructor
  ReservedHeapSpace(size_t size, size_t forced_base_alignment,
                    bool large, char* requested_address);
  ReservedHeapSpace(const size_t prefix_size, const size_t prefix_align,
                    const size_t suffix_size, const size_t suffix_align,
                    char* requested_address);
};

// Class encapsulating behavior specific memory space for Code
class ReservedCodeSpace : public ReservedSpace {
 public:
  // Constructor
  ReservedCodeSpace(size_t r_size, size_t rs_align, bool large);
	// for limiting address space less than 4G.
	ReservedCodeSpace(size_t r_size, size_t rs_align, bool large, char* requested_address);
};

// VirtualSpace is data structure for committing a previously reserved address range in smaller chunks.

class VirtualSpace VALUE_OBJ_CLASS_SPEC {
  friend class VMStructs;
 private:
  // Reserved area
  char* _low_boundary;
  char* _high_boundary;

  // Committed area
  char* _low;
  char* _high;

  // The entire space has been committed and pinned in memory, no
  // os::commit_memory() or os::uncommit_memory().
  bool _special;

  // Need to know if commit should be executable.
  bool   _executable;

  // MPSS Support
  // Each virtualspace region has a lower, middle, and upper region.
  // Each region has an end boundary and a high pointer which is the
  // high water mark for the last allocated byte.
  // The lower and upper unaligned to LargePageSizeInBytes uses default page.
  // size.  The middle region uses large page size.
  char* _lower_high;
  char* _middle_high;
  char* _upper_high;

  char* _lower_high_boundary;
  char* _middle_high_boundary;
  char* _upper_high_boundary;

  size_t _lower_alignment;
  size_t _middle_alignment;
  size_t _upper_alignment;

  // MPSS Accessors
  char* lower_high() const { return _lower_high; }
  char* middle_high() const { return _middle_high; }
  char* upper_high() const { return _upper_high; }

  char* lower_high_boundary() const { return _lower_high_boundary; }
  char* middle_high_boundary() const { return _middle_high_boundary; }
  char* upper_high_boundary() const { return _upper_high_boundary; }

  size_t lower_alignment() const { return _lower_alignment; }
  size_t middle_alignment() const { return _middle_alignment; }
  size_t upper_alignment() const { return _upper_alignment; }

 public:
  // Committed area
  char* low()  const { return _low; }
  char* high() const { return _high; }

  // Reserved area
  char* low_boundary()  const { return _low_boundary; }
  char* high_boundary() const { return _high_boundary; }

  bool special() const { return _special; }

 public:
  // Initialization
  VirtualSpace();
  bool initialize(ReservedSpace rs, size_t committed_byte_size);

  // Destruction
  ~VirtualSpace();

  // Testers (all sizes are byte sizes)
  size_t committed_size()   const;
  size_t reserved_size()    const;
  size_t uncommitted_size() const;
  bool   contains(const void* p)  const;

  // Operations
  // returns true on success, false otherwise
  bool expand_by(size_t bytes, bool pre_touch = false);
  void shrink_by(size_t bytes);
  void release();

  void check_for_contiguity() PRODUCT_RETURN;

  // Debugging
  void print() PRODUCT_RETURN;
};
