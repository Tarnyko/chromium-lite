//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libui_gl.so

#include <iostream>			// for << >>

#include "ui/gl/gl_context_stub.h"	// for gl::GLContextStub
#include "ui/gl/gpu_timing.h"		// for gl::GPUTimingClient, gl::GPUTimer


 /* we initialize OpenGL 3.2 with the timer query extension,
    and then estimate the time elapsed between GL instructions */

int main (int argc, char *argv[])
{
	auto *context = new gl::GLContextStub;

	context->SetGLVersionString ("3.2");
	context->SetExtensionsString ("GL_ARB_timer_query");

	scoped_refptr<gl::GPUTimingClient> client = context->CreateGPUTimingClient ();
	std::cout << "Current CPU time : " << client->GetCurrentCPUTime () << std::endl;


	std::unique_ptr<gl::GPUTimer> gpu_timer = client->CreateGPUTimer (false);

	gpu_timer->Reset ();

	// TODO : GL instructions here !

	while (!gpu_timer->IsAvailable ()) {
		std::cout << "Current CPU time : " << client->GetCurrentCPUTime () << std::endl;
		sleep (2);
	}

	gpu_timer->End();

	std::cout << "Time elapsed : " << gpu_timer->GetDeltaElapsed () << std::endl;

	return 0;
}
