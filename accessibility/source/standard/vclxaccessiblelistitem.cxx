/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <standard/vclxaccessiblelistitem.hxx>
#include <helper/IComboListBoxHelper.hxx>
#include <com/sun/star/awt/Rectangle.hpp>

#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboard.hpp>
#include <com/sun/star/datatransfer/clipboard/XFlushableClipboard.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <utility>
#include <vcl/svapp.hxx>
#include <vcl/toolkit/lstbox.hxx>
#include <vcl/unohelp.hxx>
#include <vcl/unohelp2.hxx>
#include <vcl/settings.hxx>
#include <unotools/accessiblerelationsethelper.hxx>
#include <comphelper/accessibleeventnotifier.hxx>
#include <i18nlangtag/languagetag.hxx>

namespace
{
    /// @throws css::lang::IndexOutOfBoundsException
    void checkIndex_Impl( sal_Int32 _nIndex, std::u16string_view _sText )
    {
        if ( _nIndex < 0 || _nIndex > static_cast<sal_Int32>(_sText.size()) )
            throw css::lang::IndexOutOfBoundsException();
    }
}

// class VCLXAccessibleListItem ------------------------------------------

using namespace ::com::sun::star::accessibility;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star;


// Ctor() and Dtor()

VCLXAccessibleListItem::VCLXAccessibleListItem(sal_Int32 _nIndexInParent, rtl::Reference< VCLXAccessibleList > _xParent)
    : m_nIndexInParent(_nIndexInParent)
    , m_bSelected(false)
    , m_bVisible(false)
    , m_nClientId(0)
    , m_xParent(std::move(_xParent))
{
    assert(m_xParent.is());
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent->getListBoxHelper();
    if (pListBoxHelper)
        m_sEntryText = pListBoxHelper->GetEntry(static_cast<sal_uInt16>(_nIndexInParent));
}

void VCLXAccessibleListItem::SetSelected( bool _bSelected )
{
    if ( m_bSelected != _bSelected )
    {
        Any aOldValue;
        Any aNewValue;
        if ( m_bSelected )
            aOldValue <<= AccessibleStateType::SELECTED;
        else
            aNewValue <<= AccessibleStateType::SELECTED;
        m_bSelected = _bSelected;
        NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
    }
}

void VCLXAccessibleListItem::SetVisible( bool _bVisible )
{
    if ( m_bVisible != _bVisible )
    {
        Any aOldValue, aNewValue;
        m_bVisible = _bVisible;
        (_bVisible ? aNewValue : aOldValue ) <<= AccessibleStateType::VISIBLE;
        NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
        (_bVisible ? aNewValue : aOldValue ) <<= AccessibleStateType::SHOWING;
        NotifyAccessibleEvent( AccessibleEventId::STATE_CHANGED, aOldValue, aNewValue );
    }
}

void VCLXAccessibleListItem::NotifyAccessibleEvent( sal_Int16 _nEventId,
                                                    const css::uno::Any& _aOldValue,
                                                    const css::uno::Any& _aNewValue )
{
    AccessibleEventObject aEvt;
    aEvt.Source = *this;
    aEvt.EventId = _nEventId;
    aEvt.OldValue = _aOldValue;
    aEvt.NewValue = _aNewValue;

    if (m_nClientId)
        comphelper::AccessibleEventNotifier::addEvent( m_nClientId, aEvt );
}

// OCommonAccessibleText

OUString VCLXAccessibleListItem::implGetText()
{
    return m_sEntryText;
}

Locale VCLXAccessibleListItem::implGetLocale()
{
    return Application::GetSettings().GetLanguageTag().getLocale();
}

void VCLXAccessibleListItem::implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex )
{
    nStartIndex = 0;
    nEndIndex = 0;
}

// XTypeProvider

Sequence< sal_Int8 > VCLXAccessibleListItem::getImplementationId()
{
    return css::uno::Sequence<sal_Int8>();
}

// XComponent

void VCLXAccessibleListItem::disposing(std::unique_lock<std::mutex>& rGuard)
{
    VCLXAccessibleListItem_BASE::disposing(rGuard);

    m_sEntryText.clear();
    m_xParent           = nullptr;
    comphelper::AccessibleEventNotifier::TClientId nId = m_nClientId;
    m_nClientId =  0;
    Reference< XInterface > xEventSource;
    if ( nId )
    {
        xEventSource = *this;

        // Send a disposing to all listeners.
        rGuard.unlock();
        comphelper::AccessibleEventNotifier::revokeClientNotifyDisposing( nId, *this );
        rGuard.lock();
    }
}

// XServiceInfo

OUString VCLXAccessibleListItem::getImplementationName()
{
    return u"com.sun.star.comp.toolkit.AccessibleListItem"_ustr;
}

sal_Bool VCLXAccessibleListItem::supportsService( const OUString& rServiceName )
{
    return cppu::supportsService(this, rServiceName);
}

Sequence< OUString > VCLXAccessibleListItem::getSupportedServiceNames()
{
    return {u"com.sun.star.accessibility.AccessibleContext"_ustr,
            u"com.sun.star.accessibility.AccessibleComponent"_ustr,
            u"com.sun.star.accessibility.AccessibleListItem"_ustr};
}

// XAccessible

Reference< XAccessibleContext > SAL_CALL VCLXAccessibleListItem::getAccessibleContext(  )
{
    return this;
}

// XAccessibleContext

sal_Int64 SAL_CALL VCLXAccessibleListItem::getAccessibleChildCount(  )
{
    return 0;
}

Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleChild( sal_Int64 )
{
    return Reference< XAccessible >();
}

Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleParent(  )
{
    std::unique_lock aGuard( m_aMutex );

    return m_xParent;
}

sal_Int64 SAL_CALL VCLXAccessibleListItem::getAccessibleIndexInParent(  )
{
    return m_nIndexInParent;
}

sal_Int16 SAL_CALL VCLXAccessibleListItem::getAccessibleRole(  )
{
    return AccessibleRole::LIST_ITEM;
    //  return AccessibleRole::LABEL;
}

OUString SAL_CALL VCLXAccessibleListItem::getAccessibleDescription(  )
{
    // no description for every item
    return OUString();
}

OUString SAL_CALL VCLXAccessibleListItem::getAccessibleName(  )
{
    std::unique_lock aGuard( m_aMutex );

    // entry text == accessible name
    return m_sEntryText;
}

Reference< XAccessibleRelationSet > SAL_CALL VCLXAccessibleListItem::getAccessibleRelationSet(  )
{
    return new utl::AccessibleRelationSetHelper;
}

sal_Int64 SAL_CALL VCLXAccessibleListItem::getAccessibleStateSet(  )
{
    std::unique_lock aGuard( m_aMutex );

    sal_Int64 nStateSet = 0;

    if ( !m_bDisposed )
    {
        nStateSet |= AccessibleStateType::TRANSIENT;

        ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
        if (pListBoxHelper && pListBoxHelper->IsEnabled())
        {
            nStateSet |= AccessibleStateType::SELECTABLE;
            nStateSet |= AccessibleStateType::ENABLED;
            nStateSet |= AccessibleStateType::SENSITIVE;
        }

        if ( m_bSelected )
            nStateSet |= AccessibleStateType::SELECTED;
        if ( m_bVisible )
        {
            nStateSet |= AccessibleStateType::VISIBLE;
            nStateSet |= AccessibleStateType::SHOWING;
        }
    }
    else
        nStateSet |= AccessibleStateType::DEFUNC;

    return nStateSet;
}

Locale SAL_CALL VCLXAccessibleListItem::getLocale(  )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return implGetLocale();
}

// XAccessibleComponent

sal_Bool SAL_CALL VCLXAccessibleListItem::containsPoint( const awt::Point& _aPoint )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    bool bInside = false;
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        tools::Rectangle aRect(pListBoxHelper->GetBoundingRectangle(static_cast<sal_uInt16>(m_nIndexInParent)));
        aRect.Move(-aRect.Left(), -aRect.Top());
        bInside = aRect.Contains(vcl::unohelper::ConvertToVCLPoint(_aPoint));
    }
    return bInside;
}

Reference< XAccessible > SAL_CALL VCLXAccessibleListItem::getAccessibleAtPoint( const awt::Point& )
{
    return Reference< XAccessible >();
}

awt::Rectangle SAL_CALL VCLXAccessibleListItem::getBounds(  )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    awt::Rectangle aRect;
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
        aRect = vcl::unohelper::ConvertToAWTRect(
            pListBoxHelper->GetBoundingRectangle(static_cast<sal_uInt16>(m_nIndexInParent)));

    return aRect;
}

awt::Point SAL_CALL VCLXAccessibleListItem::getLocation(  )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    Point aPoint(0,0);
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        tools::Rectangle aRect = pListBoxHelper->GetBoundingRectangle( static_cast<sal_uInt16>(m_nIndexInParent) );
        aPoint = aRect.TopLeft();
    }
    return vcl::unohelper::ConvertToAWTPoint(aPoint);
}

awt::Point SAL_CALL VCLXAccessibleListItem::getLocationOnScreen(  )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    Point aPoint(0,0);
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        tools::Rectangle aRect = pListBoxHelper->GetBoundingRectangle(static_cast<sal_uInt16>(m_nIndexInParent));
        aPoint = aRect.TopLeft();
        aPoint += Point(pListBoxHelper->GetWindowExtentsAbsolute().TopLeft());
    }
    return vcl::unohelper::ConvertToAWTPoint(aPoint);
}

awt::Size SAL_CALL VCLXAccessibleListItem::getSize(  )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    Size aSize;
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
        aSize = pListBoxHelper->GetBoundingRectangle( static_cast<sal_uInt16>(m_nIndexInParent) ).GetSize();

    return vcl::unohelper::ConvertToAWTSize(aSize);
}

void SAL_CALL VCLXAccessibleListItem::grabFocus(  )
{
    // no focus for each item
}

// XAccessibleText

sal_Int32 SAL_CALL VCLXAccessibleListItem::getCaretPosition()
{
    return -1;
}

sal_Bool SAL_CALL VCLXAccessibleListItem::setCaretPosition( sal_Int32 nIndex )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    if ( !implIsValidRange( nIndex, nIndex, m_sEntryText.getLength() ) )
        throw IndexOutOfBoundsException();

    return false;
}

sal_Unicode SAL_CALL VCLXAccessibleListItem::getCharacter( sal_Int32 nIndex )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return OCommonAccessibleText::implGetCharacter( m_sEntryText, nIndex );
}

Sequence< PropertyValue > SAL_CALL VCLXAccessibleListItem::getCharacterAttributes( sal_Int32 nIndex, const Sequence< OUString >& )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    if ( !implIsValidIndex( nIndex, m_sEntryText.getLength() ) )
        throw IndexOutOfBoundsException();

    return Sequence< PropertyValue >();
}

awt::Rectangle SAL_CALL VCLXAccessibleListItem::getCharacterBounds( sal_Int32 nIndex )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    if ( !implIsValidIndex( nIndex, m_sEntryText.getLength() ) )
        throw IndexOutOfBoundsException();

    awt::Rectangle aBounds( 0, 0, 0, 0 );
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        tools::Rectangle aCharRect = pListBoxHelper->GetEntryCharacterBounds( m_nIndexInParent, nIndex );
        tools::Rectangle aItemRect = pListBoxHelper->GetBoundingRectangle( static_cast<sal_uInt16>(m_nIndexInParent) );
        aCharRect.Move( -aItemRect.Left(), -aItemRect.Top() );
        aBounds = vcl::unohelper::ConvertToAWTRect(aCharRect);
    }

    return aBounds;
}

sal_Int32 SAL_CALL VCLXAccessibleListItem::getCharacterCount()
{
    std::unique_lock aGuard( m_aMutex );

    return m_sEntryText.getLength();
}

sal_Int32 SAL_CALL VCLXAccessibleListItem::getIndexAtPoint( const awt::Point& aPoint )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    sal_Int32 nIndex = -1;
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        sal_Int32 nPos = LISTBOX_ENTRY_NOTFOUND;
        tools::Rectangle aItemRect = pListBoxHelper->GetBoundingRectangle( static_cast<sal_uInt16>(m_nIndexInParent) );
        Point aPnt(vcl::unohelper::ConvertToVCLPoint(aPoint));
        aPnt += aItemRect.TopLeft();
        sal_Int32 nI = pListBoxHelper->GetIndexForPoint( aPnt, nPos );
        if ( nI != -1 && m_nIndexInParent == nPos )
            nIndex = nI;
    }
    return nIndex;
}

OUString SAL_CALL VCLXAccessibleListItem::getSelectedText()
{
    return OUString();
}

sal_Int32 SAL_CALL VCLXAccessibleListItem::getSelectionStart()
{
    return 0;
}

sal_Int32 SAL_CALL VCLXAccessibleListItem::getSelectionEnd()
{
    return 0;
}

sal_Bool SAL_CALL VCLXAccessibleListItem::setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex )
{
    std::unique_lock aGuard( m_aMutex );

    if ( !implIsValidRange( nStartIndex, nEndIndex, m_sEntryText.getLength() ) )
        throw IndexOutOfBoundsException();

    return false;
}

OUString SAL_CALL VCLXAccessibleListItem::getText()
{
    std::unique_lock aGuard( m_aMutex );

    return m_sEntryText;
}

OUString VCLXAccessibleListItem::getTextRangeImpl(std::unique_lock<std::mutex>& /*rGuard*/, sal_Int32 nStartIndex, sal_Int32 nEndIndex)
{
    return OCommonAccessibleText::implGetTextRange(m_sEntryText, nStartIndex, nEndIndex);
}

OUString SAL_CALL VCLXAccessibleListItem::getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return getTextRangeImpl(aGuard, nStartIndex, nEndIndex);
}

css::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return OCommonAccessibleText::getTextAtIndex( nIndex, aTextType );
}

css::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return OCommonAccessibleText::getTextBeforeIndex( nIndex, aTextType );
}

css::accessibility::TextSegment SAL_CALL VCLXAccessibleListItem::getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    return OCommonAccessibleText::getTextBehindIndex( nIndex, aTextType );
}

sal_Bool SAL_CALL VCLXAccessibleListItem::copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex )
{
    SolarMutexGuard aSolarGuard;
    std::unique_lock aGuard( m_aMutex );

    checkIndex_Impl( nStartIndex, m_sEntryText );
    checkIndex_Impl( nEndIndex, m_sEntryText );

    bool bRet = false;
    ::accessibility::IComboListBoxHelper* pListBoxHelper = m_xParent.is() ? m_xParent->getListBoxHelper() : nullptr;
    if (pListBoxHelper)
    {
        Reference< datatransfer::clipboard::XClipboard > xClipboard = pListBoxHelper->GetClipboard();
        if ( xClipboard.is() )
        {
            OUString sText(getTextRangeImpl(aGuard, nStartIndex, nEndIndex));
            rtl::Reference<vcl::unohelper::TextDataObject> pDataObj = new vcl::unohelper::TextDataObject( sText );

            SolarMutexReleaser aReleaser;
            xClipboard->setContents( pDataObj, nullptr );
            Reference< datatransfer::clipboard::XFlushableClipboard > xFlushableClipboard( xClipboard, uno::UNO_QUERY );
            if( xFlushableClipboard.is() )
                xFlushableClipboard->flushClipboard();

            bRet = true;
        }
    }

    return bRet;
}

sal_Bool VCLXAccessibleListItem::scrollSubstringTo( sal_Int32, sal_Int32, AccessibleScrollType )
{
    return false;
}

// XAccessibleEventBroadcaster

void SAL_CALL VCLXAccessibleListItem::addAccessibleEventListener( const Reference< XAccessibleEventListener >& xListener )
{
    if (xListener.is())
    {
        if (!m_nClientId)
            m_nClientId = comphelper::AccessibleEventNotifier::registerClient( );
        comphelper::AccessibleEventNotifier::addEventListener( m_nClientId, xListener );
    }
}

void SAL_CALL VCLXAccessibleListItem::removeAccessibleEventListener( const Reference< XAccessibleEventListener >& xListener )
{
    if ( !(xListener.is() && m_nClientId) )
        return;

    sal_Int32 nListenerCount = comphelper::AccessibleEventNotifier::removeEventListener( m_nClientId, xListener );
    if ( nListenerCount )
        return;

    // no listeners anymore
    // -> revoke ourself. This may lead to the notifier thread dying (if we were the last client),
    // and at least to us not firing any events anymore, in case somebody calls
    // NotifyAccessibleEvent, again
    if ( m_nClientId )
    {
        comphelper::AccessibleEventNotifier::TClientId nId( m_nClientId );
        m_nClientId = 0;
        comphelper::AccessibleEventNotifier::revokeClient( nId );
    }
}


// AF (Oct. 29 2002): Return black as constant foreground color.  This is an
// initial implementation and has to be substituted by code that determines
// the color that is actually used.
sal_Int32 SAL_CALL VCLXAccessibleListItem::getForeground()
{
    return sal_Int32(COL_BLACK);
}

// AF (Oct. 29 2002): Return white as constant background color.  This is an
// initial implementation and has to be substituted by code that determines
// the color that is actually used.
sal_Int32 SAL_CALL VCLXAccessibleListItem::getBackground()
{
    return sal_Int32(COL_WHITE);
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
