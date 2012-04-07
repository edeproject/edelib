/*
 * $Id: String.h 2594 2009-03-25 14:54:54Z karijes $
 *
 * EDE specific code
 * Copyright (c) 2010-2012 edelib authors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <edelib/Ede.h>

EDELIB_NS_BEGIN

ApplicationBootstrap::ApplicationBootstrap(const char *name, const char *path) {
	nls_support_init(name, path);
}

ApplicationBootstrap::~ApplicationBootstrap() {
}

EDELIB_NS_END
