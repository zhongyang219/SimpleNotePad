QT += core gui widgets

CONFIG += c++17
TARGET = ScintillaQt

TEMPLATE = lib
DEFINES += MAKING_LIBRARY

CONFIG(debug, debug|release): {
    DESTDIR = $$PWD/../../bin/debug
}
else {
    DESTDIR = $$PWD/../../bin/release
}

INCLUDEPATH += $$PWD/../include \
    $$PWD/../lexlib/ \
    $$PWD/../src/
DEFINES += SCINTILLA_QT=1 MAKING_LIBRARY=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

HEADERS += \
    ../include/ILexer.h \
    ../include/ILoader.h \
    ../include/Platform.h \
    ../include/SciLexer.h \
    ../include/Sci_Position.h \
    ../include/Scintilla.h \
    ../include/ScintillaWidget.h \
    ../lexlib/Accessor.h \
    ../lexlib/CatalogueModules.h \
    ../lexlib/CharacterCategory.h \
    ../lexlib/CharacterSet.h \
    ../lexlib/DefaultLexer.h \
    ../lexlib/LexAccessor.h \
    ../lexlib/LexerBase.h \
    ../lexlib/LexerModule.h \
    ../lexlib/LexerNoExceptions.h \
    ../lexlib/LexerSimple.h \
    ../lexlib/OptionSet.h \
    ../lexlib/PropSetSimple.h \
    ../lexlib/SparseState.h \
    ../lexlib/StringCopy.h \
    ../lexlib/StyleContext.h \
    ../lexlib/SubStyles.h \
    ../lexlib/WordList.h \
    ../src/AutoComplete.h \
    ../src/CallTip.h \
    ../src/CaseConvert.h \
    ../src/CaseFolder.h \
    ../src/Catalogue.h \
    ../src/CellBuffer.h \
    ../src/CharClassify.h \
    ../src/ContractionState.h \
    ../src/DBCS.h \
    ../src/Decoration.h \
    ../src/Document.h \
    ../src/EditModel.h \
    ../src/EditView.h \
    ../src/Editor.h \
    ../src/ElapsedPeriod.h \
    ../src/ExternalLexer.h \
    ../src/FontQuality.h \
    ../src/Indicator.h \
    ../src/IntegerRectangle.h \
    ../src/KeyMap.h \
    ../src/LineMarker.h \
    ../src/MarginView.h \
    ../src/Partitioning.h \
    ../src/PerLine.h \
    ../src/Position.h \
    ../src/PositionCache.h \
    ../src/RESearch.h \
    ../src/RunStyles.h \
    ../src/ScintillaBase.h \
    ../src/Selection.h \
    ../src/SparseVector.h \
    ../src/SplitVector.h \
    ../src/Style.h \
    ../src/UniConversion.h \
    ../src/UniqueString.h \
    ../src/ViewStyle.h \
    ../src/XPM.h \
    ScintillaEdit/ScintillaDocument.h \
    ScintillaEditBase/PlatQt.h \
    ScintillaEditBase/ScintillaEditBase.h \
    ScintillaEditBase/ScintillaQt.h

SOURCES += \
    ../lexers/LexA68k.cxx \
    ../lexers/LexAPDL.cxx \
    ../lexers/LexASY.cxx \
    ../lexers/LexAU3.cxx \
    ../lexers/LexAVE.cxx \
    ../lexers/LexAVS.cxx \
    ../lexers/LexAbaqus.cxx \
    ../lexers/LexAda.cxx \
    ../lexers/LexAsm.cxx \
    ../lexers/LexAsn1.cxx \
    ../lexers/LexBaan.cxx \
    ../lexers/LexBash.cxx \
    ../lexers/LexBasic.cxx \
    ../lexers/LexBatch.cxx \
    ../lexers/LexBibTeX.cxx \
    ../lexers/LexBullant.cxx \
    ../lexers/LexCIL.cxx \
    ../lexers/LexCLW.cxx \
    ../lexers/LexCOBOL.cxx \
    ../lexers/LexCPP.cxx \
    ../lexers/LexCSS.cxx \
    ../lexers/LexCaml.cxx \
    ../lexers/LexCmake.cxx \
    ../lexers/LexCoffeeScript.cxx \
    ../lexers/LexConf.cxx \
    ../lexers/LexCrontab.cxx \
    ../lexers/LexCsound.cxx \
    ../lexers/LexD.cxx \
    ../lexers/LexDMAP.cxx \
    ../lexers/LexDMIS.cxx \
    ../lexers/LexDataflex.cxx \
    ../lexers/LexDiff.cxx \
    ../lexers/LexECL.cxx \
    ../lexers/LexEDIFACT.cxx \
    ../lexers/LexEScript.cxx \
    ../lexers/LexEiffel.cxx \
    ../lexers/LexErlang.cxx \
    ../lexers/LexErrorList.cxx \
    ../lexers/LexFlagship.cxx \
    ../lexers/LexForth.cxx \
    ../lexers/LexFortran.cxx \
    ../lexers/LexGAP.cxx \
    ../lexers/LexGui4Cli.cxx \
    ../lexers/LexHTML.cxx \
    ../lexers/LexHaskell.cxx \
    ../lexers/LexHex.cxx \
    ../lexers/LexHexView.cxx \
    ../lexers/LexHollywood.cxx \
    ../lexers/LexIndent.cxx \
    ../lexers/LexInno.cxx \
    ../lexers/LexJSON.cxx \
    ../lexers/LexKVIrc.cxx \
    ../lexers/LexKix.cxx \
    ../lexers/LexLaTeX.cxx \
    ../lexers/LexLisp.cxx \
    ../lexers/LexLout.cxx \
    ../lexers/LexLua.cxx \
    ../lexers/LexMMIXAL.cxx \
    ../lexers/LexMPT.cxx \
    ../lexers/LexMSSQL.cxx \
    ../lexers/LexMagik.cxx \
    ../lexers/LexMake.cxx \
    ../lexers/LexMarkdown.cxx \
    ../lexers/LexMatlab.cxx \
    ../lexers/LexMaxima.cxx \
    ../lexers/LexMetapost.cxx \
    ../lexers/LexModula.cxx \
    ../lexers/LexMySQL.cxx \
    ../lexers/LexNim.cxx \
    ../lexers/LexNimrod.cxx \
    ../lexers/LexNsis.cxx \
    ../lexers/LexNull.cxx \
    ../lexers/LexOScript.cxx \
    ../lexers/LexOpal.cxx \
    ../lexers/LexPB.cxx \
    ../lexers/LexPLM.cxx \
    ../lexers/LexPO.cxx \
    ../lexers/LexPOV.cxx \
    ../lexers/LexPS.cxx \
    ../lexers/LexPascal.cxx \
    ../lexers/LexPerl.cxx \
    ../lexers/LexPowerPro.cxx \
    ../lexers/LexPowerShell.cxx \
    ../lexers/LexProgress.cxx \
    ../lexers/LexProps.cxx \
    ../lexers/LexPython.cxx \
    ../lexers/LexR.cxx \
    ../lexers/LexRaku.cxx \
    ../lexers/LexRebol.cxx \
    ../lexers/LexRegistry.cxx \
    ../lexers/LexRuby.cxx \
    ../lexers/LexRust.cxx \
    ../lexers/LexSAS.cxx \
    ../lexers/LexSML.cxx \
    ../lexers/LexSQL.cxx \
    ../lexers/LexSTTXT.cxx \
    ../lexers/LexScriptol.cxx \
    ../lexers/LexSmalltalk.cxx \
    ../lexers/LexSorcus.cxx \
    ../lexers/LexSpecman.cxx \
    ../lexers/LexSpice.cxx \
    ../lexers/LexStata.cxx \
    ../lexers/LexTACL.cxx \
    ../lexers/LexTADS3.cxx \
    ../lexers/LexTAL.cxx \
    ../lexers/LexTCL.cxx \
    ../lexers/LexTCMD.cxx \
    ../lexers/LexTeX.cxx \
    ../lexers/LexTxt2tags.cxx \
    ../lexers/LexVB.cxx \
    ../lexers/LexVHDL.cxx \
    ../lexers/LexVerilog.cxx \
    ../lexers/LexVisualProlog.cxx \
    ../lexers/LexX12.cxx \
    ../lexers/LexYAML.cxx \
    ../lexlib/Accessor.cxx \
    ../lexlib/CharacterCategory.cxx \
    ../lexlib/CharacterSet.cxx \
    ../lexlib/DefaultLexer.cxx \
    ../lexlib/LexerBase.cxx \
    ../lexlib/LexerModule.cxx \
    ../lexlib/LexerNoExceptions.cxx \
    ../lexlib/LexerSimple.cxx \
    ../lexlib/PropSetSimple.cxx \
    ../lexlib/StyleContext.cxx \
    ../lexlib/WordList.cxx \
    ../src/AutoComplete.cxx \
    ../src/CallTip.cxx \
    ../src/CaseConvert.cxx \
    ../src/CaseFolder.cxx \
    ../src/Catalogue.cxx \
    ../src/CellBuffer.cxx \
    ../src/CharClassify.cxx \
    ../src/ContractionState.cxx \
    ../src/DBCS.cxx \
    ../src/Decoration.cxx \
    ../src/Document.cxx \
    ../src/EditModel.cxx \
    ../src/EditView.cxx \
    ../src/Editor.cxx \
    ../src/ExternalLexer.cxx \
    ../src/Indicator.cxx \
    ../src/KeyMap.cxx \
    ../src/LineMarker.cxx \
    ../src/MarginView.cxx \
    ../src/PerLine.cxx \
    ../src/PositionCache.cxx \
    ../src/RESearch.cxx \
    ../src/RunStyles.cxx \
    ../src/ScintillaBase.cxx \
    ../src/Selection.cxx \
    ../src/Style.cxx \
    ../src/UniConversion.cxx \
    ../src/UniqueString.cxx \
    ../src/ViewStyle.cxx \
    ../src/XPM.cxx \
    ScintillaEdit/ScintillaDocument.cpp \
    ScintillaEditBase/PlatQt.cpp \
    ScintillaEditBase/ScintillaEditBase.cpp \
    ScintillaEditBase/ScintillaQt.cpp
