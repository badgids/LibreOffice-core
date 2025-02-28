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

#include <config_feature_desktop.h>

#include <cppuhelper/supportsservice.hxx>
#include <toolkit/awt/vclxwindows.hxx>
#include <toolkit/helper/accessiblefactory.hxx>
#include <vcl/accessiblefactory.hxx>
#include <standard/svtaccessiblenumericfield.hxx>
#include <standard/vclxaccessiblebutton.hxx>
#include <standard/vclxaccessiblecheckbox.hxx>
#include <standard/vclxaccessibledropdowncombobox.hxx>
#include <standard/vclxaccessiblecombobox.hxx>
#include <standard/vclxaccessibledropdownlistbox.hxx>
#include <standard/vclxaccessibleedit.hxx>
#include <standard/vclxaccessiblefixedhyperlink.hxx>
#include <standard/vclxaccessiblefixedtext.hxx>
#include <standard/vclxaccessibleheaderbar.hxx>
#include <standard/vclxaccessiblelistbox.hxx>
#include <standard/vclxaccessibleradiobutton.hxx>
#include <standard/vclxaccessiblescrollbar.hxx>
#include <standard/vclxaccessibletoolbox.hxx>
#include <standard/vclxaccessiblestatusbar.hxx>
#include <standard/vclxaccessibletabcontrol.hxx>
#include <standard/vclxaccessibletabpagewindow.hxx>
#include <extended/accessibletablistbox.hxx>
#include <extended/AccessibleBrowseBox.hxx>
#include <extended/accessibleiconchoicectrl.hxx>
#include <extended/AccessibleIconView.hxx>
#include <extended/accessibletabbar.hxx>
#include <extended/accessiblelistbox.hxx>
#include <extended/AccessibleBrowseBoxHeaderBar.hxx>
#include <extended/textwindowaccessibility.hxx>
#include <extended/AccessibleBrowseBoxTableCell.hxx>
#include <extended/AccessibleBrowseBoxHeaderCell.hxx>
#include <extended/AccessibleBrowseBoxCheckBoxCell.hxx>
#include <extended/accessibleeditbrowseboxcell.hxx>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <vcl/toolkit/lstbox.hxx>
#include <vcl/toolkit/combobox.hxx>
#include <vcl/toolkit/fixed.hxx>
#include <vcl/toolkit/fixedhyper.hxx>
#include <extended/AccessibleGridControl.hxx>
#include <vcl/accessibility/vclxaccessiblecomponent.hxx>
#include <vcl/accessibletable.hxx>

#include <floatingwindowaccessible.hxx>

using namespace ::accessibility;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::accessibility;
using namespace ::vcl;
using namespace ::vcl::table;

namespace {

bool hasFloatingChild(vcl::Window *pWindow)
{
    vcl::Window * pChild = pWindow->GetAccessibleChildWindow(0);
    return pChild && pChild->GetType() == WindowType::FLOATINGWINDOW;
}

// IAccessibleFactory
class AccessibleFactory :public ::toolkit::IAccessibleFactory
                        ,public ::vcl::IAccessibleFactory
{
public:
    AccessibleFactory();

    // ::toolkit::IAccessibleFactory
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(PushButton* pButton) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(CheckBox* pCheckBox) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(RadioButton* pRadioButton) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(ListBox* pListBox) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(FixedText* pFixedText) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(FixedHyperlink* pFixedHyperlink) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(ScrollBar* pScrollBar) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(Edit* pEdit) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(ComboBox* pComboBox) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(ToolBox* pToolBox) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(HeaderBar* pHeaderBar) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(FormattedField* pFormattedField) override;
    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleContext(vcl::Window* pWindow) override;

    // ::vcl::IAccessibleFactory
    virtual vcl::IAccessibleTabListBox*
        createAccessibleTabListBox(
            const css::uno::Reference< css::accessibility::XAccessible >& rxParent,
            SvHeaderTabListBox& rBox
        ) const override;

    virtual rtl::Reference<vcl::IAccessibleBrowseBox>
        createAccessibleBrowseBox(
            const css::uno::Reference< css::accessibility::XAccessible >& _rxParent,
            vcl::IAccessibleTableProvider& _rBrowseBox
        ) const override;

    virtual rtl::Reference<IAccessibleTableControl>
        createAccessibleTableControl(
            const css::uno::Reference< css::accessibility::XAccessible >& _rxParent,
            IAccessibleTable& _rTable
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleIconChoiceCtrl(
            SvtIconChoiceCtrl& _rIconCtrl,
            const css::uno::Reference< css::accessibility::XAccessible >& _xParent
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleTabBar(
            TabBar& _rTabBar
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessibleContext >
        createAccessibleTextWindowContext(
            vcl::Window* pWindow, TextEngine& rEngine, TextView& rView
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleTreeListBox(
            SvTreeListBox& _rListBox,
            const css::uno::Reference< css::accessibility::XAccessible >& _xParent
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleIconView(
            SvTreeListBox& _rListBox,
            const css::uno::Reference< css::accessibility::XAccessible >& _xParent
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleBrowseBoxHeaderBar(
            const css::uno::Reference< css::accessibility::XAccessible >& rxParent,
            vcl::IAccessibleTableProvider& _rOwningTable,
            AccessibleBrowseBoxObjType _eObjType
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleBrowseBoxTableCell(
            const css::uno::Reference< css::accessibility::XAccessible >& _rxParent,
            vcl::IAccessibleTableProvider& _rBrowseBox,
            sal_Int32 _nRowId,
            sal_uInt16 _nColId,
            sal_Int32 _nOffset
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleBrowseBoxHeaderCell(
            sal_Int32 _nColumnRowId,
            const css::uno::Reference< css::accessibility::XAccessible >& rxParent,
            vcl::IAccessibleTableProvider& _rBrowseBox,
            AccessibleBrowseBoxObjType  _eObjType
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createAccessibleCheckBoxCell(
            const css::uno::Reference< css::accessibility::XAccessible >& _rxParent,
            vcl::IAccessibleTableProvider& _rBrowseBox,
            sal_Int32 _nRowPos,
            sal_uInt16 _nColPos,
            const TriState& _eState,
            bool _bIsTriState
        ) const override;

    virtual css::uno::Reference< css::accessibility::XAccessible >
        createEditBrowseBoxTableCellAccess(
            const css::uno::Reference< css::accessibility::XAccessible >& _rxParent,
            const css::uno::Reference< css::accessibility::XAccessible >& _rxControlAccessible,
            const css::uno::Reference< css::awt::XWindow >& _rxFocusWindow,
            vcl::IAccessibleTableProvider& _rBrowseBox,
            sal_Int32 _nRowPos,
            sal_uInt16 _nColPos
        ) const override;

protected:
    virtual ~AccessibleFactory() override;
};

AccessibleFactory::AccessibleFactory()
{
}

AccessibleFactory::~AccessibleFactory()
{
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(PushButton* pButton)
{
    return new VCLXAccessibleButton(pButton);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(CheckBox* pCheckBox)
{
    return new VCLXAccessibleCheckBox(pCheckBox);
}

Reference<XAccessibleContext> AccessibleFactory::createAccessibleContext(RadioButton* pRadioButton)
{
    return new VCLXAccessibleRadioButton(pRadioButton);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(ListBox* pListBox)
{
    bool bIsDropDownBox = false;
    if (pListBox)
        bIsDropDownBox = ((pListBox->GetStyle() & WB_DROPDOWN) == WB_DROPDOWN);

    if ( bIsDropDownBox )
        return new VCLXAccessibleDropDownListBox(pListBox);
    else
        return new VCLXAccessibleListBox(pListBox);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(FixedText* pFixedText)
{
    return new VCLXAccessibleFixedText(pFixedText);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(FixedHyperlink* pFixedHyperlink)
{
    return new VCLXAccessibleFixedHyperlink(pFixedHyperlink);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(ScrollBar* pScrollBar)
{
    return new VCLXAccessibleScrollBar(pScrollBar);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(Edit* pEdit)
{
    return new VCLXAccessibleEdit(pEdit);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(ComboBox* pComboBox)
{
    bool bIsDropDownBox = false;
    if (pComboBox)
        bIsDropDownBox = ((pComboBox->GetStyle() & WB_DROPDOWN) == WB_DROPDOWN);

    if ( bIsDropDownBox )
        return new VCLXAccessibleDropDownComboBox(pComboBox);
    else
        return new VCLXAccessibleComboBox(pComboBox);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(vcl::Window* pWindow)
{
    if (!pWindow)
        return nullptr;

    WindowType nType = pWindow->GetType();

    if ( nType == WindowType::MENUBARWINDOW || pWindow->IsMenuFloatingWindow() || pWindow->IsToolbarFloatingWindow() )
    {
        Reference< XAccessible > xAcc( pWindow->GetAccessible() );
        if ( xAcc.is() )
        {
            Reference<XAccessibleContext > xContext(xAcc->getAccessibleContext());
            if ( pWindow->GetType() == WindowType::MENUBARWINDOW ||
                ( xContext.is() && xContext->getAccessibleRole() == AccessibleRole::POPUP_MENU ) )
            {
                return xContext;
            }
        }
    }

    else if ( nType == WindowType::STATUSBAR )
    {
        return new VCLXAccessibleStatusBar(pWindow);
    }

    else if ( nType == WindowType::TABCONTROL )
    {
        return new VCLXAccessibleTabControl(pWindow);
    }

    else if ( nType == WindowType::TABPAGE && pWindow->GetAccessibleParentWindow() && pWindow->GetAccessibleParentWindow()->GetType() == WindowType::TABCONTROL )
    {
        return new VCLXAccessibleTabPageWindow(pWindow);
    }

    else if ( nType == WindowType::FLOATINGWINDOW )
    {
        return new FloatingWindowAccessible(pWindow);
    }

    else if ( nType == WindowType::BORDERWINDOW && hasFloatingChild( pWindow ) )
    {
        return new FloatingWindowAccessible(pWindow);
    }

    else if ( ( nType == WindowType::HELPTEXTWINDOW ) || ( nType == WindowType::FIXEDLINE ) )
    {
        return new VCLXAccessibleFixedText(pWindow);
    }
    else
    {
        return new VCLXAccessibleComponent(pWindow);
    }

    return nullptr;
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(ToolBox* pToolBox)
{
    return new VCLXAccessibleToolBox(pToolBox);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(HeaderBar* pHeaderBar)
{
    return new VCLXAccessibleHeaderBar(pHeaderBar);
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleContext(FormattedField* pFormattedField)
{
    return new SVTXAccessibleNumericField(pFormattedField);
}

vcl::IAccessibleTabListBox* AccessibleFactory::createAccessibleTabListBox(
    const Reference< XAccessible >& rxParent, SvHeaderTabListBox& rBox ) const
{
    return new AccessibleTabListBox( rxParent, rBox );
}

rtl::Reference<vcl::IAccessibleBrowseBox> AccessibleFactory::createAccessibleBrowseBox(
    const Reference< XAccessible >& _rxParent, vcl::IAccessibleTableProvider& _rBrowseBox ) const
{
    return new AccessibleBrowseBoxAccess( _rxParent, _rBrowseBox );
}

rtl::Reference<IAccessibleTableControl> AccessibleFactory::createAccessibleTableControl(
    const Reference< XAccessible >& _rxParent, IAccessibleTable& _rTable ) const
{
    return new AccessibleGridControlAccess( _rxParent, _rTable );
}

Reference< XAccessible > AccessibleFactory::createAccessibleIconChoiceCtrl(
    SvtIconChoiceCtrl& _rIconCtrl, const Reference< XAccessible >& _xParent ) const
{
    return new AccessibleIconChoiceCtrl( _rIconCtrl, _xParent );
}

Reference< XAccessible > AccessibleFactory::createAccessibleTabBar( TabBar& _rTabBar ) const
{
    return new AccessibleTabBar( &_rTabBar );
}

Reference< XAccessibleContext > AccessibleFactory::createAccessibleTextWindowContext(
    vcl::Window* pWindow, TextEngine& rEngine, TextView& rView) const
{
    return new Document(pWindow, rEngine, rView );
}

Reference< XAccessible > AccessibleFactory::createAccessibleTreeListBox(
    SvTreeListBox& _rListBox, const Reference< XAccessible >& _xParent ) const
{
    return new AccessibleListBox( _rListBox, _xParent );
}

Reference< XAccessible > AccessibleFactory::createAccessibleIconView(
    SvTreeListBox& _rListBox, const Reference< XAccessible >& _xParent ) const
{
    return new AccessibleIconView( _rListBox, _xParent );
}

Reference< XAccessible > AccessibleFactory::createAccessibleBrowseBoxHeaderBar(
    const Reference< XAccessible >& rxParent, vcl::IAccessibleTableProvider& _rOwningTable,
    AccessibleBrowseBoxObjType _eObjType ) const
{
    return new AccessibleBrowseBoxHeaderBar( rxParent, _rOwningTable, _eObjType );
}

Reference< XAccessible > AccessibleFactory::createAccessibleBrowseBoxTableCell(
    const Reference< XAccessible >& _rxParent, vcl::IAccessibleTableProvider& _rBrowseBox,
    sal_Int32 _nRowId, sal_uInt16 _nColId, sal_Int32 _nOffset) const
{
    return new AccessibleBrowseBoxTableCell( _rxParent, _rBrowseBox,
        _nRowId, _nColId, _nOffset );
}

Reference< XAccessible > AccessibleFactory::createAccessibleBrowseBoxHeaderCell(
    sal_Int32 _nColumnRowId, const Reference< XAccessible >& rxParent, vcl::IAccessibleTableProvider& _rBrowseBox,
    AccessibleBrowseBoxObjType  _eObjType) const
{
    return new AccessibleBrowseBoxHeaderCell( _nColumnRowId, rxParent, _rBrowseBox,
        _eObjType);
}

Reference< XAccessible > AccessibleFactory::createAccessibleCheckBoxCell(
    const Reference< XAccessible >& _rxParent, vcl::IAccessibleTableProvider& _rBrowseBox,
    sal_Int32 _nRowPos, sal_uInt16 _nColPos, const TriState& _eState, bool _bIsTriState) const
{
    return new AccessibleCheckBoxCell( _rxParent, _rBrowseBox,
        _nRowPos, _nColPos, _eState, _bIsTriState );
}

Reference< XAccessible > AccessibleFactory::createEditBrowseBoxTableCellAccess(
    const Reference< XAccessible >& _rxParent, const Reference< XAccessible >& _rxControlAccessible,
    const Reference< XWindow >& _rxFocusWindow, vcl::IAccessibleTableProvider& _rBrowseBox,
    sal_Int32 _nRowPos, sal_uInt16 _nColPos ) const
{
    return new EditBrowseBoxTableCellAccess( _rxParent, _rxControlAccessible,
        _rxFocusWindow, _rBrowseBox, _nRowPos, _nColPos );
}

} // anonymous namespace

#if HAVE_FEATURE_DESKTOP

/// anonymous implementation namespace
namespace {

class GetStandardAccessibleFactoryService:
    public ::cppu::WeakImplHelper<
        css::lang::XServiceInfo,
        css::lang::XUnoTunnel>
{
public:
    // css::lang::XServiceInfo:
    virtual OUString SAL_CALL getImplementationName() override
        { return u"com.sun.star.accessibility.comp.GetStandardAccessibleFactoryService"_ustr; }
    virtual sal_Bool SAL_CALL supportsService(const OUString & serviceName) override
        { return cppu::supportsService(this, serviceName); }
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() override
        { return { u"com.sun.star.accessibility.GetStandardAccessibleFactoryService"_ustr }; }

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::css::uno::Sequence< ::sal_Int8 >& /*aIdentifier*/ ) override
    {
        ::toolkit::IAccessibleFactory* pFactory = new AccessibleFactory;
        pFactory->acquire();
        return reinterpret_cast<sal_Int64>(pFactory);
    }
};

} // closing anonymous implementation namespace

/* this is the entry point to retrieve a factory for the toolkit-level Accessible/Contexts supplied
    by this library

    This function implements the factory function needed in toolkit
    (of type GetStandardAccComponentFactory).
*/
extern "C"
{
    SAL_DLLPUBLIC_EXPORT css::uno::XInterface *
    com_sun_star_accessibility_GetStandardAccessibleFactoryService_get_implementation(
        css::uno::XComponentContext *,
        css::uno::Sequence<css::uno::Any> const &)
    {
        return cppu::acquire(new GetStandardAccessibleFactoryService);
    }
}

/** this is the entry point to retrieve a factory for the svtools-level Accessible/Contexts supplied
    by this library

    This function implements the factory function needed in svtools
    (of type GetSvtAccessibilityComponentFactory).
*/
extern "C"
{
    SAL_DLLPUBLIC_EXPORT void* getSvtAccessibilityComponentFactory()
    {
        ::vcl::IAccessibleFactory* pFactory = new AccessibleFactory;
        pFactory->acquire();
        return pFactory;
    }
}
#endif // HAVE_FEATURE_DESKTOP

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
