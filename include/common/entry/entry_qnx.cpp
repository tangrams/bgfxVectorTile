/*
 * Copyright 2011-2014 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "entry.h"

#if ENTRY_CONFIG_USE_NATIVE && BX_PLATFORM_QNX

#include <stdio.h>
#include "entry.h"

namespace entry
{
	const Event* poll()
	{
		return NULL;
	}

	void release(const Event* _event)
	{
	}

	void setWindowSize(uint32_t _width, uint32_t _height)
	{
	}

	void toggleWindowFrame()
	{
	}

	void setMouseLock(bool _lock)
	{
	}

} // namespace entry

int main(int _argc, char** _argv)
{
	entry::main(_argc, _argv);
}

#endif // ENTRY_CONFIG_USE_NATIVE && BX_PLATFORM_QNX
