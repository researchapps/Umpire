//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-21, Lawrence Livermore National Security, LLC and Umpire
// project contributors. See the COPYRIGHT file for details.
//
// SPDX-License-Identifier: (MIT)
//////////////////////////////////////////////////////////////////////////////
#include "camp/camp.hpp"
#include "umpire/ResourceManager.hpp"
#include "umpire/device_allocator_helper.hpp"

#if defined(UMPIRE_ENABLE_CUDA)
using resource_type = camp::resources::Cuda;
#elif defined(UMPIRE_ENABLE_HIP)
using resource_type = camp::resources::Hip;
#endif

/*
 * Very simple kernels that use only the first thread to "get" the
 * existing DeviceAllocator and to allocate a double. (One kernel retrieves
 * the DeviceAllocator object by ID, the other by name.)
 * Making sure that the data_ptr is pointing to the device allocated double,
 * it sets the value of that double which will be checked later.
 */
__global__ void my_kernel(double** data_ptr)
{
  if (threadIdx.x == 0) {
    // _sphinx_tag_get_dev_allocator_id_start
    umpire::DeviceAllocator alloc = umpire::get_device_allocator(0);
    // _sphinx_tag_get_dev_allocator_id_end
    double* data = static_cast<double*>(alloc.allocate(1 * sizeof(double)));
    *data_ptr = data;
    data[0] = 1024;
  }
}

__global__ void my_other_kernel(double** data_ptr)
{
  if (threadIdx.x == 0) {
    // _sphinx_tag_get_dev_allocator_name_start
    umpire::DeviceAllocator alloc = umpire::get_device_allocator("my_device_alloc");
    // _sphinx_tag_get_dev_allocator_name_end
    double* data = static_cast<double*>(alloc.allocate(1 * sizeof(double)));
    *data_ptr = data;
    data[0] = 42;
  }
}

int main(int argc, char const* argv[])
{
  auto resource = camp::resources::Resource{resource_type{}};

  // Create my allocators.
  // _sphinx_tag_make_dev_allocator_start
  auto& rm = umpire::ResourceManager::getInstance();
  auto allocator = rm.getAllocator("UM");
  auto device_allocator = umpire::make_device_allocator(allocator, sizeof(double), "my_device_alloc");
  // _sphinx_tag_make_dev_allocator_end

  // Checking that a DeviceAllocator exists...
  if (umpire::is_device_allocator(0)) {
    std::cout << "I found a DeviceAllocator! " << std::endl;
  }

  double** ptr_to_data = static_cast<double**>(allocator.allocate(sizeof(double*)));

  // Make sure that device and host side DeviceAllocator pointers are synched
  // _sphinx_tag_macro_start
  UMPIRE_SET_UP_DEVICE_ALLOCATORS();
  // _sphinx_tag_macro_end

  my_kernel<<<1, 16>>>(ptr_to_data);
  resource.get_event().wait();
  std::cout << "After first kernel, found value: " << (*ptr_to_data)[0] << std::endl;

  // DeviceAllocator only has enough memory for one double. We need to reset it!
  device_allocator.reset();

  my_other_kernel<<<1, 16>>>(ptr_to_data);
  resource.get_event().wait();
  std::cout << "After second kernel, found value: " << (*ptr_to_data)[0] << std::endl;

  return 0;
}
