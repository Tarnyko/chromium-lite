//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libui_gl.so /usr/lib/chromium/lib/libgpu.so

#include <iostream>			// for << >>

#include "gpu/command_buffer/service/shader_manager.h"
#include "gpu/command_buffer/service/shader_translator.h"
#include "ui/gl/gl_version_info.h"


 // we create a simple shader and try to compile it

int main (int argc, char *argv[])
{
	ShBuiltInResources resources;
	ShShaderOutput language;
        scoped_refptr<gpu::gles2::ShaderTranslator> translator;

	 // initialize resources, language (OpenGL 2.0) and translator

	ShInitBuiltInResources (&resources);
	resources.MaxExpressionComplexity = 32;
	resources.MaxCallStackDepth = 32;

	language = gpu::gles2::ShaderTranslator::GetShaderOutputLanguageForContext (gfx::GLVersionInfo("2.0", "", ""));

	translator = new gpu::gles2::ShaderTranslator ();
	translator->Init (GL_VERTEX_SHADER, SH_GLES2_SPEC, &resources, language, SH_EMULATE_BUILT_IN_FUNCTIONS);

	 // --------------------------
	 // create and compile shader

	gpu::gles2::ShaderManager manager;

	auto *shader = manager.CreateShader (1, 1, GL_VERTEX_SHADER);

	shader->set_source ("hello world !");
	shader->RequestCompile (translator, gpu::gles2::Shader::kANGLE);
	shader->DoCompile ();

	manager.Delete (shader);

	return 0;
}
