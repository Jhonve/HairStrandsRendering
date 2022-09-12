#pragma once

#include "render/render_base.h"
#include "elems/element.h"

class Strands : public Element
{
public:
	Strands() = default;

	virtual ~Strands();

	bool load(const std::string& filepath);
};
