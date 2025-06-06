/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2025, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file aiFileIO -> IOSystem wrapper*/

#ifndef AI_CIOSYSTEM_H_INCLUDED
#define AI_CIOSYSTEM_H_INCLUDED

#include <assimp/cfileio.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/ai_assert.h>

namespace Assimp {

class CIOSystemWrapper;

// ------------------------------------------------------------------------------------------------
/// @brief Custom IOStream implementation for the C-API-
// ------------------------------------------------------------------------------------------------
class CIOStreamWrapper final : public IOStream {
public:
    explicit CIOStreamWrapper(aiFile *pFile, CIOSystemWrapper *io);
    ~CIOStreamWrapper() override;
    size_t Read(void *pvBuffer, size_t pSize, size_t pCount) override;
    size_t Write(const void *pvBuffer, size_t pSize, size_t pCount) override;
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;
    size_t Tell(void) const override;
    size_t FileSize() const override;
    void Flush() override;

private:
    aiFile *mFile;
    CIOSystemWrapper *mIO;
};

inline CIOStreamWrapper::CIOStreamWrapper(aiFile *pFile, CIOSystemWrapper *io) :
        mFile(pFile),
        mIO(io) {
    ai_assert(io != nullptr);
}

// ------------------------------------------------------------------------------------------------
/// @brief Custom IO-System wrapper implementation for the C-API.
// ------------------------------------------------------------------------------------------------
class CIOSystemWrapper final : public IOSystem {
    friend class CIOStreamWrapper;

public:
    explicit CIOSystemWrapper(aiFileIO *pFile);
    ~CIOSystemWrapper() override = default;
    bool Exists(const char *pFile) const override;
    char getOsSeparator() const override;
    IOStream *Open(const char *pFile, const char *pMode = "rb") override;
    void Close(IOStream *pFile) override;

private:
    aiFileIO *mFileSystem;
};

inline CIOSystemWrapper::CIOSystemWrapper(aiFileIO *pFile) : mFileSystem(pFile) {
    ai_assert(pFile != nullptr);
}

} // namespace Assimp

#endif // AI_CIOSYSTEM_H_INCLUDED
