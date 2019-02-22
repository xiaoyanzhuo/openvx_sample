# 

# Copyright (c) 2012-2017 The Khronos Group Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


include $(PRELUDE)
TARGET      := vxu
TARGETTYPE  := dsmo
IDIRS       += $(HOST_ROOT)/$(OPENVX_SRC)/include $(HOST_ROOT)/debug
DEFFILE     := vx_utility.def
CSOURCES    := vx_utility.c
SHARED_LIBS := openvx
STATIC_LIBS := openvx-helper
ifneq (,$(findstring EXPERIMENTAL_USE_OPENCL,$(SYSDEFS)))
USE_OPENCL := true
endif
include $(FINALE)

