#ifndef GRAPHICSRENDERER_HEADER
#define GRAPHICSRENDERER_HEADER

namespace EX
{
	class GraphicsContext
	{
	public:
		virtual void Init()=0;
		virtual void SwapBuffers()=0;
	};
}

#endif // !GRAPHICSRENDERER_HEADER
