#pragma once
class IRenderable
{
	public:
		virtual void Update(float dt) = 0;
		virtual void Init() = 0;
};

