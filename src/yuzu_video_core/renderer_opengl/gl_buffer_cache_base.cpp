// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "yuzu_video_core/buffer_cache/buffer_cache.h"
#include "yuzu_video_core/renderer_opengl/gl_buffer_cache.h"

namespace VideoCommon {
template class VideoCommon::BufferCache<OpenGL::BufferCacheParams>;
}
