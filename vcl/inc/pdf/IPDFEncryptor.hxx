/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#pragma once

#include <rtl/ustring.hxx>
#include <vector>

namespace vcl
{
struct PDFEncryptionProperties;
}

namespace com::sun::star::beans
{
class XMaterialHolder;
}
namespace com::sun::star::uno
{
template <typename> class Reference;
}

namespace vcl::pdf
{
/** Interface for encrypting the PDF content
 *
 * This interface makes it possible to have multiple versions and
 * revisions of PDF encryption, but all using the same interface,
 * so the implementation details are hidden from the outside. This
 * also makes it easier to add new implementations in the future as
 * we only need to write a new implementation of this interface.
 */
class IPDFEncryptor
{
public:
    virtual ~IPDFEncryptor() {}

    /** PDF encryption version */
    virtual sal_Int32 getVersion() = 0;
    /** PDF encryption revision */
    virtual sal_Int32 getRevision() = 0;

    /** the numerical value of the access permissions, according to PDF spec, must be signed */
    virtual sal_Int32 getAccessPermissions() = 0;

    /** Encrypted access permission
     *
     * Depending on the encryption revision this may not be available. In that
     * case we can expect empty content.
     */
    virtual std::vector<sal_uInt8> getEncryptedAccessPermissions()
    {
        return std::vector<sal_uInt8>();
    }

    /** Length of the key in bits (i.e. 256 = 256bits) */
    virtual sal_Int32 getKeyLength() = 0;

    /** Prepares the encryption when the password is entered */
    virtual bool prepareEncryption(
        const css::uno::Reference<css::beans::XMaterialHolder>& xEncryptionMaterialHolder,
        PDFEncryptionProperties& rProperties)
        = 0;

    /** Set up the keys and does a sanity check */
    virtual void setupKeysAndCheck(PDFEncryptionProperties& rProperties) = 0;

    /** Setup before we start encrypting - remembers the key */
    virtual void setupEncryption(std::vector<sal_uInt8> const& rEncryptionKey, sal_Int32 nObject)
        = 0;

    virtual void enableStreamEncryption() = 0;
    virtual void disableStreamEncryption() = 0;
    virtual bool isStreamEncryptionEnabled() = 0;

    /** Encrypts the input and stores into the output */
    virtual void encrypt(const void* pInput, sal_uInt64 nInputSize, sal_uInt8* pOutput,
                         sal_uInt64 nOutputsSize)
        = 0;
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
