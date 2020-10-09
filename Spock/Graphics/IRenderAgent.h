#include "IRenderable.h"

class IRenderAgent
{
	public:
		//virtual void ProducePipeline() = 0;
		//virtual void ProduceDescriptorSet() = 0;
		virtual void Render(IRenderable* irenderable) = 0;
		virtual void Display() = 0;
		virtual void Resize(int x, int y) = 0;
		virtual bool WindowShouldClose() = 0;
};
