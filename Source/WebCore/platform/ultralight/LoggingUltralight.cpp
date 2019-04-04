/*
* Copyright (C) 2007 Alp Toker <alp@atoker.com>
* Copyright (C) 2009 Gustavo Noronha Silva <gns@gnome.org>
* Copyright (C) 2013 Apple Inc. All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "Logging.h"

#if !LOG_DISABLED

#include <string.h>
#include <wtf/text/WTFString.h>
#include "NotImplemented.h"

namespace WebCore {

  String logLevelString()
  {
    // TODO
    notImplemented();
    return String();
  }

} // namespace WebCore

#endif // !LOG_DISABLED