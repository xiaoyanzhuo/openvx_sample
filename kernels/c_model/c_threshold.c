/* 

 * Copyright (c) 2012-2017 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <c_model.h>
#include <vx_debug.h>

// nodeless version of the Threshold kernel
vx_status vxThreshold(vx_image src_image, vx_threshold threshold, vx_image dst_image)
{
    vx_enum type = 0;
    vx_rectangle_t rect;
    vx_imagepatch_addressing_t src_addr;
    vx_imagepatch_addressing_t dst_addr;
    void *src_base = NULL;
    void *dst_base = NULL;
    vx_uint32 y = 0;
    vx_uint32 x = 0;
    vx_pixel_value_t value;
    vx_pixel_value_t lower;
    vx_pixel_value_t upper;
    vx_pixel_value_t true_value;
    vx_pixel_value_t false_value;
    vx_status status = VX_FAILURE;
    vx_enum format = 0;

    vxQueryThreshold(threshold, VX_THRESHOLD_TYPE, &type, sizeof(type));

    if (type == VX_THRESHOLD_TYPE_BINARY)
    {
        vxQueryThreshold(threshold, VX_THRESHOLD_THRESHOLD_VALUE, &value, sizeof(value));
        VX_PRINT(VX_ZONE_INFO, "threshold type binary, value = %u\n", value);
    }
    else if (type == VX_THRESHOLD_TYPE_RANGE)
    {
        vxQueryThreshold(threshold, VX_THRESHOLD_THRESHOLD_LOWER, &lower, sizeof(lower));
        vxQueryThreshold(threshold, VX_THRESHOLD_THRESHOLD_UPPER, &upper, sizeof(upper));
        VX_PRINT(VX_ZONE_INFO, "threshold type range, lower = %u, upper = %u\n", lower, upper);
    }

    vxQueryThreshold(threshold, VX_THRESHOLD_TRUE_VALUE, &true_value, sizeof(true_value));
    vxQueryThreshold(threshold, VX_THRESHOLD_FALSE_VALUE, &false_value, sizeof(false_value));
    VX_PRINT(VX_ZONE_INFO, "threshold true value = %u, threshold false value = %u\n", true_value, false_value);

    status = vxGetValidRegionImage(src_image, &rect);
    status |= vxAccessImagePatch(src_image, &rect, 0, &src_addr, &src_base, VX_READ_ONLY);
    status |= vxAccessImagePatch(dst_image, &rect, 0, &dst_addr, &dst_base, VX_WRITE_ONLY);

    vxQueryThreshold(threshold, VX_THRESHOLD_INPUT_FORMAT, &format, sizeof(format));
    VX_PRINT(VX_ZONE_INFO, "threshold input_format = %d\n", format);

    for (y = 0; y < src_addr.dim_y; y++)
    {
        for (x = 0; x < src_addr.dim_x; x++)
        {
            void *src_void_ptr = vxFormatImagePatchAddress2d(src_base, x, y, &src_addr);
            void *dst_void_ptr = vxFormatImagePatchAddress2d(dst_base, x, y, &dst_addr);

            if( format == VX_DF_IMAGE_S16 )
            {//case of input: VX_DF_IMAGE_S16 -> output: VX_DF_IMAGE_U8

                vx_int16 *src_ptr = (vx_int16*)src_void_ptr;
                vx_uint8 *dst_ptr = (vx_uint8*)dst_void_ptr;

                vx_uint8 dst_value = 0;

                if (type == VX_THRESHOLD_TYPE_BINARY)
                {                    
                    if (*src_ptr > value.S16)
                    {
                        dst_value = true_value.U8;
                    }
                    else
                    {
                         dst_value = false_value.U8;
                    }

                    *dst_ptr = dst_value;

                }

                if (type == VX_THRESHOLD_TYPE_RANGE)
                {
                    if (*src_ptr > upper.S16)
                    {
                        dst_value = false_value.U8;
                    }
                    else if (*src_ptr < lower.S16)
                    {
                        dst_value = false_value.U8;
                    }
                    else
                    {
                        dst_value = true_value.U8;
                    }

                    *dst_ptr = dst_value;
                }
            }
            else
            {//case of input: VX_DF_IMAGE_U8  -> output: VX_DF_IMAGE_U8

                vx_uint8 *src_ptr = (vx_uint8*)src_void_ptr;
                vx_uint8 *dst_ptr = (vx_uint8*)dst_void_ptr;

                if (type == VX_THRESHOLD_TYPE_BINARY)
                {
                    if (*src_ptr > value.U8)
                    {
                        *dst_ptr = (vx_uint8)true_value.U8;
                    }
                    else
                    {
                        *dst_ptr = (vx_uint8)false_value.U8;
                    }
                }

                if (type == VX_THRESHOLD_TYPE_RANGE)
                {
                    if (*src_ptr > upper.U8)
                    {
                        *dst_ptr = (vx_uint8)false_value.U8;
                    }
                    else if (*src_ptr < lower.U8)
                    {
                        *dst_ptr = (vx_uint8)false_value.U8;
                    }
                    else
                    {
                        *dst_ptr = (vx_uint8)true_value.U8;
                    }
                }
            } //end if else
        }//end for
    }//end for

    status |= vxCommitImagePatch(src_image, NULL, 0, &src_addr, src_base);
    status |= vxCommitImagePatch(dst_image, &rect, 0, &dst_addr, dst_base);

    return status;
}
