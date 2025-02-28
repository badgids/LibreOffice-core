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

#ifndef INCLUDED_TOOLKIT_HELPER_ACCESSIBLEFACTORY_HXX
#define INCLUDED_TOOLKIT_HELPER_ACCESSIBLEFACTORY_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <salhelper/simplereferenceobject.hxx>

namespace com::sun::star::accessibility {
    class XAccessible;
    class XAccessibleContext;
}

namespace vcl {
    class Window;
}

class ComboBox;
class Edit;
class FixedHyperlink;
class FixedText;
class FormattedField;
class HeaderBar;
class ListBox;
class RadioButton;
class ScrollBar;

#include <vcl/toolbox.hxx>
#include <vcl/toolkit/button.hxx>


namespace toolkit
{


    /** a function which is able to create a factory for the standard Accessible/Context
        components needed for standard VCL controls

        The returned pointer denotes an instance of the IAccessibleFactory, which has been acquired
        <em>once</em>. The caller is responsible for holding this reference as long as it needs the
        factory, and release it afterwards.
    */
    typedef void* (* GetStandardAccComponentFactory)( );


    //= IAccessibleFactory

    class IAccessibleFactory : public virtual salhelper::SimpleReferenceObject
    {
    public:
        /** creates an accessible context for a button window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(PushButton* pButton) = 0;

        /** creates an accessible context for a checkbox window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(CheckBox* pCheckBox) = 0;

        /** creates an accessible context for a radio button window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(RadioButton* pRadioButton) = 0;

        /** creates an accessible context for a listbox window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(ListBox* pListBox) = 0;

        /** creates an accessible context for a fixed hyperlink window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(FixedHyperlink* pFixedHyperlink) = 0;

        /** creates an accessible context for a fixed text window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(FixedText* pFixedText) = 0;

        /** creates an accessible context for a scrollbar window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(ScrollBar* pScrollBar) = 0;

        /** creates an accessible context for an edit window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(Edit* pEdit) = 0;

        /** creates an accessible context for a combo box window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(ComboBox* pComboBox) = 0;

        /** creates an accessible context for a toolbox window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(ToolBox* pToolBox) = 0;

        /** creates an accessible context for a headerbar window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(HeaderBar* pHeaderBar) = 0;

        /** creates an accessible context for a numeric field
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(FormattedField* pFormattedField) = 0;

        /** creates an accessible context for a generic window
        */
        virtual css::uno::Reference< css::accessibility::XAccessibleContext >
            createAccessibleContext(vcl::Window* pWindow) = 0;

    protected:
        virtual ~IAccessibleFactory() override {}
    };


}   // namespace toolkit


#endif // INCLUDED_TOOLKIT_HELPER_ACCESSIBLEFACTORY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
