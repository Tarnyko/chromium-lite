//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libgpu_command_buffer.so

#include <iostream>		// for << >>
#include <GL/gl.h>		// for GL_COMPRESSED_TEXTURE_FORMATS

#include "gpu/command_buffer/common/gles2_cmd_utils.h"


 /* we use the helper util to :
    - set the GL_COMPRESSED_TEXTURE_FORMATS property easily ;
    - compute the real size of a 320x240 GL_RGB image */

int main (int argc, char *argv[])
{
	gpu::gles2::GLES2Util *util = new gpu::gles2::GLES2Util ();

	util->set_num_compressed_texture_formats (2);

	std::cout << "Texture formats (helper) : " << util->num_compressed_texture_formats () << std::endl;
	std::cout << "Texture formats : " << util->GLGetNumValuesReturned (GL_COMPRESSED_TEXTURE_FORMATS) << std::endl;

	// ---------------

	uint32_t size, row_size;

	gpu::gles2::GLES2Util::ComputeImageDataSizes (320, 240, 1, GL_RGB, GL_UNSIGNED_BYTE, 1, &size, &row_size, &row_size);

	std::cout << "320x240 RGB image size : " << size << std::endl;
	std::cout << "320x240 RGB row size : " << row_size << std::endl;

	return 0;
}

