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
#pragma once

#include <svtools/svtdllapi.h>
#include <rtl/ustring.hxx>
#include <tools/color.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <unotools/options.hxx>
#include <memory>


namespace svtools{
enum ColorConfigEntry : int
{
    DOCCOLOR            ,
    DOCBOUNDARIES       ,
    APPBACKGROUND       ,
    TABLEBOUNDARIES     ,
    FONTCOLOR           ,
    LINKS               ,
    LINKSVISITED        ,
    SPELL         ,
    GRAMMAR       ,
    SMARTTAGS     ,
    SHADOWCOLOR         ,
    WRITERTEXTGRID      ,
    WRITERFIELDSHADINGS ,
    WRITERIDXSHADINGS         ,
    WRITERDIRECTCURSOR        , // unused
    WRITERSCRIPTINDICATOR,
    WRITERSECTIONBOUNDARIES,
    WRITERHEADERFOOTERMARK,
    WRITERPAGEBREAKS,
    WRITERNONPRINTCHARS,
    HTMLSGML            ,
    HTMLCOMMENT         ,
    HTMLKEYWORD         ,
    HTMLUNKNOWN         ,
    CALCGRID            ,
    CALCCELLFOCUS       ,
    CALCPAGEBREAK       ,
    CALCPAGEBREAKMANUAL,
    CALCPAGEBREAKAUTOMATIC,
    CALCHIDDENROWCOL    ,
    CALCTEXTOVERFLOW    ,
    CALCCOMMENTS        ,
    CALCDETECTIVE       ,
    CALCDETECTIVEERROR       ,
    CALCREFERENCE       ,
    CALCNOTESBACKGROUND     ,
    CALCVALUE,
    CALCFORMULA,
    CALCTEXT,
    CALCPROTECTEDBACKGROUND,
    DRAWGRID            ,
    AUTHOR1,
    AUTHOR2,
    AUTHOR3,
    AUTHOR4,
    AUTHOR5,
    AUTHOR6,
    AUTHOR7,
    AUTHOR8,
    AUTHOR9,
    BASICEDITOR,
    BASICIDENTIFIER,
    BASICCOMMENT   ,
    BASICNUMBER    ,
    BASICSTRING    ,
    BASICOPERATOR  ,
    BASICKEYWORD   ,
    BASICERROR   ,
    SQLIDENTIFIER,
    SQLNUMBER,
    SQLSTRING,
    SQLOPERATOR,
    SQLKEYWORD,
    SQLPARAMETER,
    SQLCOMMENT,

    // colors for Libreoffice Theme (Application)
    WINDOWCOLOR         ,
    WINDOWTEXTCOLOR     ,
    BASECOLOR           ,
    BUTTONCOLOR         ,
    BUTTONTEXTCOLOR     ,
    ACCENTCOLOR         ,
    DISABLEDCOLOR       ,
    DISABLEDTEXTCOLOR   ,
    SHADECOLOR         ,
    SEPARATORCOLOR      ,
    FACECOLOR           ,
    ACTIVECOLOR         ,
    ACTIVETEXTCOLOR     ,
    ACTIVEBORDERCOLOR   ,
    FIELDCOLOR          ,
    MENUBARCOLOR        ,
    MENUBARTEXTCOLOR    ,
    MENUBARHIGHLIGHTCOLOR ,
    MENUBARHIGHLIGHTTEXTCOLOR ,
    MENUCOLOR           ,
    MENUTEXTCOLOR       ,
    MENUHIGHLIGHTCOLOR  ,
    MENUHIGHLIGHTTEXTCOLOR ,
    MENUBORDERCOLOR     ,
    INACTIVECOLOR       ,
    INACTIVETEXTCOLOR   ,
    INACTIVEBORDERCOLOR ,

    ColorConfigEntryCount
};

constexpr OUString AUTOMATIC_COLOR_SCHEME = u"COLOR_SCHEME_LIBREOFFICE_AUTOMATIC"_ustr;
constexpr size_t THEME_APPLICATION_COLORS_COUNT = ColorConfigEntryCount - WINDOWCOLOR;

class ColorConfig_Impl;
struct ColorConfigValue
{

    bool        bIsVisible; //validity depends on the element type
    ::Color     nColor;
    ColorConfigValue() : bIsVisible(false), nColor(0) {}
    ColorConfigValue(const Color& rColor, bool bVisible) : bIsVisible(bVisible), nColor(rColor) {}
    bool operator !=(const ColorConfigValue& rCmp) const
        { return nColor != rCmp.nColor || bIsVisible != rCmp.bIsVisible;}
};

class SAL_WARN_UNUSED SVT_DLLPUBLIC ColorConfig final :
    public utl::detail::Options
{
    friend class ColorConfig_Impl;
private:
    static ColorConfig_Impl* m_pImpl;
public:
    ColorConfig();
    virtual ~ColorConfig() override;

    // get the configured value - if bSmart is set the default color setting is provided
    // instead of the automatic color
    ColorConfigValue        GetColorValue(ColorConfigEntry eEntry, bool bSmart = true) const;
    // -1 gets the default color on current mod.
    //  0 gets the default color on light mod.
    //  1 gets the default color on dark mod.
    static Color            GetDefaultColor(ColorConfigEntry eEntry, int nMod = -1);
    static const OUString& GetCurrentSchemeName();

    static void             LoadThemeColorsFromRegistry();
    static void             SetupTheme();
};

class SVT_DLLPUBLIC EditableColorConfig
{
    std::unique_ptr<ColorConfig_Impl> m_pImpl;
    bool                m_bModified;
public:
    EditableColorConfig();
    ~EditableColorConfig();

    css::uno::Sequence< OUString >  GetSchemeNames() const;
    void                        DeleteScheme(const OUString& rScheme );
    void                        AddScheme(const OUString& rScheme );
    void                        LoadScheme(const OUString& rScheme );
    const OUString&             GetCurrentSchemeName() const;
    void                        SetCurrentSchemeName(const OUString& rScheme);

    const ColorConfigValue&     GetColorValue(ColorConfigEntry eEntry) const;
    void                        SetColorValue(ColorConfigEntry eEntry, const ColorConfigValue& rValue);
    void                        SetModified();
    void                        ClearModified() {m_bModified = false;}
    bool                        IsModified() const {return m_bModified;}
    void                        Commit();

    void                        DisableBroadcast();
    void                        EnableBroadcast();
};
}//namespace svtools

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
